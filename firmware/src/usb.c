/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    usb.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
#if defined (__PIC32MZ__) || defined (__PIC32MX__) || defined(__PIC32CX1025SG41128__) || defined (_SAMD21J18A_H_) || defined (_SAME54P20A_H_) ||  defined (__PIC32MM__) ||  defined (_PIC32CM5164LE00100_H_) || defined (__PIC32CZ__) || defined (__PIC32CK2051GC01144__)
#define APP_EP_BULK_OUT 1
#define APP_EP_BULK_IN 1
#define APP_EP_INT_OUT 2
#define APP_EP_INT_IN 2
#else
#define APP_EP_BULK_OUT 1
#define APP_EP_BULK_IN 2
#endif 

typedef struct __attribute__ ((packed))
{
    uint8_t stringIndex;                                //Index of the string descriptor
    uint16_t languageID ;                               // Language ID of this string.
    uint8_t bLength;                                    // Size of this descriptor in bytes
    uint8_t bDscType;                                   // STRING descriptor type
    uint16_t string[12];                                // String
}USB_STR_DESCR_t;
// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the USB_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

/* Extended Compat ID Feature Descriptor */
const uint8_t microsoftExtendedCompatIdDescriptor[] =
{
0x28, 0x00, 0x00, 0x00, /* dwLength Length of this descriptor */
0x00, 0x01, /* bcdVersion = Version 1.0 */
0x04, 0x00, /* wIndex = 0x0004 */
0x01, /* bCount = 1 */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Reserved */
0x00, /* Interface number = 0 */
0x01, /* Reserved */
0x57, 0x49, 0x4E, 0x55, 0x53, 0x42, 0x00, 0x00, /* compatibleID */ //WINUSB
//0x4C, 0x49, 0x42, 0x55, 0x53, 0x42, 0x30, 0x00, /* compatibleID */ //LIBUSB0
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* subCompatibleID */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00 /* Reserved */
};


const uint8_t microsoftExtendedFeatureIdDescriptor[] = {
0x8E, 0, 0, 0, /* length 214+10=224(0xE0) byte */     
0x00, 0x01,    /* BCD version 1.0 */    
0x05, 0x00,    /* Extended Property Descriptor Index(5) */    
0x01, 0x00,    /* number of section (1) */    /* 0xD6(214) = 50+2(property section) + 162(property data ) */    
/*property section */    
0x84, 0x00, 0x00, 0x00, /* 214 size of property section */    
0x1, 0, 0, 0, /*  property data type (1) 0x07 (REG_MULTI_SZ) */    
0x28, 0,      /* property name length (42)'s', 0, */    
'D', 0,    'e', 0,    'v', 0,    'i', 0,    'c', 0,    'e', 0,    'I', 0,    'n', 0,    't', 0,    'e', 0,    'r', 0,    'f', 0,    'a', 0,    'c', 0,    'e', 0,    'G', 0,    'U', 0,    'I', 0,    'D', 0,   0, 0,   
/* 0x9E(158) = 0x4E(78)x2+2    * 162= 0x9E(158)+4    */    
0x4E, 0, 0, 0,  /*  property data length */    
'{', 0,    '1', 0,    '3', 0,    'E', 0,    'B', 0,    '3', 0,    '6', 0,    '0', 0,    'B', 0,    '-', 0,    'B', 0,    'C', 0,    '1', 0,    'E', 0,    '-', 0,    '4', 0,    '6', 0,    'C', 0,    'B', 0,    '-', 0,    'A', 0,    'C', 0,    '8', 0,    'B', 0,    '-', 0,    'E', 0,    'F', 0,    '3', 0,    'D', 0,    'A', 0,    '4', 0,    '7', 0,    'B', 0,    '4', 0,    '0', 0,    '6', 0,    '2', 0,    '}', 0,    0, 0, 
};



USB_DATA usbData;

/* Receive data buffer */
uint8_t receivedDataBuffer[USB_READ_BUFFER_SIZE] CACHE_ALIGN;

/* Transmit data buffer */
uint8_t  transmitDataBuffer[USB_WRITE_BUFFER_SIZE] CACHE_ALIGN;



