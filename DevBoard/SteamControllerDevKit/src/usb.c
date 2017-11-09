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
#include "eeprom_access.h"

//TODO: straighten out weird circular includes? We cannot include usbd/usbd_core.h, even though that's what we want at this point...
//#include "usbd/usbd_core.h"
#include "app_usbd_cfg.h"

#include "chip.h"

#include <string.h>

const USBD_API_T *g_pUsbApi; //!< Through a series of non-ideal associations
	//!< this is used to access boot ROM code for USB functions. See
	//!< calls using USBD_API to access this. 

static USBD_HANDLE_T usbHandle; //!< Handle for interacting with the USB device

// Structure containing Virtual Comm port control data.
typedef struct USB_UART_DATA {
	USBD_HANDLE_T usbHandle; //!< Handle to USB device stack 
	USBD_HANDLE_T cdcHandle; //!< Handle to Communications Device Class controller

	uint8_t *rxBuf;	//!< USB CDC UART buffer to store received uart data
	uint8_t rxRcvd; //!< Number of valid bytes in rxBuf received by IRQ 
//TODO: remove?
	uint8_t rxChecked; /*!< Number of bytes in rx buffer that have been checked by main thread */
	volatile uint8_t rxBusy; //!< EP event handler is busy receiving data
	volatile uint8_t rxRdBusy; //!< rxBuf is currently being drained by
		//!< higher level code.

	uint8_t *txBuf;	//!< USB CDC UART buffer where data to be transfered is stored
	uint8_t txLen; //!< Number of bytes to send from txBuf in total
	uint8_t txSent; //!< Number of bytes already sent from txBuf
	volatile uint8_t txBusy; /*!< USB is busy sending previous packet */

} USB_UART_DATA_T;

// Virtual Comm port control data instance. 
static USB_UART_DATA_T usbUartData;

// Number of bytes for buffer tx and rx for USB CDC UART
static const uint32_t USB_UART_BUFF_SIZE = 64;

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
	WBVAL(64), /* wMaxPacketSize */
	0x00, /* bInterval: ignore for Bulk transfer */
	/* Terminator */
	0 /* bLength */
};

//TODO: better descriptor text? Maybe baud rate here?
/**
 * USB String Descriptor (optional)
 */
ALIGNED(4) const uint8_t USB_StringDescriptor[] = {
	/* Index 0x00: LANGID Codes */
	0x04, /* bLength */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
	WBVAL(0x0409),	/* US English */    /* wLANGID */
	/* Index 0x01: Manufacturer */
	(3 * 2 + 2), /* bLength (13 Char + Type + lenght) */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
	'G', 0,
	'G', 0,
	' ', 0,
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
	(6 * 2 + 2), /* bLength (8 Char + Type + lenght) */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
	'G', 0,
	'G', 0,
	' ', 0,
	'-', 0,
	'7', 0,
	'7', 0,
	/* Index 0x04: Interface 1, Alternate Setting 0 */
	( 4 * 2 + 2), /* bLength (4 Char + Type + lenght) */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
	'V', 0,
	'C', 0,
	'O', 0,
	'M', 0,
};

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
static ErrorCode_t usbUartBulkHandler(USBD_HANDLE_T usbHandle, void *data, uint32_t event)
{
	USB_UART_DATA_T *pUcom = (USB_UART_DATA_T *) data;
	uint32_t count = 0;

	switch (event) {
	/* A transfer from us to the USB host that we queued has completed. */
	case USB_EVT_IN:
		// Calculate how much data needs to be sent
		count = pUcom->txLen - pUcom->txSent;
		if (count > 0)
		{
			// Request to send more data
			pUcom->txSent += USBD_API->hw->WriteEP(usbHandle, USB_CDC_IN_EP, &pUcom->txBuf[pUcom->txSent], count);
		}
		else
		{
			// Transmission request complete
			pUcom->txSent = 0;
			pUcom->txLen = 0;
			pUcom->txBusy = 0;
		}
		break;

	/* We received a transfer from the USB host. */
	case USB_EVT_OUT:
		// Check if there is room for more data in buffer
		if (pUcom->rxRcvd < USB_UART_BUFF_SIZE && !pUcom->rxRdBusy)
		{
			// Mark we are busy receiving data
			pUcom->rxBusy = 1;

			// Add data to the receive buffer
			count = USBD_API->hw->ReadEP(usbHandle, USB_CDC_OUT_EP, &pUcom->rxBuf[pUcom->rxRcvd]);
			// Echo back input
			//TODO: we are not checking return value, and if input buffer is large, we may not echo back everything
			USBD_API->hw->WriteEP(usbHandle, USB_CDC_IN_EP, &pUcom->rxBuf[pUcom->rxRcvd], count);
			// Update how much data is in the receive buffer so main thread can check it
			pUcom->rxRcvd += count;

			// Mark we are no longer busy receiving data
			pUcom->rxBusy = 0;
		}

		break;

	default:
		break;
	}

	return LPC_OK;
}

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

	if (line_coding->dwDTERate < 3125000) {
		Chip_UART_SetBaud(LPC_USART, line_coding->dwDTERate);
	}
	Chip_UART_ConfigData(LPC_USART, config_data);

	return LPC_OK;
}


