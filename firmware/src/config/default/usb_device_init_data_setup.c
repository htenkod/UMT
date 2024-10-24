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

/* MISRA C-2012 Rule 10.3, 11.1 and 11.8 deviated below. Deviation record ID -  
   H3_USB_MISRAC_2012_R_10_3_DR_1, H3_USB_MISRAC_2012_R_11_1_DR_1 & H3_USB_MISRAC_2012_R_11_8_DR_1*/
/***********************************************
 * Sector buffer needed by for the MSD LUN.
 ***********************************************/
static uint8_t sectorBuffer[512 * USB_DEVICE_MSD_NUM_SECTOR_BUFFERS] USB_ALIGN;

/***********************************************
 * CBW and CSW structure needed by for the MSD
 * function driver instance.
 ***********************************************/
static uint8_t msdCBW0[512] USB_ALIGN;
static USB_MSD_CSW msdCSW0 USB_ALIGN;


/*******************************************
 * MSD Function Driver initialization
 *******************************************/
static USB_DEVICE_MSD_MEDIA_INIT_DATA USB_ALIGN  msdMediaInit0[1] =
{
    /* LUN 0 */ 
    {
        DRV_MEMORY_INDEX_0,
        512,
        sectorBuffer,
        NULL,
        0,
        {
            0x00,    // peripheral device is connected, direct access block device
            0x80,   // removable
            0x04,    // version = 00=> does not conform to any standard, 4=> SPC-2
            0x02,    // response is in format specified by SPC-2
            0x1F,    // additional length
            0x00,    // sccs etc.
            0x00,    // bque=1 and cmdque=0,indicates simple queueing 00 is obsolete,
                     // but as in case of other device, we are just using 00
            0x00,    // 00 obsolete, 0x80 for basic task queueing
            {
                'M','i','c','r','o','c','h','p'
            },
            {
                'M','a','s','s',' ','S','t','o','r','a','g','e',' ',' ',' ',' '
            },
            {
                '0','0','0','1'
            }
        },
        {
            DRV_MEMORY_IsAttached,
            DRV_MEMORY_Open,
            DRV_MEMORY_Close,
            DRV_MEMORY_GeometryGet,
            DRV_MEMORY_AsyncRead,
            DRV_MEMORY_AsyncEraseWrite,
            DRV_MEMORY_IsWriteProtected,
            DRV_MEMORY_TransferHandlerSet,
            NULL
        }
    },
};
  
/**************************************************
 * USB Device Function Driver Init Data
 **************************************************/
static const USB_DEVICE_MSD_INIT msdInit0 =
{
    .numberOfLogicalUnits = 1,
    .msdCBW = (USB_MSD_CBW*)&msdCBW0,
    .msdCSW = &msdCSW0,
    .mediaInit = &msdMediaInit0[0]
};

/* MISRAC 2012 deviation block end */



/**************************************************
 * USB Device Layer Function Driver Registration
 * Table
 **************************************************/
/* MISRA C-2012 Rule 10.3 deviated:2, 11.8 deviated:6 deviated below. Deviation record ID -  
   H3_USB_MISRAC_2012_R_10_3_DR_1 & H3_USB_MISRAC_2012_R_11_8_DR_1*/
static const USB_DEVICE_FUNCTION_REGISTRATION_TABLE funcRegistrationTable_setup[1] =
{
    
    /* MSD Function 0 */
    { 
        .configurationValue = 1,    /* Configuration value */ 
        .interfaceNumber = 1,       /* First interfaceNumber of this function */ 
        .speed = USB_SPEED_HIGH|USB_SPEED_FULL,    /* Function Speed */ 
        .numberOfInterfaces = 1,    /* Number of interfaces */
        .funcDriverIndex = 0,  /* Index of MSD Function Driver */
        .driver = (void*)USB_DEVICE_MSD_FUNCTION_DRIVER,    /* USB MSD function data exposed to device layer */
        .funcDriverInit = (void*)&msdInit0    /* Function driver init data */
    }

};
/* MISRAC 2012 deviation block end */
/*******************************************
 * USB Device Layer Descriptors
 *******************************************/
/*******************************************
 *  USB Device Descriptor
 *******************************************/
