/**
 * \file usb.c
 * \brief This encapsulates all USB configuration, interfacing, etc.
 *  The source here is a mix of example code from the
 *  nxp_lpcxpresso_11u37_usbd_rom_cdc_uart example and originally created 
 *  source. I am including only NXP's license for simplcity. 
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013 and Gregory Gluszek 2017 (modifications)
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "usb.h"

#include "buttons.h"
#include "adc_read.h"
#include "trackpad.h"

//TODO: straighten out weird circular includes? We cannot include usbd/usbd_core.h, even though that's what we want at this point...
//#include "usbd/usbd_core.h"
#include "app_usbd_cfg.h"

#include "chip.h"

#include "led_ctrl.h"

#include <string.h>
#include <stdio.h>

#if (__REDLIB_INTERFACE_VERSION__ >= 20000)
/* We are using new Redlib_v2 semihosting interface */
	#define WRITEFUNC __sys_write
	#define READFUNC __sys_readc
#else
/* We are using original Redlib semihosting interface */
	#define WRITEFUNC __write
	#define READFUNC __readc
#endif

const USBD_API_T *g_pUsbApi; //!< Through a series of non-ideal associations
	//!< this is used to access boot ROM code for USB functions. See
	//!< calls using USBD_API to access this. 

static USBD_HANDLE_T usbHandle; //!< Handle for interacting with the USB device

/**
 * Handle interrupt from USB0.
 *
 * \return None.
 */
void USB_IRQHandler(void)
{
	uint32_t *addr = (uint32_t *) LPC_USB->EPLISTSTART;

	/*	WORKAROUND for artf32289 ROM driver BUG:
	    As part of USB specification the device should respond
	    with STALL condition for any unsupported setup packet. The host will send
	    new setup packet/request on seeing STALL condition for EP0 instead of sending
	    a clear STALL request. Current driver in ROM doesn't clear the STALL
	    condition on new setup packet which should be fixed.
	 */
	if ( LPC_USB->DEVCMDSTAT & _BIT(8) ) {	/* if setup packet is received */
		addr[0] &= ~(_BIT(29));	/* clear EP0_OUT stall */
		addr[2] &= ~(_BIT(29));	/* clear EP0_IN stall */
	}
	USBD_API->hw->ISR(usbHandle);
}


#if (FIRMWARE_BEHAVIOR == DEV_BOARD_FW)

// Structure containing Virtual Comm port control data.
typedef struct {
	USBD_HANDLE_T usbHandle; //!< Handle to USB device stack 
	USBD_HANDLE_T cdcHandle; //!< Handle to Communications Device Class controller

	uint8_t* rxFifo; //!< Buffer treated as a FIFO which stores character
		//!< data that has been received via USB CDC UART
	volatile uint32_t rxWrapIdx; //!< Defines where rxFifo wraps around. 
		//!< This index and anything beyond it in rxFifo does not have 
		//!< valid data to read.  We need this because ReadEP can return
		//!< up to USB_MAX_PACKET_SZ bytes, but we don't know ahead of 
		//!< time how many we are actually getting.  This allows us to 
		//!< maximize memory usage, while not overflowing rxFifo.
	volatile uint32_t rxRdIdx; //!< Defines where next character is read
		//!< from in rxFifo. If rxRdIdx == rxWrIdx FIFO is empty.
	volatile uint32_t rxWrIdx; //!< Defines where next character(s) are to
		//!< to be stored when received from USB CDC UART.
	uint32_t rxStalled; //!< Flag that we did not have enough room for 
		//!< incoming data in rxFifo and did not call ReadEP. We will
		//!< need to call ReadEP to get the buffered data and have 
		//!< interrupts start working again once room opens in txFIFO.
		//!< Note that even though we do not call ReadEP, incoming 
		//!< USB packets may be dropped and into data lost. Still
		//!< looking for solutions to fix this.

	volatile int txBusy; //!< Indicates transmission is in progress. This 
		//!< does not guarantee that txFifo will be drained (use 
		//!< usb_flush() for this).
	uint8_t* txFifo; //!< Buffer treated as a FIFO which stores character
		//!< data to be transmitted via USB CDC UART
	uint32_t txRdIdx; //!< The index of where the next read sample
		//!< exists in txFifo
	uint32_t txWrIdx; //!< The index of where the next write 
		//!< sample will be put in txFifo
	uint32_t txSent; //!< Number of bytes WriteEP reports it last 
		//!< sent.
} UsbUartData;

static const uint32_t USB_MAX_PACKET_SZ = USB_FS_MAX_BULK_PACKET; //!< Maximum 
	//!< number of bytes in packet that can be sent or received via USB.

static const uint32_t USB_UART_TXFIFO_SZ = 256; //!< Number of bytes in txFifo.
	//!< This must be a power of 2!
static const uint32_t USB_UART_TXFIFO_THRESH = 128; //!< Determines when
	//!< transmission of txFifo data starts automatically.

static const uint32_t USB_UART_RXFIFO_SZ = 256; //!< Number of bytes in rxFifo.

static UsbUartData usbUartData; //!< Virtual Comm port control data 
	//!< instance. 

/**
 * USB Standard Device Descriptor
 */
ALIGNED(4) const uint8_t USB_DeviceDescriptor[] = {
	USB_DEVICE_DESC_SIZE, /* bLength */
	USB_DEVICE_DESCRIPTOR_TYPE, /* bDescriptorType */
	WBVAL(0x0200), /* bcdUSB */
	0xEF, /* bDeviceClass */
	0x02, /* bDeviceSubClass */
	0x01, /* bDeviceProtocol */
	USB_MAX_PACKET0, /* bMaxPacketSize0 */
	WBVAL(0x1FC9), /* idVendor */
	WBVAL(0x0083), /* idProduct */
	WBVAL(0x0100), /* bcdDevice */
	0x01, /* iManufacturer */
	0x02, /* iProduct */
	0x03, /* iSerialNumber */
	0x01 /* bNumConfigurations */
};

/**
 * USB FSConfiguration Descriptor
 * All Descriptors (Configuration, Interface, Endpoint, Class, Vendor)
 */
