/*******************************************************************************
  System Initialization File

  File Name:
    usb_device_init_data.c

  Summary:
    This file contains source code necessary to initialize the system.

  Description:
    This file contains source code necessary to initialize the system.  It
    implements the "SYS_Initialize" function, defines the configuration bits,
    and allocates any necessary global system resources,
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "definitions.h"
/**************************************************
 * USB Device Function Driver Init Data
 **************************************************/


/**************************************************
 * USB Device Layer Function Driver Registration
 * Table
 **************************************************/
/* MISRA C-2012 Rule 10.3 deviated:2, 11.8 deviated:6 deviated below. Deviation record ID -  
   H3_USB_MISRAC_2012_R_10_3_DR_1 & H3_USB_MISRAC_2012_R_11_8_DR_1*/
static const USB_DEVICE_FUNCTION_REGISTRATION_TABLE funcRegistrationTable[1] =
{
    
    /* Vendor Function 0 */
    { 
        .configurationValue = 1,    /* Configuration value */ 
        .interfaceNumber = 0,       /* First interfaceNumber of this function */ 
        .speed = USB_SPEED_HIGH|USB_SPEED_FULL,    /* Function Speed */ 
        .numberOfInterfaces = 1,    /* Number of interfaces */
        .funcDriverIndex = 0,  /* Index of Function Driver */
        .driver = NULL,    
        .funcDriverInit = NULL    /* Function driver init data */
    },



};
/* MISRAC 2012 deviation block end */
/*******************************************
 * USB Device Layer Descriptors
 *******************************************/
/*******************************************
 *  USB Device Descriptor
 *******************************************/
static const USB_DEVICE_DESCRIPTOR usbDeviceDescriptor =
{
    0x12,                                                   // Size of this descriptor in bytes
    (uint8_t)USB_DESCRIPTOR_DEVICE,                                  // DEVICE descriptor type
    0x0200,                                                 // USB Spec Release Number in BCD format
    0xEF,                                                   // Class Code
    0x02,                                                   // Subclass code
    0x01,                                                   // Protocol code
    USB_DEVICE_EP0_BUFFER_SIZE,                             // Max packet size for EP0, see configuration.h
    0x04D8,                                                 // Vendor ID
    0x0066,                                                 // Product ID
    0x0100,                                                 // Device release number in BCD format
    0x01,                                                   // Manufacturer string index
    0x02,                                                   // Product string index
    0x03,                                                   // Device serial number string index
    0x01                                                    // Number of possible configurations
};

/*******************************************
 *  USB Device Qualifier Descriptor for this
 *  demo.
 *******************************************/
static const USB_DEVICE_QUALIFIER deviceQualifierDescriptor1 =
{
    0x0A,                                                   // Size of this descriptor in bytes
    USB_DESCRIPTOR_DEVICE_QUALIFIER,                        // Device Qualifier Type
    0x0200,                                                 // USB Specification Release number
    0xEF,                                                   // Class Code
    0x02,                                                   // Subclass code
    0x01,                                                   // Protocol code
    USB_DEVICE_EP0_BUFFER_SIZE,                             // Maximum packet size for endpoint 0
    0x01,                                                   // Number of possible configurations
    0x00                                                    // Reserved for future use.
};

/*******************************************
 *  USB High Speed Configuration Descriptor
 *******************************************/
