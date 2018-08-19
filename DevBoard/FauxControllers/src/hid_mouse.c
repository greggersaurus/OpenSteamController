/*
 * @brief This file contains USB HID Mouse example using USB ROM Drivers.
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
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

#include "board.h"
#include <stdint.h>
#include <string.h>
#include "usbd_rom_api.h"
#include "hid_mouse.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/**
 * @brief Structure to hold mouse data
 */
typedef struct {
	USBD_HANDLE_T hUsb;	/*!< Handle to USB stack. */
	uint8_t report[MOUSE_REPORT_SIZE];	/*!< Last report data  */
	uint8_t tx_busy;	/*!< Flag indicating whether a report is pending in endpoint queue. */
} Mouse_Ctrl_T;

/** Singleton instance of mouse control */
static Mouse_Ctrl_T g_mouse;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

extern const uint8_t ProController_ReportDescriptor[];
extern const uint16_t ProController_ReportDescSize;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

static void setXYMouseReport(uint8_t *rep, int8_t xVal, int8_t yVal)
{
	rep[1] = (uint8_t) xVal;
	rep[2] = (uint8_t) yVal;
}

static void setLeftButtonMouseReport(uint8_t *rep, uint8_t state)
{
	if (state) {
		rep[0] |= 0x01;
	}
	else {
		rep[0] &= ~0x01;
	}
}

/* Routine to update mouse state report */
static void Mouse_UpdateReport(void)
{
	uint8_t joystick_status = Joystick_GetStatus();
	CLEAR_HID_MOUSE_REPORT(&g_mouse.report[0]);

	switch (joystick_status) {
	case JOY_PRESS:
		setLeftButtonMouseReport(g_mouse.report, 1);
		break;

	case JOY_LEFT:
		setXYMouseReport(g_mouse.report, -4, 0);
		break;

	case JOY_RIGHT:
		setXYMouseReport(g_mouse.report, 4, 0);
		break;

	case JOY_UP:
		setXYMouseReport(g_mouse.report, 0, -4);
		break;

	case JOY_DOWN:
		setXYMouseReport(g_mouse.report, 0, 4);
		break;
	}
}

/* HID Get Report Request Callback. Called automatically on HID Get Report Request */
static ErrorCode_t Mouse_GetReport(USBD_HANDLE_T hHid, USB_SETUP_PACKET *pSetup, uint8_t * *pBuffer, uint16_t *plength)
{
	/* ReportID = SetupPacket.wValue.WB.L; */
	switch (pSetup->wValue.WB.H) {
	case HID_REPORT_INPUT:
		Mouse_UpdateReport();
		//*pBuffer = &g_mouse.report[0];
		*plength = 0;//MOUSE_REPORT_SIZE;
		return -32;
		break;

	case HID_REPORT_OUTPUT:				/* Not Supported */
	case HID_REPORT_FEATURE:			/* Not Supported */
		return ERR_USBD_STALL;
	}
	return LPC_OK;
}

/* HID Set Report Request Callback. Called automatically on HID Set Report Request */
static ErrorCode_t Mouse_SetReport(USBD_HANDLE_T hHid, USB_SETUP_PACKET *pSetup, uint8_t * *pBuffer, uint16_t length)
{
	/* we will reuse standard EP0Buf */
	if (length == 0) {
		return LPC_OK;
	}
	/* ReportID = SetupPacket.wValue.WB.L; */
	switch (pSetup->wValue.WB.H) {
	case HID_REPORT_INPUT:				/* Not Supported */
	case HID_REPORT_OUTPUT:				/* Not Supported */
	case HID_REPORT_FEATURE:			/* Not Supported */
		return ERR_USBD_STALL;
	}
	return LPC_OK;
}

static volatile int send_updates = 0;

/* HID interrupt IN endpoint handler */
static ErrorCode_t Mouse_EpIN_Hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	switch (event) {
	case USB_EVT_IN:
		/* USB_EVT_IN occurs when HW completes sending IN packet. So clear the
		    busy flag for main loop to queue next packet.
		 */
		g_mouse.tx_busy = 0;
		break;
	}
	return LPC_OK;
}

typedef struct {
	uint8_t data[8];
	uint32_t size;
} UsbDataPacket;

static UsbDataPacket usbDataPackets[16];
static int usbDataPacketsCnt = 0;
static int epOutHdlrCnt = 0;