ALIGNED(4) uint8_t USB_FsConfigDescriptor[] = {
	/* Configuration 1 */
	USB_CONFIGURATION_DESC_SIZE, /* bLength */
	USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType */
	WBVAL( /* wTotalLength */
		USB_CONFIGURATION_DESC_SIZE     +
		USB_INTERFACE_ASSOC_DESC_SIZE   +	/* interface association descriptor */
		USB_INTERFACE_DESC_SIZE         +	/* communication control interface */
		0x0013                          +	/* CDC functions */
		1 * USB_ENDPOINT_DESC_SIZE      +	/* interrupt endpoint */
		USB_INTERFACE_DESC_SIZE         +	/* communication data interface */
		2 * USB_ENDPOINT_DESC_SIZE      +	/* bulk endpoints */
		0
		),
	0x02, /* bNumInterfaces */
	0x01, /* bConfigurationValue */
	0x00, /* iConfiguration */
	USB_CONFIG_SELF_POWERED, /* bmAttributes  */
	USB_CONFIG_POWER_MA(500), /* bMaxPower */

	/* Interface association descriptor IAD*/
	USB_INTERFACE_ASSOC_DESC_SIZE, /* bLength */
	USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE, /* bDescriptorType */
	USB_CDC_CIF_NUM, /* bFirstInterface */
	0x02, /* bInterfaceCount */
	CDC_COMMUNICATION_INTERFACE_CLASS, /* bFunctionClass */
	CDC_ABSTRACT_CONTROL_MODEL, /* bFunctionSubClass */
	0x00, /* bFunctionProtocol */
	0x04, /* iFunction */

	/* Interface 0, Alternate Setting 0, Communication class interface descriptor */
	USB_INTERFACE_DESC_SIZE, /* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
	USB_CDC_CIF_NUM, /* bInterfaceNumber: Number of Interface */
	0x00, /* bAlternateSetting: Alternate setting */
	0x01, /* bNumEndpoints: One endpoint used */
	CDC_COMMUNICATION_INTERFACE_CLASS, /* bInterfaceClass: Communication Interface Class */
	CDC_ABSTRACT_CONTROL_MODEL, /* bInterfaceSubClass: Abstract Control Model */
	0x00, /* bInterfaceProtocol: no protocol used */
	0x04, /* iInterface: */
	/* Header Functional Descriptor*/
	0x05, /* bLength: CDC header Descriptor size */
	CDC_CS_INTERFACE, /* bDescriptorType: CS_INTERFACE */
	CDC_HEADER, /* bDescriptorSubtype: Header Func Desc */
	WBVAL(CDC_V1_10), /* bcdCDC 1.10 */
	/* Call Management Functional Descriptor*/
	0x05, /* bFunctionLength */
	CDC_CS_INTERFACE, /* bDescriptorType: CS_INTERFACE */
	CDC_CALL_MANAGEMENT, /* bDescriptorSubtype: Call Management Func Desc */
	0x01, /* bmCapabilities: device handles call management */
	USB_CDC_DIF_NUM, /* bDataInterface: CDC data IF ID */
	/* Abstract Control Management Functional Descriptor*/
	0x04, /* bFunctionLength */
	CDC_CS_INTERFACE, /* bDescriptorType: CS_INTERFACE */
	CDC_ABSTRACT_CONTROL_MANAGEMENT, /* bDescriptorSubtype: Abstract Control Management desc */
	0x02, /* bmCapabilities: SET_LINE_CODING, GET_LINE_CODING, SET_CONTROL_LINE_STATE supported */
	/* Union Functional Descriptor*/
	0x05, /* bFunctionLength */
	CDC_CS_INTERFACE, /* bDescriptorType: CS_INTERFACE */
	CDC_UNION, /* bDescriptorSubtype: Union func desc */
	USB_CDC_CIF_NUM, /* bMasterInterface: Communication class interface is master */
	USB_CDC_DIF_NUM, /* bSlaveInterface0: Data class interface is slave 0 */
	/* Endpoint 1 Descriptor*/
	USB_ENDPOINT_DESC_SIZE, /* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType */
	USB_CDC_INT_EP, /* bEndpointAddress */
	USB_ENDPOINT_TYPE_INTERRUPT, /* bmAttributes */
	WBVAL(0x0010), /* wMaxPacketSize */
	0x02, /* 2ms */ /* bInterval */

	/* Interface 1, Alternate Setting 0, Data class interface descriptor*/
	USB_INTERFACE_DESC_SIZE, /* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
	USB_CDC_DIF_NUM, /* bInterfaceNumber: Number of Interface */
	0x00, /* bAlternateSetting: no alternate setting */
	0x02, /* bNumEndpoints: two endpoints used */
	CDC_DATA_INTERFACE_CLASS, /* bInterfaceClass: Data Interface Class */
	0x00, /* bInterfaceSubClass: no subclass available */
	0x00, /* bInterfaceProtocol: no protocol used */
	0x04, /* iInterface: */
	/* Endpoint, EP Bulk Out */
	USB_ENDPOINT_DESC_SIZE, /* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType */
	USB_CDC_OUT_EP,	 /* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK, /* bmAttributes */
	WBVAL(USB_FS_MAX_BULK_PACKET), /* wMaxPacketSize */
	0x00, /* bInterval: ignore for Bulk transfer */
	/* Endpoint, EP Bulk In */
	USB_ENDPOINT_DESC_SIZE, /* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType */
	USB_CDC_IN_EP, /* bEndpointAddress */
	USB_ENDPOINT_TYPE_BULK, /* bmAttributes */
	WBVAL(USB_FS_MAX_BULK_PACKET), /* wMaxPacketSize */
	0x00, /* bInterval: ignore for Bulk transfer */
	/* Terminator */
	0 /* bLength */
};

/**
 * USB String Descriptor (optional)
 */