/* MISRA C-2012 Rule 10.3 deviated:25 Deviation record ID -  H3_USB_MISRAC_2012_R_10_3_DR_1 */
static const uint8_t highSpeedConfigurationDescriptor[]=
{
    /* Configuration Descriptor */

    0x09,                                               // Size of this descriptor in bytes
    (uint8_t)USB_DESCRIPTOR_CONFIGURATION,                       // Descriptor Type
    USB_DEVICE_16bitTo8bitArrange(46),                  //(23 Bytes)Size of the Configuration descriptor
    1,                                                  // Number of interfaces in this configuration
    0x01,                                               // Index value of this configuration
    0x00,                                               // Configuration string index
    USB_ATTRIBUTE_DEFAULT | USB_ATTRIBUTE_SELF_POWERED, // Attributes
    50,                                                 // Maximum power consumption (mA) /2

    /* Interface Descriptor */

    0x09,                       // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,   // INTERFACE descriptor type
    0,                          // Interface Number
    0,                          // Alternate Setting Number
    4,                          // Number of endpoints in this intf
    0xFF,                       // Class code
    0xFF,                       // Subclass code
    0xFF,                       // Protocol code
    0,                          // Interface string index

    /* Endpoint (OUT) Descriptor */

    0x07,                       // Size of this descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,    // Endpoint Descriptor 
    1 | USB_EP_DIRECTION_OUT,   // EndpointAddress ( EP1 OUT )
    USB_TRANSFER_TYPE_BULK,     // Attributes
    0x00,0x02,                  // Max packet size of this EP
    1,                          // Interval

    /* Endpoint (IN) Descriptor */

    0x07,                       // Size of this descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,    // Endpoint Descriptor
    1 | USB_EP_DIRECTION_IN,    // EndpointAddress ( EP1 IN )
    USB_TRANSFER_TYPE_BULK,     // Attributes
    0x00,0x02,                  // Max packet size of this EP
    1,                          // Interval

    /* Endpoint (OUT) Descriptor */

    0x07,                       // Size of this descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,    // Endpoint Descriptor 
    2 | USB_EP_DIRECTION_OUT,   // EndpointAddress ( EP1 OUT )
    USB_TRANSFER_TYPE_INTERRUPT,     // Attributes
    0x00,0x02,                  // Max packet size of this EP
    1,  


	/* Endpoint (IN) Descriptor */
	
	0x07,						// Size of this descriptor in bytes
	USB_DESCRIPTOR_ENDPOINT,	// Endpoint Descriptor
	2 | USB_EP_DIRECTION_IN,	// EndpointAddress ( EP1 IN )
	USB_TRANSFER_TYPE_INTERRUPT,// Attributes
	0x00,0x02,					// Max packet size of this EP
	1,							// Interval

};
/* MISRAC 2012 deviation block end */   
/*******************************************
 * Array of High speed config descriptors
 *******************************************/
static USB_DEVICE_CONFIGURATION_DESCRIPTORS_TABLE highSpeedConfigDescSet[1] =
{
    highSpeedConfigurationDescriptor
};

/*******************************************
 *  USB Full Speed Configuration Descriptor
 *******************************************/
 /* MISRA C-2012 Rule 10.3 deviated:25 Deviation record ID -  H3_USB_MISRAC_2012_R_10_3_DR_1 */
static const uint8_t fullSpeedConfigurationDescriptor[]=
{
    /* Configuration Descriptor */

    0x09,                                                   // Size of this descriptor in bytes
    (uint8_t)USB_DESCRIPTOR_CONFIGURATION,                           // Descriptor Type
    USB_DEVICE_16bitTo8bitArrange(46),                      //(23 Bytes)Size of the Configuration descriptor
    1,                                                      // Number of interfaces in this configuration
    0x01,                                                   // Index value of this configuration
    0x00,                                                   // Configuration string index
    USB_ATTRIBUTE_DEFAULT | USB_ATTRIBUTE_SELF_POWERED, // Attributes
    50,                                                 // Maximum power consumption (mA) /2
    /* Interface Descriptor */

    0x09,                       // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,   // INTERFACE descriptor type
    0,                          // Interface Number
    0,                          // Alternate Setting Number
    4,                          // Number of endpoints in this intf
    0xFF,                       // Class code
    0xFF,                       // Subclass code
    0xFF,                       // Protocol code
    0,                          // Interface string index

    /* Endpoint (OUT) Descriptor */

    0x07,                       // Size of this descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,    // Endpoint Descriptor 
    1 | USB_EP_DIRECTION_OUT,   // EndpointAddress ( EP1 OUT )
    USB_TRANSFER_TYPE_BULK,     // Attributes
    0x40,0x00,                  // Max packet size of this EP
    1,                          // Interval

    /* Endpoint (IN) Descriptor */

    0x07,                       // Size of this descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,    // Endpoint Descriptor
    1 | USB_EP_DIRECTION_IN,    // EndpointAddress ( EP1 IN )
    USB_TRANSFER_TYPE_BULK,     // Attributes
    0x40,0x00,                  // Max packet size of this EP
    1,                          // Interval

    /* Endpoint (OUT) Descriptor */

    0x07,                       // Size of this descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,    // Endpoint Descriptor 
    2 | USB_EP_DIRECTION_OUT,   // EndpointAddress ( EP1 OUT )
    USB_TRANSFER_TYPE_INTERRUPT,     // Attributes
    0x40,0x00,                  // Max packet size of this EP
    1,                          // Interval

	/* Endpoint (IN) Descriptor */
	
	0x07,						// Size of this descriptor in bytes
	USB_DESCRIPTOR_ENDPOINT,	// Endpoint Descriptor
	2 | USB_EP_DIRECTION_IN,	// EndpointAddress ( EP1 IN )
	USB_TRANSFER_TYPE_INTERRUPT,// Attributes
	0x40,0x00,					// Max packet size of this EP
	1,	

};
/* MISRAC 2012 deviation block end */
/*******************************************
 * Array of Full speed Configuration
 * descriptors
 *******************************************/