static const USB_DEVICE_DESCRIPTOR usbDeviceDescriptor_setup =
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
static const USB_DEVICE_QUALIFIER deviceQualifierDescriptor1_setup =
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
static const uint8_t highSpeedConfigurationDescriptor_setup[]=
{
    /* Configuration Descriptor */

    0x09,                                               // Size of this descriptor in bytes
    (uint8_t)USB_DESCRIPTOR_CONFIGURATION,                       // Descriptor Type
//    USB_DEVICE_16bitTo8bitArrange(69),                  //(46 Bytes)Size of the Configuration descriptor
    0x20, 0x00,                                         //(46 Bytes)Size of the Configuration descriptor
    2,                                                  // Number of interfaces in this configuration
    0x01,                                               // Index value of this configuration
    0x00,                                               // Configuration string index
    USB_ATTRIBUTE_DEFAULT | USB_ATTRIBUTE_SELF_POWERED, // Attributes
    50,                                                 // Maximum power consumption (mA) /2


        /* Descriptor for Function 1 - MSD     */ 
    
    /* Interface Descriptor */

    9,                              // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,       // INTERFACE descriptor type
    1,                              // Interface Number
    0,                              // Alternate Setting Number
    2,                              // Number of endpoints in this intf
    USB_MSD_CLASS_CODE,             // Class code
    USB_MSD_SUBCLASS_CODE_SCSI_TRANSPARENT_COMMAND_SET, // Subclass code
    USB_MSD_PROTOCOL,               // Protocol code
    0,                              // Interface string index

    /* Endpoint Descriptor */

    7,                          // Size of this descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,    // Endpoint Descriptor
    3  | USB_EP_DIRECTION_IN,    // EndpointAddress ( EP1 IN )
    (uint8_t)USB_TRANSFER_TYPE_BULK,     // Attributes type of EP (BULK)
    0x00,0x02,                  // Max packet size of this EP
    0x00,                       // Interval (in ms)


    7,                          // Size of this descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,    // Endpoint Descriptor
    3  | USB_EP_DIRECTION_OUT,   // EndpointAddress ( EP2 OUT )
    (uint8_t)USB_TRANSFER_TYPE_BULK,     // Attributes type of EP (BULK)
    0x00,0x02,                  // Max packet size of this EP
    0x00,                       // Interval (in ms)
    
};
/* MISRAC 2012 deviation block end */   
/*******************************************
 * Array of High speed config descriptors
 *******************************************/
static USB_DEVICE_CONFIGURATION_DESCRIPTORS_TABLE highSpeedConfigDescSet_setup[1] =
{
    highSpeedConfigurationDescriptor_setup
};

/*******************************************
 *  USB Full Speed Configuration Descriptor
 *******************************************/
 /* MISRA C-2012 Rule 10.3 deviated:25 Deviation record ID -  H3_USB_MISRAC_2012_R_10_3_DR_1 */
static const uint8_t fullSpeedConfigurationDescriptor_setup[]=
{
    /* Configuration Descriptor */

    0x09,                                                   // Size of this descriptor in bytes
    (uint8_t)USB_DESCRIPTOR_CONFIGURATION,                           // Descriptor Type
//    USB_DEVICE_16bitTo8bitArrange(69),                      //(46 Bytes)Size of the Configuration descriptor
    0x20, 0x00,                                             //(69 Bytes)Size of the Configuration descriptor    
    2,                                                      // Number of interfaces in this configuration
    0x01,                                                   // Index value of this configuration
    0x00,                                                   // Configuration string index
    USB_ATTRIBUTE_DEFAULT | USB_ATTRIBUTE_SELF_POWERED, // Attributes
    50,                                                 // Maximum power consumption (mA) /2
    /* Descriptor for Function 1 - MSD     */ 
    
    /* Interface Descriptor */

    9,                              // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,       // INTERFACE descriptor type
    1,                              // Interface Number
    0,                              // Alternate Setting Number
    2,                              // Number of endpoints in this intf
    USB_MSD_CLASS_CODE,             // Class code
    USB_MSD_SUBCLASS_CODE_SCSI_TRANSPARENT_COMMAND_SET, // Subclass code
    USB_MSD_PROTOCOL,               // Protocol code
    0,                              // Interface string index

    /* Endpoint Descriptor */

    7,                          // Size of this descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,    // Endpoint Descriptor
    3  | USB_EP_DIRECTION_IN,    // EndpointAddress ( EP1 IN )
    (uint8_t)USB_TRANSFER_TYPE_BULK,     // Attributes type of EP (BULK)
    0x40,0x00,                  // Max packet size of this EP
    0x00,                       // Interval (in ms)


    7,                          // Size of this descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,    // Endpoint Descriptor
    3  | USB_EP_DIRECTION_OUT,   // EndpointAddress ( EP2 OUT )
    (uint8_t)USB_TRANSFER_TYPE_BULK,     // Attributes type of EP (BULK)
    0x40,0x00,                  // Max packet size of this EP
    0x00,                       // Interval (in ms)
};
/* MISRAC 2012 deviation block end */
/*******************************************
 * Array of Full speed Configuration
 * descriptors
 *******************************************/
static USB_DEVICE_CONFIGURATION_DESCRIPTORS_TABLE fullSpeedConfigDescSet_setup[1] =
{
    fullSpeedConfigurationDescriptor_setup
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

static sd000_setup =
{
    0,                                                  // Index of this string is 0
    0,                                                  // This field is always blank for String Index 0
    sizeof(sd000_setup)-sizeof(sd000_setup.stringIndex)-sizeof(sd000_setup.languageID),
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

static sd001_setup =
{
    1,                                                  // Index of this string descriptor is 1.
    0x0409,                                             // Language ID of this string descriptor is 0x0409 (English)
    sizeof(sd001_setup)-sizeof(sd001_setup.stringIndex)-sizeof(sd001_setup.languageID),
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

static sd002_setup =
{
    2,                                                  // Index of this string descriptor is 2.
    0x0409,                                             // Language ID of this string descriptor is 0x0409 (English)
    sizeof(sd002_setup)-sizeof(sd002_setup.stringIndex)-sizeof(sd002_setup.languageID),
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

USB_STR_DESCR_t sd003_setup =
{
    3,                                                  // Index of this string descriptor is 2.
    0x0409,                                             // Language ID of this string descriptor is 0x0409 (English)
    sizeof(sd003_setup)-sizeof(sd003_setup.stringIndex)-sizeof(sd003_setup.languageID),
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
static microSoftOsDescriptor_setup =
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
static USB_DEVICE_STRING_DESCRIPTORS_TABLE stringDescriptors_setup[5]=
{
    (const uint8_t *const)&sd000_setup,
    (const uint8_t *const)&sd001_setup,
    (const uint8_t *const)&sd002_setup,
    (const uint8_t *const)&sd003_setup,
    (const uint8_t *const)&microSoftOsDescriptor_setup
};

/*******************************************
 * USB Device Layer Master Descriptor Table
 *******************************************/
static const USB_DEVICE_MASTER_DESCRIPTOR usbMasterDescriptor_setup =
{
    &usbDeviceDescriptor_setup,                                      // Full speed descriptor
    1,                                                      // Total number of full speed configurations available
    fullSpeedConfigDescSet_setup,                                 // Pointer to array of full speed configurations descriptors
    &usbDeviceDescriptor_setup,                                      // High speed device descriptor
    1,                                                      // Total number of high speed configurations available
    highSpeedConfigDescSet_setup,                                 // Pointer to array of high speed configurations descriptors
    5,                                                      // Total number of string descriptors available.
    stringDescriptors_setup,                                      // Pointer to array of string descriptors.
    &deviceQualifierDescriptor1_setup,                            // Pointer to full speed dev qualifier.
    &deviceQualifierDescriptor1_setup                             // Pointer to high speed dev qualifier.
};


/****************************************************
 * USB Device Layer Initialization Data
 ****************************************************/
const USB_DEVICE_INIT usbDevInitData_setup =
{
    /* Number of function drivers registered to this instance of the
       USB device layer */
    .registeredFuncCount = 1,

    /* Function driver table registered to this instance of the USB device layer*/
    .registeredFunctions = (USB_DEVICE_FUNCTION_REGISTRATION_TABLE*)funcRegistrationTable_setup,

    /* Pointer to USB Descriptor structure */
    .usbMasterDescriptor = (USB_DEVICE_MASTER_DESCRIPTOR*)&usbMasterDescriptor_setup,

    /* USB Device Speed */
    .deviceSpeed =  USB_SPEED_HIGH,

    /* Index of the USB Driver to be used by this Device Layer Instance */
    .driverIndex = DRV_USBHS_INDEX_0,

    /* Pointer to the USB Driver Functions. */
    .usbDriverInterface = DRV_USBHS_DEVICE_INTERFACE,

    /* Specify queue size for vendor endpoint read */
    .queueSizeEndpointRead = 4,

    /* Specify queue size for vendor endpoint write */
    .queueSizeEndpointWrite= 4
};
// </editor-fold>