/* Receive data buffer */
uint8_t usbRxDataCircuBuff[USB_RX_CIRC_BUFF_SIZE] CACHE_ALIGN;

/* Transmit data buffer */
uint8_t  usbTxDataCircuBuff[USB_TX_CIRC_BUFF_SIZE] CACHE_ALIGN;


volatile static UART_RING_BUFFER_OBJECT usbBufferObj;


static const SYS_CMD_INIT usbCmdInitData = {     
	.moduleInit = {0},
    .consoleCmdIOParam = SYS_CMD_SINGLE_CHARACTER_READ_CONSOLE_IO_PARAM,
	.consoleIndex = 1,
	};       // data the command processor has been initialized with

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************
void U1RX_Callback_Handler(uintptr_t context)
{
	// write it back to the USB Async Pipe



}



/******************************************************************************
  Function:
    void APP_USBDeviceEventHandler
    (
        USB_DEVICE_EVENTS event
        void * pEventData,
        uintptr_t context
    );

  Remarks:
    Handles the Device Layers Events.
*/

void USB_MSD_DeviceEventHandler( USB_DEVICE_EVENT event, void * pEventData, uintptr_t context )
{
    /* This is an example of how the context parameter
       in the event handler can be used.*/

    USB_DATA * usbDataObject = (USB_DATA*)context;

    switch( event )
    {
        case USB_DEVICE_EVENT_RESET:
        case USB_DEVICE_EVENT_DECONFIGURED:
            usbDataObject->deviceIsConfigured = false;
            /* Device was reset or de-configured. Update LED status */
            LED2_Off();
            break;

        case USB_DEVICE_EVENT_CONFIGURED:
            usbDataObject->deviceIsConfigured = true;
            /* Device is configured. Update LED status */
            LED2_On();
            break;

        case USB_DEVICE_EVENT_SUSPENDED:
            
            LED2_Off();
            break;

        case USB_DEVICE_EVENT_POWER_DETECTED:

            /* VBUS is detected. Attach the device. */
            USB_DEVICE_Attach(usbDataObject->deviceHandle);
            break;

        case USB_DEVICE_EVENT_POWER_REMOVED:
            usbData.deviceIsConfigured = false;
            /* VBUS is not detected. Detach the device */
            USB_DEVICE_Detach(usbDataObject->deviceHandle);
            LED2_Off();
            break;

        /* These events are not used in this demo */
        case USB_DEVICE_EVENT_RESUMED:
            if(usbDataObject->deviceIsConfigured == true)
            {
                LED2_On();
            }
            break;
        case USB_DEVICE_EVENT_ERROR:
        case USB_DEVICE_EVENT_SOF:
        default:
            break;
    }
}


/* TODO:  Add any necessary callback functions.
*/
// This is the application device layer event handler function.