static USB_DEVICE_CONFIGURATION_DESCRIPTORS_TABLE fullSpeedConfigDescSet[1] =
{
    fullSpeedConfigurationDescriptor
};

/**************************************
 *  String descriptors.
 *************************************/
/*******************************************
*  Language code string descriptor
*******************************************/
const struct __attribute__ ((packed))
{
    uint8_t stringIndex;                                //Index of the string descriptor
    uint16_t languageID ;                               // Language ID of this string.
    uint8_t bLength;                                    // Size of this descriptor in bytes
    uint8_t bDscType;                                   // STRING descriptor type
    uint16_t string[1];                                 // String
}

static sd000 =
{
    0,                                                  // Index of this string is 0
    0,                                                  // This field is always blank for String Index 0
    sizeof(sd000)-sizeof(sd000.stringIndex)-sizeof(sd000.languageID),
    USB_DESCRIPTOR_STRING,
    {0x0409}                                            // Language ID
};

/*******************************************
 *  Manufacturer string descriptor
 *******************************************/
/* MISRA C-2012 Rule 10.3 deviated:43 Deviation record ID -  H3_USB_MISRAC_2012_R_10_3_DR_1 */

const struct __attribute__ ((packed))
{
    uint8_t stringIndex;                                //Index of the string descriptor
    uint16_t languageID ;                               // Language ID of this string.
    uint8_t bLength;                                    // Size of this descriptor in bytes
    uint8_t bDscType;                                   // STRING descriptor type
    uint16_t string[25];                                // String
}

static sd001 =
{
    1,                                                  // Index of this string descriptor is 1.
    0x0409,                                             // Language ID of this string descriptor is 0x0409 (English)
    sizeof(sd001)-sizeof(sd001.stringIndex)-sizeof(sd001.languageID),
    USB_DESCRIPTOR_STRING,
    {'M','i','c','r','o','c','h','i','p',' ','T','e','c','h','n','o','l','o','g','y',' ','I','n','c','.'}
};

/*******************************************
 *  Product string descriptor
 *******************************************/
const struct __attribute__ ((packed))
{
    uint8_t stringIndex;                                //Index of the string descriptor
    uint16_t languageID ;                               // Language ID of this string.
    uint8_t bLength;                                    // Size of this descriptor in bytes
    uint8_t bDscType;                                   // STRING descriptor type
    uint16_t string[23];                                // String
}

static sd002 =
{
    2,                                                  // Index of this string descriptor is 2.
    0x0409,                                             // Language ID of this string descriptor is 0x0409 (English)
    sizeof(sd002)-sizeof(sd002.stringIndex)-sizeof(sd002.languageID),
    USB_DESCRIPTOR_STRING,
    {'M','C','H','P',' ','M','o','d','u','l','e',' ','T','e','s','t',' ','D','e','v','i','c','e'}
};

/*******************************************
 *  Serial number string descriptor
 *******************************************/
typedef struct __attribute__ ((packed))
{
    uint8_t stringIndex;                                //Index of the string descriptor
    uint16_t languageID ;                               // Language ID of this string.
    uint8_t bLength;                                    // Size of this descriptor in bytes
    uint8_t bDscType;                                   // STRING descriptor type
    uint16_t string[12];                                // String
}USB_STR_DESCR_t;