ALIGNED(4) const uint8_t USB_StringDescriptor[] = {
	/* Index 0x00: LANGID Codes */
	0x04, /* bLength */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
	WBVAL(0x0409),	/* US English */ /* wLANGID */
	/* Index 0x01: Manufacturer */
	(21 * 2 + 2), /* bLength */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
	'O', 0,
	'p', 0,
	'e', 0,
	'n', 0,
	' ', 0,
	'S', 0,
	't', 0,
	'e', 0,
	'a', 0,
	'm', 0,
	' ', 0,
	'C', 0,
	'o', 0,
	'n', 0,
	't', 0,
	'r', 0,
	'o', 0,
	'l', 0,
	'l', 0,
	'e', 0,
	'r', 0,
	/* Index 0x02: Product */
	(9 * 2 + 2), /* bLength */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
	'V', 0,
	'C', 0,
	'O', 0,
	'M', 0,
	' ', 0,
	'P', 0,
	'o', 0,
	'r', 0,
	't', 0,
	/* Index 0x03: Serial Number */
	(4 * 2 + 2), /* bLength (8 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
	'1', 0,
	'7', 0,
	'8', 0,
	'9', 0,
	/* Index 0x04: Interface 1, Alternate Setting 0 */
	(4 * 2 + 2), /* bLength (4 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
	'V', 0,
	'C', 0,
	'O', 0,
	'M', 0,
};

/**
 * \param[in] uartData Contains details on Virtual Comm to transmit via.
 *
 * \return The number of bytes in txFifo ready to be sent.
 */
static uint32_t usbTxFifoNumBytes(const UsbUartData* uartData) {
	if (uartData->txWrIdx >= uartData->txRdIdx) {
		return uartData->txWrIdx - uartData->txRdIdx;
	}

	return 1 + uartData->txRdIdx + USB_UART_TXFIFO_SZ - uartData->txWrIdx;
}

/**
 * Start a (series of) transmission(s) via USB with as much txFifo data as
 *  possible. This will do nothing if a transmission is already in progress
 *  or txFIFO is empty.
 *
 * \param[inout] uartData Contains details on Virtual Comm to transmit via.
 *
 * \return None.
 */
static void usbUartTxStart(UsbUartData* uartData) {
	// Make sure we are not already busy with a transmit
	if (uartData->txBusy) {
		return;
	}

	// Mark that we are busy now
	uartData->txBusy = 1;

	// Check if there is any data to send
	uint32_t bytes_to_send = usbTxFifoNumBytes(uartData);

	// Make sure there is no wrap around in send request
	if (bytes_to_send + uartData->txRdIdx >= USB_UART_TXFIFO_SZ) {
		bytes_to_send = USB_UART_TXFIFO_SZ - uartData->txRdIdx;
	}

	// Make sure we actually have data to send
	if (!bytes_to_send) {
		uartData->txBusy = 0;
		return;
	}

	// Seems we can enter an error state (i.e. interrupt does not fire or
	//  keeps firing continuously) if we tell WriteEP() that it should send
	//  number of bytes larger than what is specified for wMaxPacketSize
	if (bytes_to_send > USB_MAX_PACKET_SZ) {
		bytes_to_send = USB_MAX_PACKET_SZ;
	}

	// Apparently IRQs need to be disabled around call to WriteEP. If they
	//  are not and other interrupts are active of (higher priority)
	//  (i.e. ADC interrupts) we can get repeated prints or dropped data... 
	//  Not sure why. Maybe related to built in CDC UART support?
	__disable_irq();

	// Send the data to the USB EP (the interrupt handler will adjust rxIdx)
	if (USB_IsConfigured(uartData->usbHandle)) {
		uartData->txSent = USBD_API->hw->WriteEP(uartData->usbHandle, 
			USB_CDC_IN_EP, &uartData->txFifo[uartData->txRdIdx], 
			bytes_to_send);
	}

	__enable_irq();

	// Just in case something went wrong
	if (!uartData->txSent) {
		uartData->txBusy = 0;
	}
}

/**
 * Queue character to be transmitted via USB CDC UART. This does not guarantee
 *  character will be sent upon function return (use usb_flush() to guarantee).
 *
 * \param character Character to write out via virtual UART.
 * 
 * \return Character queued. Function will stall until character is queued.
 */
int usb_putc(int character) {
	uint32_t next_wr_idx = 0;
	// Wait until FIFO is not full
	// Note: this is a wasteful "full" calculation as there is still
	//  one byte left in the FIFO, however, this makes calculating the
	//  number of bytes in the FIFO simpler.
	do {
		next_wr_idx = (usbUartData.txWrIdx + 1) % USB_UART_TXFIFO_SZ;
	} while (next_wr_idx == usbUartData.txRdIdx);

	// Put new character info FIFO
	usbUartData.txFifo[usbUartData.txWrIdx] = character;
	usbUartData.txWrIdx = next_wr_idx;

	// Check if transmit should be initiated
	if (usbUartData.txBusy) {
		// Transmit is already taking place. New character may or may
		//  not make it out
		return character;
	}

	// Request starting a new (series of) transmission(s) if the FIFO has
	//  filled up adequately
	if (usbTxFifoNumBytes(&usbUartData) >= USB_UART_TXFIFO_THRESH) {
		usbUartTxStart(&usbUartData);
	}

	return character;
}

/**
 * Queue the data in the buffer for output via USB CDC UART. Useful for cases
 *  in which we want to only print some characters of a string, or print from
 *  a buffer that is not necessarily null terminated.
 *
 * \param[in] buff Buffer storing data to write out via virtual serial.
 * \param len Numbers of chars in buff.
 * 
 * \return None.
 */
void usb_putb(const char* buff, uint32_t len) {
	for (int idx = 0; idx < len; idx++){
		usb_putc(buff[idx]);
	}
}

/**
 * Make sure any character currently in the transmit FIFO are transmitted via
 *  USB.
 *
 * \return 0 on success. Function will stall until flushing of data in transmit
 *  FIFO is initiated.
 */
int usb_flush(void) {
	// Wait for any ongoing transmissions to finish
	while (usbUartData.txBusy);

	// Start a new transmission that will make sure all data currently in
	//  txFifo is sent
	usbUartTxStart(&usbUartData);

	return 0;
}

/**
 * Called by bottom level of printf routine within RedLib C library to print
 *  characters. 
 * 
 * \parma iFileHandle Ignored.
 * \param[in] pcBuffer Stores characters to be printed.
 * \parma iLength Number of characters in pcBuffer.
 * 
 * \return Number of characters queued for printing.
 */
int WRITEFUNC(int iFileHandle, char *pcBuffer, int iLength) {
	for (int idx = 0; idx < iLength; idx++) {
		usb_putc(pcBuffer[idx]);
		// Need to add carriage return and flush after each newline
		if (pcBuffer[idx] == '\n') {
			usb_putc('\r');
			usb_flush();
		}
	}

	return iLength;
}

/**
 * Receive data from the USB CDC UART. This is called when we already know
 *  that data is waiting for us and ReadEP needs to be called.
 *
 * \param[in] uartData Contains details on Virtual Comm to receive from.
 * 
 * \return None.
 */
static void rcvUartData(UsbUartData* uartData) {
	if (uartData->rxRdIdx <= uartData->rxWrIdx) {
		// Check if there is enough room until end of buffer to 
		//  contain max number of bytes we could receive
		if (uartData->rxWrIdx + USB_MAX_PACKET_SZ >= 
			USB_UART_TXFIFO_SZ) {

			// At this point we know rxWrIdx needs to wrap to 0:

			// If rxRdIdx is 0 this would look like FIFO is empty, 
			//  so stall until reads occur
			if (uartData->rxRdIdx == 0) {
				// Mark as stalled we so try again next getc()
				uartData->rxStalled = 1;
				return;
			}

			// If FIFO is empty we also need to move rxRdIdx so
			//  that garbage samples are not read
			if (uartData->rxRdIdx == uartData->rxWrIdx) {
				uartData->rxRdIdx = 0;
			}
			uartData->rxWrapIdx = uartData->rxWrIdx;
			uartData->rxWrIdx = 0;
		}
	} 

	if (uartData->rxRdIdx > uartData->rxWrIdx) {
		// Check if this write could overflow read pointer
		if (uartData->rxWrIdx + USB_MAX_PACKET_SZ >= 
			uartData->rxRdIdx) {

			// Mark as stalled we so try again next getc()
			uartData->rxStalled = 1;
			return;
		}
	}

	uint32_t bytes_rcvd = USBD_API->hw->ReadEP(usbHandle, 
		USB_CDC_OUT_EP, 
		&uartData->rxFifo[uartData->rxWrIdx]);

	uartData->rxWrIdx += bytes_rcvd;

	if (uartData->rxWrIdx >= uartData->rxWrapIdx) {
		// Update wrap index if necessary
		uartData->rxWrapIdx = uartData->rxWrIdx + 1;
	}
}

/**
 * Check if there is a character in the USB CDC UART RX FIFO.
 * 
 * \return 0 if no character is available.
 */
int usb_tstc(void) {
	return usbUartData.rxRdIdx != usbUartData.rxWrIdx;
}

/**
 * Get a character from the USB CDC UART RX FIFO. This will not return until
 *  a character is received via USB.
 * 
 * \return The next character in the USB CDC UART RX FIFO. Will not return 
 *  until character is available.
 */
int usb_getc(void) {
	// Wait until there is a character
	while (!usb_tstc());

	char c = usbUartData.rxFifo[usbUartData.rxRdIdx];

	usbUartData.rxRdIdx++;

	if (usbUartData.rxRdIdx >= usbUartData.rxWrapIdx) {
		usbUartData.rxRdIdx = 0;
	}

	// If we stalled previously, try to receive that pending data and 
	//  startup receive callback
	if (usbUartData.rxStalled) {
		usbUartData.rxStalled = 0;
		rcvUartData(&usbUartData);
	}

	return c;
}

/**
 * Called by bottom level of scanf routine within RedLib C library to read
 *  a character. 
 *
 * \return The next character in the USB CDC UART RX FIFO. Will not return 
 *  until character is available.
 */
int READFUNC(void) {
	return usb_getc();
}

/**
 * USB CDC UART bulk EP_IN and EP_OUT endpoints handler 
 *
 * \param[in] usbHandle Handle to USB device stack.
 * \param[inout] data Pointer to USB UART data structure.
 * \param event Provides information on transfer event that occurred to trigger
 *	handler call.
 *
 * \return LPC_OK on success.
 */
static ErrorCode_t usbUartBulkHandler(USBD_HANDLE_T usbHandle, void* data, 
	uint32_t event) {
	UsbUartData* usb_uart_data = (UsbUartData*)data;

	switch (event) {
	// A transfer from us to the USB host that we queued has completed
	case USB_EVT_IN:
		// Update read index
		usb_uart_data->txRdIdx = (usb_uart_data->txRdIdx + 
			usb_uart_data->txSent) % USB_UART_TXFIFO_SZ;
		usb_uart_data->txSent = 0;
		usb_uart_data->txBusy = 0;

		// Attempt to start another transmission
		usbUartTxStart(usb_uart_data);
		break;

	// We received a transfer from the USB host. 
	case USB_EVT_OUT:
		rcvUartData(usb_uart_data);
		break;

	case ERR_USBD_STALL:
		setLedIntensity(0);
		break;

	default:
		break;
	}

	return LPC_OK;
}

//TODO: better comments
/* Find the address of interface descriptor for given class type. */
USB_INTERFACE_DESCRIPTOR *find_IntfDesc(const uint8_t *pDesc, uint32_t intfClass)
{
	USB_COMMON_DESCRIPTOR *pD;
	USB_INTERFACE_DESCRIPTOR *pIntfDesc = 0;
	uint32_t next_desc_adr;

	pD = (USB_COMMON_DESCRIPTOR *) pDesc;
	next_desc_adr = (uint32_t) pDesc;

	while (pD->bLength) {
		/* is it interface descriptor */
		if (pD->bDescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE) {

			pIntfDesc = (USB_INTERFACE_DESCRIPTOR *) pD;
			/* did we find the right interface descriptor */
			if (pIntfDesc->bInterfaceClass == intfClass) {
				break;
			}
		}
		pIntfDesc = 0;
		next_desc_adr = (uint32_t) pD + pD->bLength;
		pD = (USB_COMMON_DESCRIPTOR *) next_desc_adr;
	}

	return pIntfDesc;
}

//TODO: better comments
/* Set line coding call back routine */
static ErrorCode_t usbUartSetLineCode(USBD_HANDLE_T hCDC, CDC_LINE_CODING *line_coding)
{
	uint32_t config_data = 0;

	switch (line_coding->bDataBits) {
	case 5:
		config_data |= UART_LCR_WLEN5;
		break;

	case 6:
		config_data |= UART_LCR_WLEN6;
		break;

	case 7:
		config_data |= UART_LCR_WLEN7;
		break;

	case 8:
	default:
		config_data |= UART_LCR_WLEN8;
		break;
	}

	switch (line_coding->bCharFormat) {
	case 1:	/* 1.5 Stop Bits */
		/* In the UART hardware 1.5 stop bits is only supported when using 5
		 * data bits. If data bits is set to 5 and stop bits is set to 2 then
		 * 1.5 stop bits is assumed. Because of this 2 stop bits is not support
		 * when using 5 data bits.
		 */
		if (line_coding->bDataBits == 5) {
			config_data |= UART_LCR_SBS_2BIT;
		}
		else {
			return ERR_USBD_UNHANDLED;
		}
		break;

	case 2:	/* 2 Stop Bits */
		/* In the UART hardware if data bits is set to 5 and stop bits is set to 2 then
		 * 1.5 stop bits is assumed. Because of this 2 stop bits is
		 * not support when using 5 data bits.
		 */
		if (line_coding->bDataBits != 5) {
			config_data |= UART_LCR_SBS_2BIT;
		}
		else {
			return ERR_USBD_UNHANDLED;
		}
		break;

	default:
	case 0:	/* 1 Stop Bit */
		config_data |= UART_LCR_SBS_1BIT;
		break;
	}

	switch (line_coding->bParityType) {
	case 1:
		config_data |= (UART_LCR_PARITY_EN | UART_LCR_PARITY_ODD);
		break;

	case 2:
		config_data |= (UART_LCR_PARITY_EN | UART_LCR_PARITY_EVEN);
		break;

	case 3:
		config_data |= (UART_LCR_PARITY_EN | UART_LCR_PARITY_F_1);
		break;

	case 4:
		config_data |= (UART_LCR_PARITY_EN | UART_LCR_PARITY_F_0);
		break;

	default:
	case 0:
		config_data |= UART_LCR_PARITY_DIS;
		break;
	}

	// TODO: Should this still be here...?
	if (line_coding->dwDTERate < 3125000) {
		Chip_UART_SetBaud(LPC_USART, line_coding->dwDTERate);
	}
	Chip_UART_ConfigData(LPC_USART, config_data);

	return LPC_OK;
}

/**
 * Intialize EP on USB to act as virtual UART.
 *
 * \param[in] usbHandle Handle to USB device stack.
 * \param[in] usbDescs Points to various descriptors for the USB device, to 
 *	provide details for EPs on the bus.
 * \param[inout] usbParams Parameters related to the USB stack (i.e. memory
 *	to be used by EPs)
 *
 * \return LPC_OK on success.
 */
static ErrorCode_t usbUartInit(USBD_HANDLE_T usbHandle, 
	const USB_CORE_DESCS_T *usbDescs, USBD_API_INIT_PARAM_T *usbParams)
{
	USBD_CDC_INIT_PARAM_T cdc_param;
	ErrorCode_t ret = LPC_OK;
	uint32_t ep_indx;
	USB_CDC_CTRL_T *pCDC;

	/* Store USB stack handle for future use. */
	usbUartData.usbHandle = usbHandle;
	/* Initi CDC params */
	memset((void *) &cdc_param, 0, sizeof(USBD_CDC_INIT_PARAM_T));
	cdc_param.mem_base = usbParams->mem_base;
	cdc_param.mem_size = usbParams->mem_size;
	cdc_param.cif_intf_desc = (uint8_t*)find_IntfDesc(
		usbDescs->high_speed_desc, CDC_COMMUNICATION_INTERFACE_CLASS);
	cdc_param.dif_intf_desc = (uint8_t*)find_IntfDesc(
		usbDescs->high_speed_desc, CDC_DATA_INTERFACE_CLASS);
	cdc_param.SetLineCode = usbUartSetLineCode;

	/* Init CDC interface */
	ret = USBD_API->cdc->init(usbHandle, &cdc_param, &usbUartData.cdcHandle);

	if (ret == LPC_OK) {
		// Allocate buffer for transmit FIFO
		usbUartData.txFifo = (uint8_t *) cdc_param.mem_base;
		cdc_param.mem_base += USB_UART_TXFIFO_SZ;
		cdc_param.mem_size -= USB_UART_TXFIFO_SZ;

		// Allocate buffer for receive FIFO
		usbUartData.rxFifo = (uint8_t *) cdc_param.mem_base;
		cdc_param.mem_base += USB_UART_RXFIFO_SZ;
		cdc_param.mem_size -= USB_UART_RXFIFO_SZ;
		usbUartData.rxWrapIdx = USB_UART_RXFIFO_SZ;

		/* register endpoint interrupt handler */
		ep_indx = (((USB_CDC_IN_EP & 0x0F) << 1) + 1);
		ret = USBD_API->core->RegisterEpHandler(usbHandle, ep_indx, 
			usbUartBulkHandler, &usbUartData);

		if (ret == LPC_OK) {
			/* register endpoint interrupt handler */
			ep_indx = ((USB_CDC_OUT_EP & 0x0F) << 1);
			ret = USBD_API->core->RegisterEpHandler(usbHandle, 
				ep_indx, usbUartBulkHandler, &usbUartData);
			/* Set the line coding values as per UART Settings */
			pCDC = (USB_CDC_CTRL_T *) usbUartData.cdcHandle;
			pCDC->line_coding.dwDTERate = 115200;
			pCDC->line_coding.bDataBits = 8;
		}

		/* update mem_base and size variables for cascading calls. */
		usbParams->mem_base = cdc_param.mem_base;
		usbParams->mem_size = cdc_param.mem_size;
	}

	return ret;
}

/**
 * Configure USB interface. This allows for USB to communicate to act as a 
 *  virtual comm port.
 *
 * \return 0 on success.
 */
int usbConfig(void){
	USBD_API_INIT_PARAM_T usb_param;
	USB_CORE_DESCS_T usb_desc;
	ErrorCode_t errCode = ERR_FAILED;

	// This is externed in lpc_chip_11uxx_lib for some reason. 
	// Adding this so it will compile, though it does not seem to be used
	//  anywhere...
	// TODO: look into, understand and clean this up?
	g_pUsbApi = (const USBD_API_T *) LPC_ROM_API->usbdApiBase;

	/* initialize call back structures */
	memset((void *) &usb_param, 0, sizeof(USBD_API_INIT_PARAM_T));
	usb_param.usb_reg_base = LPC_USB0_BASE;
	/*	WORKAROUND for artf44835 ROM driver BUG:
	    Code clearing STALL bits in endpoint reset routine corrupts memory area
	    next to the endpoint control data. For example When EP0, EP1_IN, EP1_OUT,
	    EP2_IN are used we need to specify 3 here. But as a workaround for this
	    issue specify 4. So that extra EPs control structure acts as padding buffer
	    to avoid data corruption. Corruption of padding memory doesn’t affect the
	    stack/program behaviour.
	 */
	usb_param.max_num_ep = 3 + 1;
	usb_param.mem_base = USB_STACK_MEM_BASE;
	usb_param.mem_size = USB_STACK_MEM_SIZE;

	/* Set the USB descriptors */
	usb_desc.device_desc = (uint8_t *)&USB_DeviceDescriptor[0];
	usb_desc.string_desc = (uint8_t *)&USB_StringDescriptor[0];
	/* Note, to pass USBCV test full-speed only devices should have both
	   descriptor arrays point to same location and device_qualifier set to 0.
	 */
	usb_desc.full_speed_desc = (uint8_t *)&USB_FsConfigDescriptor[0];
	usb_desc.high_speed_desc = (uint8_t *)&USB_FsConfigDescriptor[0];
	usb_desc.device_qualifier = 0;

	/* USB Initialization */
	errCode = USBD_API->hw->Init(&usbHandle, &usb_desc, &usb_param);
	if (LPC_OK != errCode){
		return -1;
	}

	/*	WORKAROUND for artf32219 ROM driver BUG:
	    The mem_base parameter part of USB_param structure returned
	    by Init() routine is not accurate causing memory allocation issues for
	    further components.
	 */
	usb_param.mem_base = USB_STACK_MEM_BASE + (USB_STACK_MEM_SIZE 
		- usb_param.mem_size);

	/* Init UCOM - USB to UART bridge interface */
	errCode = usbUartInit(usbHandle, &usb_desc, &usb_param);
	if (errCode != LPC_OK) {
		return -1;
	}

	/* Make sure USB and UART IRQ priorities are same for this example */
	NVIC_SetPriority(USB0_IRQn, 1);
	/*  enable USB interrupts */
	NVIC_EnableIRQ(USB0_IRQn);
	/* now connect */
	USBD_API->hw->Connect(usbHandle, 1);

	return 0;
}

#elif (FIRMWARE_BEHAVIOR == SWITCH_WIRED_POWERA_FW)

/**
 * HID Report Descriptor. Binary copied from Wired Power A Controller.
 */
const uint8_t PowerAReportDescriptor[] = {
	//TODO: Translate these all to HID_* macros
	HID_UsagePage(HID_USAGE_PAGE_GENERIC), // 05 01 
	0x09, 0x05, // 09 05 
	0xa1, 0x01, // a1 01 
		0x15, 0x00, // 15 00 
		0x25, 0x01, // 25 01 
		0x35, 0x00, // 35 00 
		0x45, 0x01, // 45 01 
		0x75, 0x01, // 75 01 
		0x95, 0x0e, // 95 0e 

		0x05, 0x09, // 05 09 
		0x19, 0x01, // 19 01 
		0x29, 0x0e, // 29 0e 
		0x81, 0x02, // 81 02 
		0x95, 0x02, // 95 02 
		0x81, 0x01, // 81 01 

		0x05, 0x01, // 05 01 
		0x25, 0x07, // 25 07 
		0x46, 0x3b, 0x01, // 46 3b 01 
		0x75, 0x04, // 75 04 
		0x95, 0x01, // 95 01 
		0x65, 0x14, // 65 14 
		0x09, 0x39, // 09 39 
		0x81, 0x42, // 81 42 
		0x65, 0x00, // 65 00 
		0x95, 0x01, // 95 01 
		0x81, 0x01, // 81 01 

		0x26, 0xff, 0x00, // 26 ff 00 
		0x46, 0xff, 0x00, // 46 ff 00 
		0x09, 0x30, // 09 30 
		0x09, 0x31, // 09 31 
		0x09, 0x32, // 09 32 
		0x09, 0x35, // 09 35 

		0x75, 0x08, // 75 08 
		0x95, 0x04, // 95 04 
		0x81, 0x02, // 81 02 
		0x75, 0x08, // 75 08 
		0x95, 0x01, // 95 01 
		0x81, 0x03, // 81 03 

	HID_EndCollection, // c0
};

const uint16_t PowerAReportDescSize = sizeof(PowerAReportDescriptor);

/**
 * USB Standard Device Descriptor. Set to match Power A Wired Controller.
 */
ALIGNED(4) const uint8_t USB_DeviceDescriptor[] = {
	USB_DEVICE_DESC_SIZE, /* bLength */
	USB_DEVICE_DESCRIPTOR_TYPE, /* bDescriptorType */
	WBVAL(0x0200), /* bcdUSB : 2.00*/
	0x00, /* bDeviceClass */
	0x00, /* bDeviceSubClass */
	0x00, /* bDeviceProtocol */
	USB_MAX_PACKET0, /* bMaxPacketSize0 */
	WBVAL(0x20d6), /* idVendor */
	WBVAL(0xa711), /* idProduct */
	WBVAL(0x0200), /* bcdDevice : 2.00 */
	0x01, /* iManufacturer */
	0x02, /* iProduct */
	0x03, /* iSerialNumber */
	0x01 /* bNumConfigurations */
};

/**
 * USB FSConfiguration Descriptor
 * All Descriptors (Configuration, Interface, Endpoint, Class, Vendor)
 * Matches Power A Wired Controller.
 */
ALIGNED(4) uint8_t USB_FsConfigDescriptor[] = {
	/* Configuration 1 */
	USB_CONFIGURATION_DESC_SIZE, /* bLength */
	USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType */
	WBVAL( /* wTotalLength */
		USB_CONFIGURATION_DESC_SIZE   +
		USB_INTERFACE_DESC_SIZE       +
		HID_DESC_SIZE                 +
		USB_ENDPOINT_DESC_SIZE        +
		USB_ENDPOINT_DESC_SIZE
		),
	0x01, /* bNumInterfaces */
	0x01, /* bConfigurationValue */
	0x00, /* iConfiguration */
	USB_CONFIG_BUS_POWERED | USB_CONFIG_REMOTE_WAKEUP, /* bmAttributes */
	USB_CONFIG_POWER_MA(500), /* bMaxPower */

	/* Interface 0, Alternate Setting 0, HID Class */
	USB_INTERFACE_DESC_SIZE, /* bLength */
	USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
	0x00, /* bInterfaceNumber */
	0x00, /* bAlternateSetting */
	0x02, /* bNumEndpoints */
	USB_DEVICE_CLASS_HUMAN_INTERFACE, /* bInterfaceClass */
	HID_SUBCLASS_NONE, /* bInterfaceSubClass */
	HID_PROTOCOL_NONE, /* bInterfaceProtocol */
	0x00, /* iInterface */
	/* HID Class Descriptor */
	/* HID_DESC_OFFSET = 0x0012 */
	HID_DESC_SIZE, /* bLength */
	HID_HID_DESCRIPTOR_TYPE, /* bDescriptorType */
	WBVAL(0x0111), /* bcdHID : 1.11*/
	0x00, /* bCountryCode */
	0x01, /* bNumDescriptors */
	HID_REPORT_DESCRIPTOR_TYPE, /* bDescriptorType */
	WBVAL(sizeof(PowerAReportDescriptor)), /* wDescriptorLength */

	/* Endpoint, HID Interrupt In */
	USB_ENDPOINT_DESC_SIZE, /* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType */
	HID_EP_OUT, /* bEndpointAddress */
	USB_ENDPOINT_TYPE_INTERRUPT, /* bmAttributes */
	WBVAL(0x0040), /* wMaxPacketSize */
	8, /* bInterval */

	/* Endpoint, HID Interrupt Out */
	USB_ENDPOINT_DESC_SIZE, /* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType */
	HID_EP_IN, /* bEndpointAddress */
	USB_ENDPOINT_TYPE_INTERRUPT, /* bmAttributes */
	WBVAL(0x0040), /* wMaxPacketSize */
	8, /* bInterval */
	/* Terminator */
	0 /* bLength */
};

/**
 * USB String Descriptor (optional)
 * Matches Power A Wired Controller.
 */
const uint8_t USB_StringDescriptor[] = {
	/* Index 0x00: LANGID Codes */
	0x04, /* bLength */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
	WBVAL(0x0409), /* wLANGID : US English */
	/* Index 0x01: Manufacturer */
	(40 * 2 + 2), /* bLength (40 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
	'B', 0,
	'e', 0,
	'n', 0,
	's', 0,
	'u', 0,
	's', 0,
	's', 0,
	'e', 0,
	'n', 0,
	' ', 0,
	'D', 0,
	'e', 0,
	'u', 0,
	't', 0,
	's', 0,
	'c', 0,
	'h', 0,
	' ', 0,
	'&', 0,
	' ', 0,
	'A', 0,
	's', 0,
	's', 0,
	'o', 0,
	'c', 0,
	'i', 0,
	'a', 0,
	't', 0,
	'e', 0,
	's', 0,
	',', 0,
	'I', 0,
	'n', 0,
	'c', 0,
	'.', 0,
	'(', 0,
	'B', 0,
	'D', 0,
	'A', 0,
	')', 0,
	/* Index 0x02: Product */
	(28 * 2 + 2), /* bLength (28 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
	'C', 0,
	'o', 0,
	'r', 0,
	'e', 0,
	' ', 0,
	'(', 0,
	'P', 0,
	'l', 0,
	'u', 0,
	's', 0,
	')', 0,
	' ', 0,
	'W', 0,
	'i', 0,
	'r', 0,
	'e', 0,
	'd', 0,
	' ', 0,
	'C', 0,
	'o', 0,
	'n', 0,
	't', 0,
	'r', 0,
	'o', 0,
	'l', 0,
	'l', 0,
	'e', 0,
	'r', 0,
	/* Index 0x03: Serial Number */
	(12 * 2 + 2), /* bLength (12 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
	'0', 0,
	'0', 0,
	'0', 0,
	'0', 0,
	'0', 0,
	'0', 0,
	'0', 0,
	'0', 0,
	'0', 0,
	'0', 0,
	'0', 0,
	'1', 0,
	/* Index 0x04: Interface 0, Alternate Setting 0 */
	(3 * 2 + 2), /* bLength (9 Char + Type + length) */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */ //TODO: this correct?
	'H', 0,
	'I', 0,
	'D', 0,
};

// Defines how Direction Pad inputs are encoded in Power A Status Report Packet
typedef enum {
	DPAD_UP = 0x0,
	DPAD_UP_RIGHT = 0x1,
	DPAD_RIGHT = 0x2,
	DPAD_DOWN_RIGHT = 0x3,
	DPAD_DOWN = 0x4,
	DPAD_DOWN_LEFT = 0x5,
	DPAD_LEFT = 0x6,
	DPAD_UP_LEFT = 0x7,
	DPAD_NEUTRAL = 0xF,
} DpadEncoding;

// Breakdown of Status Report Packet which is sent over USB to communicate
//  state of controller inputs to Nintendo Switch
typedef struct {
	uint8_t yButton : 1;
	uint8_t bButton : 1;
	uint8_t aButton : 1;
	uint8_t xButton : 1;

	uint8_t leftBumper : 1 ;
	uint8_t rightBumper : 1;
	uint8_t leftTrigger : 1;
	uint8_t rightTrigger : 1;

	uint8_t minusButton : 1;
	uint8_t plusButton : 1;
	uint8_t leftAnalogClick : 1;
	uint8_t rightAnalogClick : 1;

	uint8_t homeButton : 1;
	uint8_t snapshotButton : 1;
	uint8_t rsvd0 : 1;
	uint8_t rsvd1 : 1;

	DpadEncoding dPad : 4;	

	uint8_t rsvd2 : 4;
	
	uint8_t leftAnalogX; // Left = 0x00, Neutral = 0x80, Right = 0xff
	uint8_t leftAnalogY; // Up = 0x00, Neutral = 0x80, Down = 0xff

	uint8_t rightAnalogX; // Left = 0x00, Neutral = 0x80, Right = 0xff
	uint8_t rightAnalogY; // Up = 0x00, Neutral = 0x80, Down = 0xff

	uint8_t rsvd3;
} ControllreStatusReport;

// Combines data related to how we communicate controller state to Nin Switch
typedef struct {
	USBD_HANDLE_T hUsb; // Handle to USB stack. 
	ControllreStatusReport statusReport; // Last report data. Defines
		// states of inputs on controller.
	volatile uint8_t txBusy; // Flag indicating whether a report is pending
		// in endpoint queue.
} ControllerUsbData;

static ControllerUsbData controllerUsbData;

/**
 * Function for converting raw analog X or Y value to analog X or Y value in
 *   range expected by Power A USB control packet. 
 *
 * \param rawVal Raw ADC value representing an X or Y position.
 * \param centerWidth Number of ADC counts that define an area for 
 *   which the control stick is considered to be in the neutral position. This 
 *   is required as the raw ADC results returned from the analog stick being in
 *   a neutral position vary from controller to controller and even from usage
 *   to usage (i.e. moving and releasing the control stick).
 * \param activeWidth Number ADC counts that define the maximum area the 
 *   control stick is expected to travel. Anything beyond this will be railed
 *   high or low, depending on which extreme is closer.
 * \param maxRawVal Max ADC count that could be returned for rawVal.
 * 
 * \return Value ready for Status Report Analog fields.
 */
static uint8_t convToPowerAJoyPos(uint32_t rawVal, uint32_t centerWidth, 
	uint32_t activeWidth, uint32_t maxRawVal) {

	const uint32_t POS_MAX = 0xFF;
	const uint32_t MID_VAL = (POS_MAX+1)/2;

	// Defines where neutral deadzone starts and ends
	const uint32_t NEUTRAL_HI_LIM = maxRawVal/2 + centerWidth/2;
	const uint32_t NEUTRAL_LO_LIM = maxRawVal/2 - centerWidth/2;

	// Defines where the saturation points are for the extremes
	const uint32_t MAX_HI_LIM = maxRawVal/2 + activeWidth/2;
	const uint32_t MAX_LO_LIM = maxRawVal/2 - activeWidth/2;

	// Used to properly convert ADC counts in active range to POWER A units
	const uint32_t TRAVEL_DIST = (activeWidth - centerWidth)/2;

	if (rawVal > MAX_HI_LIM) {
		return POS_MAX;
	}

	if (rawVal < MAX_LO_LIM) {
		return 0;
	}

	if (rawVal >= NEUTRAL_HI_LIM) { 
		return MID_VAL + (MID_VAL * (rawVal - NEUTRAL_HI_LIM))/TRAVEL_DIST;
	} else if (rawVal <= NEUTRAL_LO_LIM) {
		return (MID_VAL * (rawVal - MAX_LO_LIM))/TRAVEL_DIST;
	}

	return MID_VAL;
}

/**
 * Update HID Report(s) for Faux Wired Controller Plus (by PowerA) for Nintendo
 *  Switch. These report(s) give status information on the controller (i.e. 
 *  what buttons are being pressed, what position is the analog stick in).
 *
 * \return None.
 */
static void updateReports(void) {
	// Start long conversions run via IRQs
	updateAdcVals();
	trackpadLocUpdate(L_TRACKPAD);
	trackpadLocUpdate(R_TRACKPAD);

	// Associate Steam Controller buttons to Switch Controller buttons:
	controllerUsbData.statusReport.rightTrigger = getRightTriggerState();
	controllerUsbData.statusReport.leftTrigger = getLeftTriggerState();
	controllerUsbData.statusReport.rightBumper = getRightBumperState();
	controllerUsbData.statusReport.leftBumper = getLeftBumperState();

	controllerUsbData.statusReport.xButton = getYButtonState();
	controllerUsbData.statusReport.aButton = getBButtonState();
	controllerUsbData.statusReport.bButton = getAButtonState();
	controllerUsbData.statusReport.yButton = getXButtonState();

	controllerUsbData.statusReport.snapshotButton = getLeftGripState();
	controllerUsbData.statusReport.homeButton = getSteamButtonState();

	controllerUsbData.statusReport.rightAnalogClick = getRightTrackpadClickState();
	controllerUsbData.statusReport.leftAnalogClick = getJoyClickState();
	controllerUsbData.statusReport.plusButton = getFrontRightButtonState();
	controllerUsbData.statusReport.minusButton = getFrontLeftButtonState();

	// Analog Joystick is Left Analog:
	controllerUsbData.statusReport.leftAnalogX = convToPowerAJoyPos(
		JOYSTICK_MAX_X-getAdcVal(ADC_JOYSTICK_X), 128, JOYSTICK_MAX_X/2,
		JOYSTICK_MAX_X);
	controllerUsbData.statusReport.leftAnalogY = convToPowerAJoyPos(
		JOYSTICK_MAX_Y-getAdcVal(ADC_JOYSTICK_Y), 128, JOYSTICK_MAX_Y/2,
		JOYSTICK_MAX_Y);

	uint16_t tpad_x = 0;
	uint16_t tpad_y = 0;

	// Default to neutral position
	controllerUsbData.statusReport.dPad = DPAD_NEUTRAL;

	// Have Left Trackpad act as DPAD:
	// Only check (and convert) finger position to DPAD location on click
	if (getLeftTrackpadClickState()) {

		trackpadGetLastXY(L_TRACKPAD, &tpad_x, &tpad_y);

		if (tpad_x > TPAD_MAX_X * 3/8 && tpad_x < TPAD_MAX_X * 5/8) {
			if (tpad_y > TPAD_MAX_Y * 3/8 && tpad_y < TPAD_MAX_Y * 5/8) {
				controllerUsbData.statusReport.dPad = DPAD_NEUTRAL;
			} else if (tpad_y <= TPAD_MAX_Y * 3/8) {
				controllerUsbData.statusReport.dPad = DPAD_DOWN;
			} else {
				controllerUsbData.statusReport.dPad = DPAD_UP;
			}
		} else if (tpad_x <= TPAD_MAX_X * 3/8) {
			// Put more emphasis into cardinal directions
			if (tpad_y > TPAD_MAX_Y * 2/8 && tpad_y < TPAD_MAX_Y * 6/8) {
				controllerUsbData.statusReport.dPad = DPAD_LEFT;
			} else if (tpad_y <= TPAD_MAX_Y * 2/8) {
				controllerUsbData.statusReport.dPad = DPAD_DOWN_LEFT;
			} else {
				controllerUsbData.statusReport.dPad = DPAD_UP_LEFT;
			}
		} else {
			// Put more emphasis into cardinal directions
			if (tpad_y > TPAD_MAX_Y * 2/8 && tpad_y < TPAD_MAX_Y * 6/8) {
				controllerUsbData.statusReport.dPad = DPAD_RIGHT;
			} else if (tpad_y <= TPAD_MAX_Y * 2/8) {
				controllerUsbData.statusReport.dPad = DPAD_DOWN_RIGHT;
			} else {
				controllerUsbData.statusReport.dPad = DPAD_UP_RIGHT;
			}
		}
	}

	// Have Right Trackpad act as Right Analog:
	trackpadGetLastXY(R_TRACKPAD, &tpad_x, &tpad_y);
	controllerUsbData.statusReport.rightAnalogX = convToPowerAJoyPos(tpad_x, 
		0, TPAD_MAX_X/2, TPAD_MAX_X);
	controllerUsbData.statusReport.rightAnalogY = convToPowerAJoyPos(
		 TPAD_MAX_Y - tpad_y, 0, TPAD_MAX_Y/2, TPAD_MAX_Y);
}

/**
 * HID Get Report Request Callback. Called automatically on HID Get Report Request 
 */
static ErrorCode_t ControllerGetReport(USBD_HANDLE_T hHid, 
	USB_SETUP_PACKET* pSetup, uint8_t* * pBuffer, uint16_t* plength) {

	// ReportID = SetupPacket.wValue.WB.L
	switch (pSetup->wValue.WB.H) {
	case HID_REPORT_INPUT:
		updateReports();
		*pBuffer = (uint8_t*)&controllerUsbData.statusReport;
		*plength = sizeof(ControllreStatusReport);
		break;

	case HID_REPORT_OUTPUT:	/* Not Supported */
	case HID_REPORT_FEATURE: /* Not Supported */
		return ERR_USBD_STALL;
	}

	return LPC_OK;
}

/** 
 * HID Set Report Request Callback. Called automatically on HID Set Report Request 
 */
static ErrorCode_t ControllerSetReport(USBD_HANDLE_T hHid, 
	USB_SETUP_PACKET *pSetup, uint8_t * *pBuffer, uint16_t length) {

	// we will reuse standard EP0Buf
	if (length == 0) {
		return LPC_OK;
	}

	// ReportID = SetupPacket.wValue.WB.L;
	switch (pSetup->wValue.WB.H) {
	case HID_REPORT_INPUT: /* Not Supported */
	case HID_REPORT_OUTPUT: /* Not Supported */
	case HID_REPORT_FEATURE: /* Not Supported */
		return ERR_USBD_STALL;
	}

	return LPC_OK;
}

/**
 * HID interrupt IN endpoint handler 
 */
static ErrorCode_t ControllerEpInHandler(USBD_HANDLE_T hUsb, void *data, 
	uint32_t event) {

	switch (event) {
	case USB_EVT_IN:
		// USB_EVT_IN occurs when HW completes sending IN packet. So 
		//  clear the busy flag for main loop to queue next packet.
		controllerUsbData.txBusy = 0;
		break;
	}

	return LPC_OK;
}

/**
 * HID Controller interface init routine 
 */
static ErrorCode_t ControllerInit(USBD_HANDLE_T hUsb,
			   USB_INTERFACE_DESCRIPTOR *pIntfDesc,
			   uint32_t *mem_base,
			   uint32_t *mem_size) {

	USBD_HID_INIT_PARAM_T hid_param;
	USB_HID_REPORT_T reports_data[1];
	ErrorCode_t ret = LPC_OK;

	// Do a quick check of if the interface descriptor passed is the right one.
	if ((pIntfDesc == 0) || (pIntfDesc->bInterfaceClass != USB_DEVICE_CLASS_HUMAN_INTERFACE)) {
		return ERR_FAILED;
	}

	// Init HID params
	memset((void *) &hid_param, 0, sizeof(USBD_HID_INIT_PARAM_T));
	hid_param.max_reports = 1;
	hid_param.mem_base = *mem_base;
	hid_param.mem_size = *mem_size;
	hid_param.intf_desc = (uint8_t *) pIntfDesc;

	// user defined functions
	hid_param.HID_GetReport = ControllerGetReport;
	hid_param.HID_SetReport = ControllerSetReport;
	hid_param.HID_EpIn_Hdlr = ControllerEpInHandler;

	// Init reports_data
	reports_data[0].len = PowerAReportDescSize;
	reports_data[0].idle_time = 0;
	reports_data[0].desc = (uint8_t *) &PowerAReportDescriptor[0];
	hid_param.report_data  = reports_data;

	ret = USBD_API->hid->init(hUsb, &hid_param);

	// update memory variables
	*mem_base = hid_param.mem_base;
	*mem_size = hid_param.mem_size;

	// store stack handle for later use.
	controllerUsbData.hUsb = hUsb;

	return ret;
}

/**
 * Configure USB interface. This allows for the Steam Controller to appear as
 *  a Wired Controller Plus (by PowerA) for the Switch. 
 *
 * \return 0 on success.
 */
int usbConfig(void){
	USBD_API_INIT_PARAM_T usb_param;
	USB_CORE_DESCS_T desc;
	ErrorCode_t errCode = ERR_FAILED;

	/* initialize USBD ROM API pointer. */
	g_pUsbApi = (const USBD_API_T *) LPC_ROM_API->usbdApiBase;

	/* initialize call back structures */
	memset((void *) &usb_param, 0, sizeof(USBD_API_INIT_PARAM_T));
	usb_param.usb_reg_base = LPC_USB0_BASE;
	/*	WORKAROUND for artf44835 ROM driver BUG:
	    Code clearing STALL bits in endpoint reset routine corrupts memory area
	    next to the endpoint control data. For example When EP0, EP1_IN, EP1_OUT,
	    EP2_IN are used we need to specify 3 here. But as a workaround for this
	    issue specify 4. So that extra EPs control structure acts as padding buffer
	    to avoid data corruption. Corruption of padding memory doesn’t affect the
	    stack/program behaviour.
	 */
	usb_param.max_num_ep = 2 + 1;
	usb_param.mem_base = USB_STACK_MEM_BASE;
	usb_param.mem_size = USB_STACK_MEM_SIZE;

	/* Set the USB descriptors */
	desc.device_desc = (uint8_t *) USB_DeviceDescriptor;
	desc.string_desc = (uint8_t *) USB_StringDescriptor;

	/* Note, to pass USBCV test full-speed only devices should have both
	 * descriptor arrays point to same location and device_qualifier set
	 * to 0.
	 */
	desc.high_speed_desc = USB_FsConfigDescriptor;
	desc.full_speed_desc = USB_FsConfigDescriptor;
	desc.device_qualifier = 0;

	/* USB Initialization */
	errCode = USBD_API->hw->Init(&usbHandle, &desc, &usb_param);
	if (LPC_OK != errCode){
		return -1;
	}

	/*	WORKAROUND for artf32219 ROM driver BUG:
	    The mem_base parameter part of USB_param structure returned
	    by Init() routine is not accurate causing memory allocation issues for
	    further components.
	 */
	usb_param.mem_base = USB_STACK_MEM_BASE + (USB_STACK_MEM_SIZE 
		- usb_param.mem_size);

	errCode = ControllerInit(usbHandle, (USB_INTERFACE_DESCRIPTOR *) &USB_FsConfigDescriptor[sizeof(USB_CONFIGURATION_DESCRIPTOR)],
		&usb_param.mem_base, &usb_param.mem_size);
	if (errCode != LPC_OK) {
		return -1;
	}

	/*  enable USB interrupts */
	NVIC_EnableIRQ(USB0_IRQn);
	/* now connect */
	USBD_API->hw->Connect(usbHandle, 1);

	return 0;
}

/**
 * If applicable get all the latest state information for the controller and
 *  send an updated status packet to the Switch via USB.
 * 
 * \return None.
 */
void updateControllerStatusPacket(void) {
	// check device is configured before sending report.
	if (USB_IsConfigured(controllerUsbData.hUsb)) {
		if (controllerUsbData.txBusy == 0) {
			// Update report based on board state
			updateReports();

			// Send report data
			controllerUsbData.txBusy = 1;
			USBD_API->hw->WriteEP(controllerUsbData.hUsb, HID_EP_IN, 
				(uint8_t*)&controllerUsbData.statusReport, 
				sizeof(ControllreStatusReport));
		}
	} else {
		// Reset busy flag if we get disconnected
		controllerUsbData.txBusy = 0;
	}
}

/**
 * Not used in this build configuration.
 */
int usb_flush(void) {
	return 0;
}

/**
 * Not used in this build configuration.
 */
int usb_putc(int character) {
	return 0;
}

/**
 * Not used in this build configuration.
 */
void usb_putb(const char* buff, uint32_t len) {
}

/**
 * Not used in this build configuration.
 */
int usb_tstc(void) {
	return 0;
}

/**
 * Not used in this build configuration.
 */
int usb_getc(void) {
	return 0;
}
#endif
