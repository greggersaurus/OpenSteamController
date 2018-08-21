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

#include "app_usbd_cfg.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/**
 * HID Pro Controller Report Descriptor
 */
const uint8_t ProController_ReportDescriptor[] = {
#ifdef SWITCH_WIRED
	//TODO: Convert these all to HID_* macros
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
#endif
#ifdef SWITCH_PRO
	HID_UsagePage(HID_USAGE_PAGE_GENERIC), // 05 01 
	HID_LogicalMin(0), // 15 00 
	HID_Usage(HID_USAGE_GENERIC_JOYSTICK), // 09 04 
	HID_Collection(HID_Application), // a1 01 

		HID_ReportID(48), // 85 30 
		HID_UsagePage(HID_USAGE_PAGE_GENERIC), // 05 01 
		HID_UsagePage(HID_USAGE_PAGE_BUTTON), // 05 09 
		HID_UsageMin(1), // 19 01 
		HID_UsageMax(10), // 29 0a 
		HID_LogicalMin(0), // 15 00 
		HID_LogicalMax(1), // 25 01 
		HID_ReportSize(1), // 75 01 
		HID_ReportCount(10), // 95 0a 
		HID_UnitExponent(0), // 55 00 
		HID_Unit(0), // 65 00 
		HID_Input(HID_Data | HID_Variable | HID_Absolute), // 81 02 

		HID_UsagePage(HID_USAGE_PAGE_BUTTON), // 05 09 
		HID_UsageMin(11), // 19 0b 
		HID_UsageMin(14), // 29 0e 
		HID_LogicalMin(0), // 15 00 
		HID_LogicalMax(1), // 25 01 
		HID_ReportSize(1), // 75 01 
		HID_ReportCount(4), // 95 04 
		HID_Input(HID_Data | HID_Variable | HID_Absolute), // 81 02 

		HID_ReportSize(1), // 75 01 
		HID_ReportCount(2), // 95 02 
		HID_Input(HID_Constant | HID_Variable), // 81 03 

		0x0b, HID_USAGE_GENERIC_POINTER, 0x00, 0x01, 0x00, // 0b 01 00 01 00 // TODO: Make sense of this...

		HID_Collection(HID_Physical), // a1 00 

			0x0b, HID_USAGE_GENERIC_X, 0x00, 0x01, 0x00, // 0b 30 00 01 00 // TODO: Make sense of this...
			0x0b, HID_USAGE_GENERIC_Y, 0x00, 0x01, 0x00, // 0b 31 00 01 00 // TODO: Make sense of this...
			0x0b, HID_USAGE_GENERIC_Z, 0x00, 0x01, 0x00, // 0b 32 00 01 00 // TODO: Make sense of this...
			0x0b, HID_USAGE_GENERIC_RX, 0x00, 0x01, 0x00, // 0b 35 00 01 00 // TODO: Make sense of this...

			HID_LogicalMin(0), // 15 00 
			HID_LogicalMaxL(65535), // 27 ff ff 00 00 
			HID_ReportSize(16), // 75 10 
			HID_ReportCount(4), // 95 04 
			HID_Input(HID_Data | HID_Variable | HID_Absolute), // 81 02 

		HID_EndCollection, // c0 

		0x0b, HID_USAGE_GENERIC_HATSWITCH, 0x00, 0x01, 0x00, // 0b 39 00 01 00 // TODO: Make sense of this...

		HID_LogicalMin(0), // 15 00 
		HID_LogicalMax(7), // 25 07 
		HID_PhysicalMin(0), // 35 00 
		HID_PhysicalMaxS(315), // 46 3b 01 
		HID_Unit(20), // 65 14 
		HID_ReportSize(4), // 75 04 
		HID_ReportCount(1), // 95 01 
		HID_Input(HID_Data | HID_Variable | HID_Absolute), // 81 02 

		HID_UsagePage(HID_USAGE_PAGE_BUTTON), /// 05 09 

		HID_UsageMin(15), /// 19 0f 
		HID_UsageMax(18), /// 29 12 
		HID_LogicalMin(0), // 15 00 
		HID_LogicalMax(0), // 25 01 
		HID_ReportSize(1), // 75 01 
		HID_ReportCount(4), // 95 04 
		HID_Input(HID_Data | HID_Variable | HID_Absolute), // 81 02 

		HID_ReportSize(8), // 75 08 
		HID_ReportCount(52), // 95 34 
		HID_Input(HID_Constant | HID_Variable | HID_Absolute), // 81 03 

		HID_UsagePageVendor(0), // 06 00 ff 

		HID_ReportID(33), // 85 21 
		HID_Usage(0x01), // 09 01 
		HID_ReportSize(8), // 75 08 
		HID_ReportCount(63), // 95 3f 
		HID_Input(HID_Constant | HID_Variable | HID_Absolute), // 81 03 

		HID_ReportID(129), // 85 81 
		HID_Usage(0x02), // 09 02 
		HID_ReportSize(8), // 75 08 
		HID_ReportCount(63), // 95 3f 
		HID_Input(HID_Constant | HID_Variable | HID_Absolute), // 81 03 

		HID_ReportID(1), // 85 01 
		HID_Usage(0x03), // 09 03 
		HID_ReportSize(8), // 75 08 
		HID_ReportCount(63), // 95 3f 
		HID_Output(HID_Constant | HID_Variable | HID_Absolute | HID_Volatile), // 91 83 

		HID_ReportID(16), // 85 10 
		HID_Usage(0x04), // 09 04 
		HID_ReportSize(8), // 75 08 
		HID_ReportCount(63), // 95 3f 
		HID_Output(HID_Constant | HID_Variable | HID_Absolute | HID_Volatile), // 91 83 

		HID_ReportID(128), // 85 80 
		HID_Usage(0x05), // 09 05 
		HID_ReportSize(8), // 75 08 
		HID_ReportCount(63), // 95 3f 
		HID_Output(HID_Constant | HID_Variable | HID_Absolute | HID_Volatile), // 91 83 

		HID_ReportID(130), // 85 82 
		HID_Usage(0x06), // 09 06
		HID_ReportSize(8), // 75 08 
		HID_ReportCount(63), // 95 3f 
		HID_Output(HID_Constant | HID_Variable | HID_Absolute | HID_Volatile), // 91 83 
	HID_EndCollection, // c0 
#endif
};
const uint16_t ProController_ReportDescSize = sizeof(ProController_ReportDescriptor);