USB_STR_DESCR_t sd003 =
{
    3,                                                  // Index of this string descriptor is 2.
    0x0409,                                             // Language ID of this string descriptor is 0x0409 (English)
    sizeof(sd003)-sizeof(sd003.stringIndex)-sizeof(sd003.languageID),
    USB_DESCRIPTOR_STRING,    
    {'U','M','T','#','1','2','3','4','5','6', '7', '8'}
};
/* MISRAC 2012 deviation block end */

/*******************************************
*  MS OS string descriptor
*******************************************/
const struct __attribute__ ((packed))
{
    uint8_t stringIndex;    //Index of the string descriptor
    uint16_t languageID ;   // Language ID of this string.
    uint8_t bLength;        // Size of this descriptor in bytes
    uint8_t bDscType;       // STRING descriptor type
    uint16_t string[18];    // String
}
static microSoftOsDescriptor =
{
    0xEE,       /* This value is per Microsoft OS Descriptor documentation */
    0x0000,     /* Language ID is 0x0000 as per Microsoft Documentation */
    18,         /* Size is 18 Bytes as Microsoft documentation */
    USB_DESCRIPTOR_STRING,
    /* Vendor code to retrieve OS feature descriptors.  */
    /* qwSignature = MSFT100 */
    /* Vendor Code = User Defined Value */
    {'M','S','F','T','1','0','0',0x0100 | USB_DEVICE_MICROSOFT_OS_DESCRIPTOR_VENDOR_CODE}
};

/***************************************
 * Array of string descriptors
 ***************************************/
static USB_DEVICE_STRING_DESCRIPTORS_TABLE stringDescriptors[5]=
{
    (const uint8_t *const)&sd000,
    (const uint8_t *const)&sd001,
    (const uint8_t *const)&sd002,
    (const uint8_t *const)&sd003,
    (const uint8_t *const)&microSoftOsDescriptor
};

/*******************************************
 * USB Device Layer Master Descriptor Table
 *******************************************/
static const USB_DEVICE_MASTER_DESCRIPTOR usbMasterDescriptor =
{
    &usbDeviceDescriptor,                                      // Full speed descriptor
    1,                                                      // Total number of full speed configurations available
    fullSpeedConfigDescSet,                                 // Pointer to array of full speed configurations descriptors
    &usbDeviceDescriptor,                                      // High speed device descriptor
    1,                                                      // Total number of high speed configurations available
    highSpeedConfigDescSet,                                 // Pointer to array of high speed configurations descriptors
    5,                                                      // Total number of string descriptors available.
    stringDescriptors,                                      // Pointer to array of string descriptors.
    &deviceQualifierDescriptor1,                            // Pointer to full speed dev qualifier.
    &deviceQualifierDescriptor1                             // Pointer to high speed dev qualifier.
};


/****************************************************
 * USB Device Layer Initialization Data
 ****************************************************/
const USB_DEVICE_INIT usbDevInitData =
{
    /* Number of function drivers registered to this instance of the
       USB device layer */
    .registeredFuncCount = 1,

    /* Function driver table registered to this instance of the USB device layer*/
    .registeredFunctions = (USB_DEVICE_FUNCTION_REGISTRATION_TABLE*)funcRegistrationTable,

    /* Pointer to USB Descriptor structure */
    .usbMasterDescriptor = (USB_DEVICE_MASTER_DESCRIPTOR*)&usbMasterDescriptor,

    /* USB Device Speed */
    .deviceSpeed =  USB_SPEED_HIGH,

    /* Index of the USB Driver to be used by this Device Layer Instance */
    .driverIndex = DRV_USBHS_INDEX_0,

    /* Pointer to the USB Driver Functions. */
    .usbDriverInterface = DRV_USBHS_DEVICE_INTERFACE,

    /* Specify queue size for vendor endpoint read */
    .queueSizeEndpointRead = 10,

    /* Specify queue size for vendor endpoint write */
    .queueSizeEndpointWrite= 10
};
// </editor-fold>