USB_DEVICE_EVENT_RESPONSE USB_Test_DeviceEventCallBack
(
    USB_DEVICE_EVENT event,
    void * pData, 
    uintptr_t context
)
{
    uint8_t * configurationValue;
    USB_SETUP_PACKET * setupPacket;
        
    switch(event)
    {
        case USB_DEVICE_EVENT_RESET:
        case USB_DEVICE_EVENT_DECONFIGURED:

            /* Device is reset or deconfigured. Provide LED indication.*/
            LED1_Off();

            usbData.deviceIsConfigured = false;

            break;

        case USB_DEVICE_EVENT_CONFIGURED:

            /* Check the configuration */
            configurationValue = (uint8_t *)pData;
            if(*configurationValue == 1 )
            {
                /* The device is in configured state. Update LED indication */
                
                                               
                /* Reset endpoint data send & receive flag  */
                usbData.deviceIsConfigured = true;
            }
            break;

        case USB_DEVICE_EVENT_SUSPENDED:

			LED1_Off();
            /* Device is suspended. */ 
            break;


        case USB_DEVICE_EVENT_POWER_DETECTED:

            /* VBUS is detected. Attach the device */
            USB_DEVICE_Attach(usbData.deviceHandle);
            break;

        case USB_DEVICE_EVENT_POWER_REMOVED:

            /* VBUS is removed. Detach the device */
            USB_DEVICE_Detach (usbData.deviceHandle);
            LED1_Off();
            break;

        case USB_DEVICE_EVENT_CONTROL_TRANSFER_SETUP_REQUEST:
            /* This means we have received a setup packet */
            setupPacket = (USB_SETUP_PACKET *)pData;
            if(setupPacket->bRequest == USB_DEVICE_MICROSOFT_OS_DESCRIPTOR_VENDOR_CODE)
            {
                if ((setupPacket->bmRequestType == 0xC0) && (setupPacket->W_Index.Val == 0x0004))
                {
                   uint16_t length = setupPacket->wLength;
                   if (length > sizeof(microsoftExtendedCompatIdDescriptor))
                   {
                       length = sizeof(microsoftExtendedCompatIdDescriptor);
                   }
                   USB_DEVICE_ControlSend(usbData.deviceHandle, (uint8_t*)&microsoftExtendedCompatIdDescriptor, length);                   
                }
                else if((setupPacket->bmRequestType == 0xC1))
                {
                    uint16_t length = setupPacket->wLength;
                    if (length > sizeof(microsoftExtendedFeatureIdDescriptor))
                    {
                        length = sizeof(microsoftExtendedFeatureIdDescriptor);
                    }
                    USB_DEVICE_ControlSend(usbData.deviceHandle, (uint8_t*)&microsoftExtendedFeatureIdDescriptor, length);     
                }
                else
                {
                   USB_DEVICE_ControlStatus(usbData.deviceHandle, USB_DEVICE_CONTROL_STATUS_ERROR);
                }                
            }
            else 
            if(setupPacket->bRequest == USB_REQUEST_SET_INTERFACE)
            {
                /* If we have got the SET_INTERFACE request, we just acknowledge
                 for now. This demo has only one alternate setting which is already
                 active. */
                USB_DEVICE_ControlStatus(usbData.deviceHandle, USB_DEVICE_CONTROL_STATUS_OK);
            }
            else if(setupPacket->bRequest == USB_REQUEST_GET_INTERFACE)
            {
                /* We have only one alternate setting and this setting 0. So
                 * we send this information to the host. */

                USB_DEVICE_ControlSend(usbData.deviceHandle, &usbData.altSetting, 1);
            }
            else
            {
                /* We have received a request that we cannot handle. Stall it*/
                USB_DEVICE_ControlStatus(usbData.deviceHandle, USB_DEVICE_CONTROL_STATUS_ERROR);
            }
            break;                       

        case USB_DEVICE_EVENT_ENDPOINT_READ_COMPLETE:
        {
           /* Endpoint read is complete */
			USB_DEVICE_EVENT_DATA_ENDPOINT_READ_COMPLETE *eventData = pData;
			if(eventData->status == USB_DEVICE_RESULT_OK)
			{
//				SYS_CONSOLE_PRINT("USB Rx %d\r\n", eventData->length);
				usbData.epDataReadPending = eventData->length;
			}
        }    
        break;

        case USB_DEVICE_EVENT_ENDPOINT_WRITE_COMPLETE:
        {
            /* Endpoint write is complete */
			USB_DEVICE_EVENT_DATA_ENDPOINT_WRITE_COMPLETE *eventData = pData;
			if(eventData->status == USB_DEVICE_RESULT_OK)
			{
                //SYS_CONSOLE_PRINT("USB Rx %d %d\r\n", eventData->length, usbData.epDataWritePending);
                if(usbData.epDataWritePending >= eventData->length)
                    usbData.epDataWritePending -= eventData->length;				
                else
                    usbData.epDataWritePending = 0;				
				
			}            
		}
        break;
        
        /* These events are not used in this demo. */
        case USB_DEVICE_EVENT_RESUMED:
            if(usbData.deviceIsConfigured == true)
            {
                LED1_On();
            }
        break;
			
        case USB_DEVICE_EVENT_ERROR:
        default:
            break;
    }
}


// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