static ErrorCode_t Mouse_EpOUT_Hdlr(USBD_HANDLE_T hUsb, void* data, uint32_t event)
{
	uint32_t cnt = 0;
	uint8_t rd_data[64];

	static int unhandled_cnt = 0;
	static int unhandled_ids[64];

	epOutHdlrCnt++;

	switch (event) {
		case USB_EVT_OUT:
			/* USB_EVT_IN occurs when HW completes sending IN packet. So clear the
			    busy flag for main loop to queue next packet.
			 */
			g_mouse.tx_busy = 0;



			memset(rd_data, 0, 64);

			cnt = USBD_API->hw->ReadEP(hUsb, HID_EP_OUT, rd_data);

			int cpy_cnt = cnt;
			if (cpy_cnt > 8)
				cpy_cnt = 8;
			if (usbDataPacketsCnt < 16) {
				memcpy(usbDataPackets[usbDataPacketsCnt].data, rd_data, cpy_cnt);
				usbDataPackets[usbDataPacketsCnt].size = cnt;
				usbDataPacketsCnt++;
			}

			if (cnt) {
				if (rd_data[0] == 0x00) {
					if (cnt > 1) {
						if (rd_data[1] == 0x00) {
							// Respond with status and MAC address
							rd_data[0] = 0x81;
							rd_data[1] = 0x01;
							// Connection status?
							rd_data[2] = 0x00;
							// Controller Type?
							rd_data[3] = 0x03;
							// MAC address
							rd_data[4] = 0x86;
							rd_data[5] = 0xb3;
							rd_data[6] = 0xef;
							rd_data[7] = 0xbe;
							rd_data[8] = 0xad;
							rd_data[9] = 0xde;
							// TODO: what if cnt is not 64?
							cnt = USBD_API->hw->WriteEP(hUsb, HID_EP_IN, rd_data, 64);
						}
					}
				} else if (rd_data[0] == 0x01 && cnt == 49) {
							// Response copied from one run by Pro Controller... Will call out which numbers change (though don't know why yet...)
							rd_data[0] = 0x21;
							rd_data[1] = 0x92; // Changes: Counter/timer?
							rd_data[2] = 0x91;
							rd_data[3] = 0x00;
							rd_data[4] = 0x80;
							rd_data[5] = 0x00;
							rd_data[6] = 0xb2; // Changes: bluetooth synchronization related??
							rd_data[7] = 0xc7; // Changes: bluetooth synchronization related??
							rd_data[8] = 0x73; // Changes: bluetooth synchronization related??
							rd_data[9] = 0x7c; // Changes: bluetooth synchronization related??
							rd_data[10] = 0xd7; // Changes: bluetooth synchronization related??
							rd_data[11] = 0x78; // Changes: bluetooth synchronization related??
							rd_data[12] = 0x0b; // Changes: bluetooth synchronization related??
							rd_data[13] = 0x80;
							rd_data[14] = 0x00;
							rd_data[15] = 0x03;
							// TODO: what if cnt is not 64?
							cnt = USBD_API->hw->WriteEP(hUsb, HID_EP_IN, rd_data, 64);
				} else if (rd_data[0] == 0x80) {
					if (cnt > 1) {
						if ( rd_data[1] == 0x01) {
							// Respond with status and MAC address
							rd_data[0] = 0x81;
							rd_data[1] = 0x01;
							// Connection status?
							rd_data[2] = 0x00;
							// Controller Type?
							rd_data[3] = 0x03;
							// MAC address
							rd_data[4] = 0x86;
							rd_data[5] = 0xb3;
							rd_data[6] = 0xef;
							rd_data[7] = 0xbe;
							rd_data[8] = 0xad;
							rd_data[9] = 0xde;
							// TODO: what if cnt is not 64?
							cnt = USBD_API->hw->WriteEP(hUsb, HID_EP_IN, rd_data, 64);
						} else if (rd_data[1] == 0x02) {
							// Handshake?
							rd_data[0] = 0x81;
							rd_data[1] = 0x02;
							// TODO: what if cnt is not 64?
							cnt = USBD_API->hw->WriteEP(hUsb, HID_EP_IN, rd_data, 64);
						} else if (rd_data[1] == 0x03) {
							// Switch baud rate to 3 Mbit?
							rd_data[0] = 0x81;
							rd_data[1] = 0x03;
							// TODO: what if cnt is not 64?
							//cnt = USBD_API->hw->WriteEP(hUsb, HID_EP_IN, rd_data, 64);
						} else if (rd_data[1] == 0x04) {
							// Talk HID only from now on??
							// no response? but start sending packets?
							//send_updates = 1;
						} else if (rd_data[1] == 0x05) {
							// De-init to try and talk BT again...?
							// no response?
						} else {
							unhandled_ids[unhandled_cnt] = rd_data[0] << 8 | rd_data[1];
							unhandled_cnt++;
						}
					} else {
							unhandled_ids[unhandled_cnt] = -1;
							unhandled_cnt++;
					}
				} else {
					unhandled_ids[unhandled_cnt] = rd_data[0];
					unhandled_cnt++;
				}
			}

			break;
		}
		return LPC_OK;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* HID mouse interface init routine */
ErrorCode_t Mouse_Init(USBD_HANDLE_T hUsb,
					   USB_INTERFACE_DESCRIPTOR *pIntfDesc,
					   uint32_t *mem_base,
					   uint32_t *mem_size)
{
	USBD_HID_INIT_PARAM_T hid_param;
	USB_HID_REPORT_T reports_data[1];
	ErrorCode_t ret = LPC_OK;

	/* Do a quick check of if the interface descriptor passed is the right one. */
	if ((pIntfDesc == 0) || (pIntfDesc->bInterfaceClass != USB_DEVICE_CLASS_HUMAN_INTERFACE)) {
		return ERR_FAILED;
	}

	/* init joystick control */
	Board_Joystick_Init();

	/* Init HID params */
	memset((void *) &hid_param, 0, sizeof(USBD_HID_INIT_PARAM_T));
	hid_param.max_reports = 1;
	hid_param.mem_base = *mem_base;
	hid_param.mem_size = *mem_size;
	hid_param.intf_desc = (uint8_t *) pIntfDesc;
	/* user defined functions */
	hid_param.HID_GetReport = Mouse_GetReport;
	hid_param.HID_SetReport = Mouse_SetReport;
	hid_param.HID_EpIn_Hdlr  = Mouse_EpIN_Hdlr;
	hid_param.HID_EpOut_Hdlr  = Mouse_EpOUT_Hdlr;
	/* Init reports_data */
	reports_data[0].len = ProController_ReportDescSize;
	reports_data[0].idle_time = 0;
	reports_data[0].desc = (uint8_t *) &ProController_ReportDescriptor[0];
	hid_param.report_data  = reports_data;

	ret = USBD_API->hid->init(hUsb, &hid_param);

	/* update memory variables */
	*mem_base = hid_param.mem_base;
	*mem_size = hid_param.mem_size;
	/* store stack handle for later use. */
	g_mouse.hUsb = hUsb;

	return ret;
}

/* Mouse tasks */
void Mouse_Tasks(void)
{
//TODO: Not getting a response from the switch...

// 	Is wired controller communicated to differently?
//	Do we need to send additional messages to not talk BT?
//	Are our timestampts/delays wrong and that is throwing things off?
//	Is this related to unhandle EP Out messages?
//	why do we end up in fault handler (sometimes)??

	int cnt = 0;
	static uint8_t status_msg[64] = {
		0x30,
		0xf4,
		0x91,
		0xC0,

		0x80,
		0xC0,	
		0xff,
		0xd7,

		0x71,
		0x90,
		0xb7,
		0x78, 

		0x00,
		0x1d,
		0xfd,
		0x50, 

		0x00,
		0xb4,
		0x0f,
		0xec, 

		0xff,
		0x00,
		0x00,
		0xfd, 

		0xff,
		0x1c,
		0xfd,
		0x52, 

		0x00,
		0xb8,
		0x0f,
		0xec
	};

	status_msg[1] += 4;

	if (!send_updates) {
		__WFI();
	} else { 
		// TODO: what if cnt is not 64?
		cnt = USBD_API->hw->WriteEP(g_mouse.hUsb, HID_EP_IN, status_msg, 64);
		for (volatile int cnt = 0; cnt < 0x20000; cnt++);
	}

#if (0)
	/* check device is configured before sending report. */
	if ( USB_IsConfigured(g_mouse.hUsb)) {
		if (g_mouse.tx_busy == 0) {
			/* update report based on board state */
			Mouse_UpdateReport();
			/* send report data */
			g_mouse.tx_busy = 1;
			USBD_API->hw->WriteEP(g_mouse.hUsb, HID_EP_IN, &g_mouse.report[0], MOUSE_REPORT_SIZE);
		}
	}
	else {
		/* reset busy flag if we get disconnected. */
		g_mouse.tx_busy = 0;
	}
#endif
}