/**
 * USB Standard Device Descriptor
 */
ALIGNED(4) const uint8_t USB_DeviceDescriptor[] = {
#ifdef SWITCH_WIRED
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
#endif
#ifdef SWITCH_PRO
	USB_DEVICE_DESC_SIZE, /* bLength */
	USB_DEVICE_DESCRIPTOR_TYPE, /* bDescriptorType */
	WBVAL(0x0200), /* bcdUSB : 2.00*/
	0x00, /* bDeviceClass */
	0x00, /* bDeviceSubClass */
	0x00, /* bDeviceProtocol */
	USB_MAX_PACKET0, /* bMaxPacketSize0 */
	WBVAL(0x057e), /* idVendor */
	WBVAL(0x2009), /* idProduct */
	WBVAL(0x0200), /* bcdDevice : 2.00 */
	0x01, /* iManufacturer */
	0x02, /* iProduct */
	0x03, /* iSerialNumber */
	0x01 /* bNumConfigurations */
#endif
};

/**
 * USB FSConfiguration Descriptor
 * All Descriptors (Configuration, Interface, Endpoint, Class, Vendor)
 */
ALIGNED(4) uint8_t USB_FsConfigDescriptor[] = {
#ifdef SWITCH_WIRED
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
	WBVAL(sizeof(ProController_ReportDescriptor)), /* wDescriptorLength */

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
#endif
#ifdef SWITCH_PRO
	/* Configuration 1 */
	USB_CONFIGURATION_DESC_SIZE, /* bLength */
	USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType */
	WBVAL( /* wTotalLength */
		USB_CONFIGURATION_DESC_SIZE   +
		USB_INTERFACE_DESC_SIZE       +
		HID_DESC_SIZE                 +
		USB_ENDPOINT_DESC_SIZE +
		7 //TODO: why this additional 7? should this be USB_ENDPOINT_DESC_SIZE??
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
	WBVAL(sizeof(ProController_ReportDescriptor)), /* wDescriptorLength */
	/* Endpoint, HID Interrupt In */
	USB_ENDPOINT_DESC_SIZE, /* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType */
	HID_EP_IN, /* bEndpointAddress */
	USB_ENDPOINT_TYPE_INTERRUPT, /* bmAttributes */
	WBVAL(0x0040), /* wMaxPacketSize */
	8, /* bInterval */
	/* Endpoint, HID Interrupt Out */
	USB_ENDPOINT_DESC_SIZE, /* bLength */
	USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType */
	HID_EP_OUT, /* bEndpointAddress */
	USB_ENDPOINT_TYPE_INTERRUPT, /* bmAttributes */
	WBVAL(0x0040), /* wMaxPacketSize */
	8, /* bInterval */
	/* Terminator */
	0 /* bLength */
#endif
};

/**
 * USB String Descriptor (optional)
 */
const uint8_t USB_StringDescriptor[] = {
#ifdef SWITCH_WIRED
	/* Index 0x00: LANGID Codes */
	0x04, /* bLength */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
	WBVAL(0x0409), /* wLANGID : US English */
	/* Index 0x01: Manufacturer */
	(40 * 2 + 2), /* bLength (40 Char + Type + lenght) */
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
	(28 * 2 + 2), /* bLength (28 Char + Type + lenght) */
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
	(12 * 2 + 2), /* bLength (12 Char + Type + lenght) */
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
	(3 * 2 + 2), /* bLength (9 Char + Type + lenght) */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */ //TODO: this correct?
	'H', 0,
	'I', 0,
	'D', 0,
#endif
#ifdef SWITCH_PRO
	/* Index 0x00: LANGID Codes */
	0x04, /* bLength */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
	WBVAL(0x0409), /* wLANGID : US English */
	/* Index 0x01: Manufacturer */
	(18 * 2 + 2), /* bLength (18 Char + Type + lenght) */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
	'N', 0,
	'i', 0,
	'n', 0,
	't', 0,
	'e', 0,
	'n', 0,
	'd', 0,
	'o', 0,
	' ', 0,
	'C', 0,
	'o', 0,
	'.', 0,
	',', 0,
	' ', 0,
	'L', 0,
	't', 0,
	'd', 0,
	'.', 0,
	/* Index 0x02: Product */
	(14 * 2 + 2), /* bLength (14 Char + Type + lenght) */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
	'P', 0,
	'r', 0,
	'o', 0,
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
	(12 * 2 + 2), /* bLength (12 Char + Type + lenght) */
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
	(3 * 2 + 2), /* bLength (9 Char + Type + lenght) */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */ //TODO: this correct?
	'H', 0,
	'I', 0,
	'D', 0,
#endif
};