size_t USB_Read(uint8_t* pRdBuffer, const size_t size)
{
    size_t nBytesRead = 0;
    uint32_t rdOutIndex = 0;
    uint32_t rdInIndex = 0;
//    uint32_t rdOut16Idx;
//    uint32_t nBytesRead16Idx;

    /* Take a snapshot of indices to avoid creation of critical section */
    rdOutIndex = usbBufferObj.rdOutIndex;
    rdInIndex = usbBufferObj.rdInIndex;

    while (nBytesRead < size)
    {
        if (rdOutIndex != rdInIndex)
        {
            
            pRdBuffer[nBytesRead] = usbRxDataCircuBuff[rdOutIndex];

            nBytesRead++;
            rdOutIndex++;

            if (rdOutIndex >= usbBufferObj.rdBufferSize)
            {
                rdOutIndex = 0U;
            }
        }
        else
        {
            /* No more data available in the RX buffer */
            break;
        }
    }

    usbBufferObj.rdOutIndex = rdOutIndex;

    return nBytesRead;
}

size_t USB_ReadCountGet(void)
{
    size_t nUnreadBytesAvailable;
    uint32_t rdInIndex;
    uint32_t rdOutIndex;

    /* Take a snapshot of indices to avoid processing in critical section */
    rdInIndex = usbBufferObj.rdInIndex;
    rdOutIndex = usbBufferObj.rdOutIndex;

    if ( rdInIndex >=  rdOutIndex)
    {
        nUnreadBytesAvailable =  rdInIndex -  rdOutIndex;
    }
    else
    {
        nUnreadBytesAvailable =  (usbBufferObj.rdBufferSize -  rdOutIndex) + rdInIndex;
    }

    return nUnreadBytesAvailable;
}

size_t USB_ReadFreeBufferCountGet(void)
{
    return (usbBufferObj.rdBufferSize - 1U) - UART2_ReadCountGet();
}



static bool USB_TxPullByte(uint8_t* pWrByte)
{
    bool isSuccess = false;
    uint32_t wrOutIndex = usbBufferObj.wrOutIndex;
    uint32_t wrInIndex = usbBufferObj.wrInIndex;
//    uint32_t wrOut16Idx;

    if (wrOutIndex != wrInIndex)
    {
       
		*pWrByte = usbTxDataCircuBuff[wrOutIndex];       
        wrOutIndex++;

        if (wrOutIndex >= usbBufferObj.wrBufferSize)
        {
            wrOutIndex = 0U;
        }

        usbBufferObj.wrOutIndex = wrOutIndex;

        isSuccess = true;
    }

    return isSuccess;
}

static inline bool USB_TxPushByte(uint8_t wrByte)
{
    uint32_t tempInIndex;
    bool isSuccess = false;
    uint32_t wrOutIndex = usbBufferObj.wrOutIndex;
    uint32_t wrInIndex = usbBufferObj.wrInIndex;
//    uint32_t wrIn16Idx;

    tempInIndex = wrInIndex + 1U;

    if (tempInIndex >= usbBufferObj.wrBufferSize)
    {
        tempInIndex = 0U;
    }
    if (tempInIndex != wrOutIndex)
    {

		usbTxDataCircuBuff[wrInIndex] = (uint8_t)wrByte;
        
        usbBufferObj.wrInIndex = tempInIndex;

        isSuccess = true;
    }
    else
    {
        /* Queue is full. Report Error. */
    }

    return isSuccess;
}


