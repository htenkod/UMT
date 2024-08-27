/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    test.c

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

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the TEST_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

TEST_DATA testData;

volatile static UART_RING_BUFFER_OBJECT asyncBufferObj;


// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void TEST_Initialize ( void )

  Remarks:
    See prototype in test.h.
 */

void TEST_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    testData.state = TEST_STATE_INIT;


	 /* Initialize instance object */
    asyncBufferObj.rdCallback = NULL;
    asyncBufferObj.rdInIndex = 0;
    asyncBufferObj.rdOutIndex = 0;
    asyncBufferObj.isRdNotificationEnabled = false;
    asyncBufferObj.isRdNotifyPersistently = false;
    asyncBufferObj.rdThreshold = 0;

    asyncBufferObj.wrCallback = NULL;
    asyncBufferObj.wrInIndex = 0;
    asyncBufferObj.wrOutIndex = 0;
    asyncBufferObj.isWrNotificationEnabled = false;
    asyncBufferObj.isWrNotifyPersistently = false;
    asyncBufferObj.wrThreshold = 0;

    asyncBufferObj.errors = UART_ERROR_NONE;
    
    asyncBufferObj.rdBufferSize = USB_READ_BUFFER_SIZE;
    asyncBufferObj.wrBufferSize = USB_WRITE_BUFFER_SIZE;



    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void TEST_Tasks ( void )

  Remarks:
    See prototype in test.h.
 */

void TEST_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( testData.state )
    {
        /* Application's initial state. */
        case TEST_STATE_INIT:
        {
            bool appInitialized = true;


            if (appInitialized)
            {

                testData.state = TEST_STATE_SERVICE_TASKS;
            }
            break;
        }

        case TEST_STATE_SERVICE_TASKS:
        {

            break;
        }

		case TEST_STATE_ASYNC_TASKS:
		{
//			UART1_Read(void * buffer, const size_t size);
		

		}	
			break;

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