/**
 * Process a read command.
 *
 * \param[in] params Portion of null terminated command string containing parameters.
 * \param len Number of bytes in params
 *
 * \return None.
 */
void readCmd(const uint8_t* params, uint32_t len)
{
	uint8_t resp_msg[256];
	int resp_msg_len = 0;

	int word_size = 0;
	uint32_t addr = 0;
	int num_words = 0;	

	int bytes_per_word = 0;

	int num_params_rd = 0;

	num_params_rd = sscanf(params, "%d %x %d", &word_size, &addr, &num_words);

	if (num_params_rd != 3)
	{
		resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
			"\r\nUsage: r word_size hex_base_addr num_words\r\n");
		sendUsbSerialData(resp_msg, resp_msg_len);
		return;
	}

	if (word_size != 8 && word_size != 16 && word_size != 32)
	{
		resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
			"\r\nUnsuported word size %d\r\n", word_size);
		sendUsbSerialData(resp_msg, resp_msg_len);
		return;
	}

	bytes_per_word = word_size / 8;

	resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
		"\r\nReading %d %d-bit words starting at 0x%X\r\n", num_words, word_size, addr);
	sendUsbSerialData(resp_msg, resp_msg_len);

	for (int word_cnt = 0; word_cnt < num_words; word_cnt++)
	{
		if (!(word_cnt % 8))
		{
			resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
				"\r\n%08X: ", addr);
			sendUsbSerialData(resp_msg, resp_msg_len);
		}

		if (word_size == 8)
		{
			resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
				"%02X ", *(uint8_t*)addr);
		}
		else if (word_size == 16)
		{
			resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
				"%04X ", *(uint16_t*)addr);
		}
		else if (word_size == 32)
		{
			resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
				"%08X ", *(uint32_t*)addr);
		}
		sendUsbSerialData(resp_msg, resp_msg_len);

		addr += bytes_per_word;
	}

	resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), "\r\n");
	sendUsbSerialData(resp_msg, resp_msg_len);
}

void eepromCmd(const uint8_t* params, uint32_t len)
{
	uint8_t resp_msg[256];
	int resp_msg_len = 0;

	int word_size = 0;
	uint32_t offset = 0;
	int num_words = 0;	

	int bytes_per_word = 0;

	int num_params_rd = 0;

	num_params_rd = sscanf(params, "%d %x %d", &word_size, &offset, &num_words);

	if (num_params_rd != 3)
	{
		resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
			"\r\nUsage: e word_size hex_offset num_words\r\n");
		sendUsbSerialData(resp_msg, resp_msg_len);
		return;
	}

	if (word_size != 8 && word_size != 16 && word_size != 32)
	{
		resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
			"\r\nUnsuported word size %d\r\n", word_size);
		sendUsbSerialData(resp_msg, resp_msg_len);
		return;
	}

	bytes_per_word = word_size / 8;
	uint32_t num_read_bytes = bytes_per_word * num_words;

	resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
		"\r\nReading %d %d-bit words starting at 0x%X in EEPROM\r\n", num_words, word_size, offset);
	sendUsbSerialData(resp_msg, resp_msg_len);


	void* read_data = malloc(num_read_bytes);
	if (!read_data){
		resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
			"\r\nmalloc failed for read_data buffer\r\n");
		sendUsbSerialData(resp_msg, resp_msg_len);
		return;
	}

	int err_code = eeprom_read(offset, read_data, num_read_bytes);
	if (CMD_SUCCESS != err_code){
		resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
			"\r\nEEPROM Read failed with error code %d\r\n", err_code);
		sendUsbSerialData(resp_msg, resp_msg_len);
		free(read_data);
		return;
	}

	for (int word_cnt = 0; word_cnt < num_words; word_cnt++)
	{
		if (!(word_cnt % 8))
		{
			resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
				"\r\n%08X: ", offset);
			sendUsbSerialData(resp_msg, resp_msg_len);
		}

		if (word_size == 8)
		{
			resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
				"%02X ", *(uint8_t*)(read_data+offset));
		}
		else if (word_size == 16)
		{
			resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
				"%04X ", *(uint16_t*)(read_data+offset));
		}
		else if (word_size == 32)
		{
			resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
				"%08X ", *(uint32_t*)(read_data+offset));
		}
		sendUsbSerialData(resp_msg, resp_msg_len);

		offset += bytes_per_word;
	}

	resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), "\r\n");
	sendUsbSerialData(resp_msg, resp_msg_len);

	free(read_data);
}

/**
 * Process input command.
 *
 * \param[in] buffer Contains null terminated command string.
 * \param len Number of valid bytes in command buffer.
 *
 * \return None.
 */
void processCmd(const uint8_t* buffer, uint32_t len)
{
	uint8_t resp_msg[256];
	int resp_msg_len = 0;
	uint8_t cmd = 0;

	if (!len)
	{
		resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
			"\r\nInvalid Command Length of 0\r\n");
		sendUsbSerialData(resp_msg, resp_msg_len);
		return;
	}

	cmd = buffer[0];

	switch (cmd)
	{
	case 'r':
		readCmd(&buffer[1], len-1);
		sendUsbSerialData(resp_msg, resp_msg_len);
		break;

	case 'w':
		resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
			"\r\nWrite Command not supported yet\r\n");
		sendUsbSerialData(resp_msg, resp_msg_len);
		break;

	case 'e':
		eepromCmd(&buffer[1], len-1);
		sendUsbSerialData(resp_msg, resp_msg_len);
		break;

	default:
		resp_msg_len = snprintf((char*)resp_msg, sizeof(resp_msg), 
			"\r\nUnrecognized command %c\r\n", cmd);
		sendUsbSerialData(resp_msg, resp_msg_len);
		break;
	}
}

/* UART to USB com port init routine */
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
	cdc_param.cif_intf_desc = (uint8_t *) find_IntfDesc(usbDescs->high_speed_desc, CDC_COMMUNICATION_INTERFACE_CLASS);
	cdc_param.dif_intf_desc = (uint8_t *) find_IntfDesc(usbDescs->high_speed_desc, CDC_DATA_INTERFACE_CLASS);
	cdc_param.SetLineCode = usbUartSetLineCode;

	/* Init CDC interface */
	ret = USBD_API->cdc->init(usbHandle, &cdc_param, &usbUartData.cdcHandle);

	if (ret == LPC_OK) {
		/* allocate transfer buffers */
		usbUartData.txBuf = (uint8_t *) cdc_param.mem_base;
		cdc_param.mem_base += USB_UART_BUFF_SIZE;
		cdc_param.mem_size -= USB_UART_BUFF_SIZE;
		usbUartData.rxBuf = (uint8_t *) cdc_param.mem_base;
		// Make rxBuf twice as big to avoid potential overflow. We are
		//  using this buffer to build commands, as a USB packet of 
		//  USB_UART_BUFF_SIZE could come in to a partially filled buffer
		cdc_param.mem_base += 2*USB_UART_BUFF_SIZE;
		cdc_param.mem_size -= 2*USB_UART_BUFF_SIZE;

		/* register endpoint interrupt handler */
		ep_indx = (((USB_CDC_IN_EP & 0x0F) << 1) + 1);
		ret = USBD_API->core->RegisterEpHandler(usbHandle, ep_indx, usbUartBulkHandler, &usbUartData);

		if (ret == LPC_OK) {
			/* register endpoint interrupt handler */
			ep_indx = ((USB_CDC_OUT_EP & 0x0F) << 1);
			ret = USBD_API->core->RegisterEpHandler(usbHandle, ep_indx, usbUartBulkHandler, &usbUartData);
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
//TODO: end: clean up and fix style

/**
 * Configure USB interface. This allows for USB to communicate to other devices
 *  on the bus (i.e. act as a virtual comm port, or to haptics (TODO)).
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
	usb_param.mem_base = USB_STACK_MEM_BASE + (USB_STACK_MEM_SIZE - usb_param.mem_size);

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
}

/**
 * Called by main thread to handle commands received via USB (i.e. CDC UART)
 */
//TODO: should take input parameter for usb CDC device to handle input from instead of using global
void handleInput()
{
	uint8_t rxRcvd = usbUartData.rxRcvd;

	// Don't want to be here if IRQ might be changing value underneath us
	if (usbUartData.rxBusy)
		return;

	// See if there is anything new in the rxBuf to check
	if (usbUartData.rxChecked >= rxRcvd)
		return;

	// Mark we are busy checking the receive buffer
	usbUartData.rxRdBusy = 1;

	// Check for new characters in buffer
	for (uint8_t offset = usbUartData.rxChecked; offset < rxRcvd; offset++)
	{
		// Carriage return marks end of a command
		if (usbUartData.rxBuf[offset] == '\r')
		{
			// Null terminate for when buffer is treated as cstring
			usbUartData.rxBuf[offset] = 0;
			processCmd(usbUartData.rxBuf, offset);

			// Reset receive buffer
			usbUartData.rxRcvd = 0;
			usbUartData.rxChecked = 0;
		}
		else
		{
			usbUartData.rxChecked++;
		}
	}

	// Check if too long of a command has been input
	if (usbUartData.rxRcvd >= USB_UART_BUFF_SIZE-1)
	{
		char msg[] = "\r\nInvalid command. Too large. Clearing input buffer.\r\n";
		sendUsbSerialData((uint8_t*)msg, sizeof(msg));

		// Reset receive buffer
		usbUartData.rxRcvd = 0;
		usbUartData.rxChecked = 0;
	}

	// No longer busy with the receive buffer
	usbUartData.rxRdBusy = 0;
}

/**
 * Function for sending data out the CDC UART interface.
 *  Does not return until all the data has been sent.
 *
 * \param[in] data The data to transmit.
 * \param len The number of bytes in the data buffer.
 *
 * \return None.
 */
void sendUsbSerialData(const uint8_t* data, uint32_t len)
{
	// Transmission may need to be broken up given txBuf size
	for (uint32_t sent = 0; sent < len; sent+=USB_UART_BUFF_SIZE)
	{
		// Calculate how much to send in this request
		uint32_t to_send = len - sent;
		if (to_send > USB_UART_BUFF_SIZE)
		{
			usbUartData.txLen = USB_UART_BUFF_SIZE;
		}
		else
		{
			usbUartData.txLen = to_send;
		}

		memcpy(usbUartData.txBuf, &data[sent], usbUartData.txLen);

		usbUartData.txBusy = 1;
		usbUartData.txSent = USBD_API->hw->WriteEP(usbUartData.usbHandle, 
			USB_CDC_IN_EP, usbUartData.txBuf, usbUartData.txLen);

		// Wait for request to completely finish
		while(usbUartData.txBusy);
	}
}