/* This routine is only called from ISR. Hence do not disable/enable USART interrupts. */
static inline bool USB_RxPushByte(uint16_t rdByte)
{
    uint32_t tempInIndex;
    bool isSuccess = false;
//    uint32_t rdInIdx;

    tempInIndex = usbBufferObj.rdInIndex + 1U;

    if (tempInIndex >= usbBufferObj.rdBufferSize)
    {
        tempInIndex = 0U;
    }

    if (tempInIndex == usbBufferObj.rdOutIndex)
    {
        /* Queue is full - Report it to the application. Application gets a chance to free up space by reading data out from the RX ring buffer */
        if(usbBufferObj.rdCallback != NULL)
        {
            uintptr_t rdContext = usbBufferObj.rdContext;

            usbBufferObj.rdCallback(UART_EVENT_READ_BUFFER_FULL, rdContext);

            /* Read the indices again in case application has freed up space in RX ring buffer */
            tempInIndex = usbBufferObj.rdInIndex + 1U;

            if (tempInIndex >= usbBufferObj.rdBufferSize)
            {
                tempInIndex = 0U;
            }
        }
    }

    /* Attempt to push the data into the ring buffer */
    if (tempInIndex != usbBufferObj.rdOutIndex)
    {
        uint32_t rdInIndex = usbBufferObj.rdInIndex;


        usbRxDataCircuBuff[rdInIndex] = (uint8_t)rdByte;

        usbBufferObj.rdInIndex = tempInIndex;

        isSuccess = true;
    }
    else
    {
        /* Queue is full. Data will be lost. */
    }

    return isSuccess;
}


// Get single data API
char GetUSBCharacter(const void* cmdIoParam)
{
	uint8_t pRdByte;
	USB_Read(&pRdByte, 1);

	return pRdByte;
}

// Data available API
int IsUSBReady(const void* cmdIoParam)
{


	return USB_ReadCountGet();

}

// Put single char function API
void SendUSBCharacter(const void* cmdIoParam, char c)
{
	USB_TxPushByte(c);
}

// Print function API
void SendUSBPrint(const void* cmdIoParam, const char* format, ...)
{    
//	static int printBuffPtr = 0;
    char tmpBuf[SYS_CMD_PRINT_BUFFER_SIZE];
    size_t len = 0;
//    size_t padding = 0;
    va_list args;
    va_start( args, format );

    len = (uint32_t)vsnprintf(tmpBuf, SYS_CMD_PRINT_BUFFER_SIZE, format, args);

    va_end( args );

	
	for(int printBuffPtr = 0; printBuffPtr < len; printBuffPtr++)		
		USB_TxPushByte(tmpBuf[printBuffPtr]);

}



void SendUSBMessage(const void* cmdIoParam, const char* str)
{
	for(int i = 0; i < strlen(str); i++)	
		USB_TxPushByte(str[i]);

}


static const SYS_CMD_API sysUSBVendorApi =
{
    .msg = SendUSBMessage,
    .print = SendUSBPrint,
    .putc_t = SendUSBCharacter,
    .isRdy = IsUSBReady,
    .getc_t = GetUSBCharacter,
};


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void USB_Initialize ( void )

  Remarks:
    See prototype in usb.h.
 */

void USB_Initialize ( void )
{
    extern USB_STR_DESCR_t sd003_run;
    const char hexMap[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    /* Place the App state machine in its initial state. */
    usbData.state = USB_STATE_INIT;
    usbData.deviceHandle = USB_DEVICE_HANDLE_INVALID;
    usbData.deviceIsConfigured = false;
    
    if(!usbData.mode)
    {
        usbData.endpointRx = (APP_EP_BULK_OUT | USB_EP_DIRECTION_OUT);
        usbData.endpointTx = (APP_EP_BULK_IN | USB_EP_DIRECTION_IN);
        usbData.endpointIntRx = (APP_EP_INT_OUT | USB_EP_DIRECTION_OUT);
        usbData.endpointIntTx = (APP_EP_INT_IN | USB_EP_DIRECTION_IN);    
        usbData.epDataReadPending = 0;
        usbData.epDataWritePending = 0;
        usbData.altSetting = 0;        
        
        
        /* Initialize instance object */
        usbBufferObj.rdCallback = NULL;
        usbBufferObj.rdInIndex = 0;
        usbBufferObj.rdOutIndex = 0;
        usbBufferObj.isRdNotificationEnabled = false;
        usbBufferObj.isRdNotifyPersistently = false;
        usbBufferObj.rdThreshold = 0;

        usbBufferObj.wrCallback = NULL;
        usbBufferObj.wrInIndex = 0;
        usbBufferObj.wrOutIndex = 0;
        usbBufferObj.isWrNotificationEnabled = false;
        usbBufferObj.isWrNotifyPersistently = false;
        usbBufferObj.wrThreshold = 0;

        usbBufferObj.errors = UART_ERROR_NONE;

        usbBufferObj.rdBufferSize = USB_READ_BUFFER_SIZE;
        usbBufferObj.wrBufferSize = USB_WRITE_BUFFER_SIZE;


    }
    
    sd003_run.string[4] = hexMap[(DEVSN0 >> 12) & 0xF];
    sd003_run.string[5] = hexMap[(DEVSN0 >> 8) & 0xF];
    sd003_run.string[6] = hexMap[(DEVSN0 >> 4) & 0xF];
    sd003_run.string[7] = hexMap[DEVSN0 & 0xF];

    sd003_run.string[8] = hexMap[(DEVSN1 >> 12) & 0xF];
    sd003_run.string[9] = hexMap[(DEVSN1 >> 8) & 0xF];
    sd003_run.string[10] = hexMap[(DEVSN1 >> 4) & 0xF];
    sd003_run.string[11] = hexMap[DEVSN1 & 0xF];


	
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void USB_Tasks ( void )

  Remarks:
    See prototype in usb.h.
 */

void USB_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( usbData.state )
    {
        /* Application's initial state. */
        case USB_STATE_INIT:
        {            

            /* Open the device layer */
            usbData.deviceHandle = USB_DEVICE_Open( USB_DEVICE_INDEX_0,DRV_IO_INTENT_READWRITE );

            if(usbData.deviceHandle != USB_DEVICE_HANDLE_INVALID)
            {
                
                if(usbData.mode) {
                    /* Register a callback with device layer to get event notification */
                    USB_DEVICE_EventHandlerSet(usbData.deviceHandle, USB_MSD_DeviceEventHandler, (uintptr_t)&usbData);
                
                    /* The MSD Device is maintained completely by the MSD function
                    * driver and does not require application intervention. So there
                    * is nothing related to MSD Device to do here. */
                    usbData.state = USB_STATE_SERVICE_TASKS;
                    LED1_On();
                }
                else {
                    USB_DEVICE_EventHandlerSet(usbData.deviceHandle, USB_Test_DeviceEventCallBack, (uintptr_t)&usbData);
                    /* Move to test mode, waiting for the console bring */
                    usbData.state = USB_STATE_WAIT_FOR_CONSOLE;
                }
                    
            }
            else
            {
                /* The Device Layer is not ready to be opened. We should try
                 * again later. */
            }
                       
            break;
        }
        
        case USB_STATE_WAIT_FOR_CONSOLE:
        {
            if(SYS_CMDIO_GET_HANDLE(0) != NULL)
			{
				if(SYS_CMDIO_ADD(&sysUSBVendorApi, &usbCmdInitData.consoleCmdIOParam, (int32_t)usbCmdInitData.consoleCmdIOParam) == NULL)
			    {
			        SYS_ERROR_PRINT(SYS_ERROR_WARNING, "Failed to create the USB Console API\r\n");
			    }
				else
				{
//			        SYS_CONSOLE_MESSAGE("Added USB Console CLI interface\r\n");		
                    usbData.state = USB_STATE_WAIT_FOR_CONFIGURATION;
				}	
			}			
        }
        break;
        case USB_STATE_WAIT_FOR_CONFIGURATION:
            /* Check if the device is configured */			
			
            if(usbData.deviceIsConfigured == true)
            {
                if (USB_DEVICE_ActiveSpeedGet(usbData.deviceHandle) == USB_SPEED_FULL)
                {
                    usbData.endpointMaxPktSize = 64;
                }
                else if (USB_DEVICE_ActiveSpeedGet(usbData.deviceHandle) == USB_SPEED_HIGH)
                {
                    usbData.endpointMaxPktSize = 512;
                }
                if (USB_DEVICE_EndpointIsEnabled(usbData.deviceHandle, usbData.endpointRx) == false )
                {
                    /* Enable Read Endpoint */
                    USB_DEVICE_EndpointEnable(usbData.deviceHandle, 0, usbData.endpointRx,
                            USB_TRANSFER_TYPE_BULK, usbData.endpointMaxPktSize);
                }
                if (USB_DEVICE_EndpointIsEnabled(usbData.deviceHandle, usbData.endpointTx) == false )
                {
                    /* Enable Write Endpoint */
                    USB_DEVICE_EndpointEnable(usbData.deviceHandle, 0, usbData.endpointTx,
                            USB_TRANSFER_TYPE_BULK, usbData.endpointMaxPktSize);
                }

				if (USB_DEVICE_EndpointIsEnabled(usbData.deviceHandle, usbData.endpointIntRx) == false )
                {
                    /* Enable Read Endpoint */
                    USB_DEVICE_EndpointEnable(usbData.deviceHandle, 0, usbData.endpointIntRx,
                            USB_TRANSFER_TYPE_INTERRUPT, usbData.endpointMaxPktSize);
                }
                if (USB_DEVICE_EndpointIsEnabled(usbData.deviceHandle, usbData.endpointIntTx) == false )
                {
                    /* Enable Write Endpoint */
                    USB_DEVICE_EndpointEnable(usbData.deviceHandle, 0, usbData.endpointIntTx,
                            USB_TRANSFER_TYPE_INTERRUPT, usbData.endpointMaxPktSize);
                }
				
                /* Indicate that we are waiting for read */
                usbData.epDataReadPending = 0;

                /* Place a new read request. */
				// TBD set the receive data length to buffer free space
				
                USB_DEVICE_EndpointRead(usbData.deviceHandle, &usbData.readTranferHandle,
                        usbData.endpointRx, &receivedDataBuffer, USB_READ_BUFFER_SIZE);

                /* Device is ready to run the main task */
                usbData.state = USB_STATE_MAIN_TASK;
            }
                
            break;
            
        case USB_STATE_MAIN_TASK:
            if(!usbData.deviceIsConfigured)
            {
                /* This means the device got deconfigured. Change the
                 * application state back to waiting for configuration. */
                usbData.state = USB_STATE_WAIT_FOR_CONFIGURATION;

                /* Disable the endpoint*/
                USB_DEVICE_EndpointDisable(usbData.deviceHandle, usbData.endpointRx);
                USB_DEVICE_EndpointDisable(usbData.deviceHandle, usbData.endpointTx);
                USB_DEVICE_EndpointDisable(usbData.deviceHandle, usbData.endpointIntRx);
                USB_DEVICE_EndpointDisable(usbData.deviceHandle, usbData.endpointIntTx);
                usbData.epDataReadPending = 0;
                usbData.epDataWritePending = 0;
            }
            else
            {
                if (usbData.epDataReadPending)
                {
                    /* Look at the data the host sent, to see what kind of
                     * application specific command it sent. */

                    for(int i = 0; i < usbData.epDataReadPending; i++)
                        USB_RxPushByte(receivedDataBuffer[i]);

                    usbData.epDataReadPending = 0 ;
                    
                    /* Place a new read request. */
                    USB_DEVICE_EndpointRead ( usbData.deviceHandle, &usbData.readTranferHandle,
                            usbData.endpointRx, &receivedDataBuffer[0], sizeof(receivedDataBuffer) );

                }
                /* This is a switch check command. Check if the TX is free
                     * for us to send a status. */

                if(!usbData.epDataWritePending)
                {
                    for(usbData.epDataWritePending = 0; usbData.epDataWritePending < USB_WRITE_BUFFER_SIZE; usbData.epDataWritePending++)				
                    {
                        if(USB_TxPullByte(&transmitDataBuffer[usbData.epDataWritePending]) == false)
                            break;						
                    }

                    if(usbData.epDataWritePending)
                    {
                        /* Send the data to the host */
                        USB_DEVICE_EndpointWrite ( usbData.deviceHandle, &usbData.writeTranferHandle,
                                usbData.endpointTx, &transmitDataBuffer[0],
                                usbData.epDataWritePending,
                                USB_DEVICE_TRANSFER_FLAGS_DATA_COMPLETE);

                    }
                }               
            }
            
            break;

        case USB_STATE_SERVICE_TASKS:
        {

            break;
        }

        /* TODO: implement your application state machine.*/


        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */