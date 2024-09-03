/*******************************************************************************
 System Tasks File

  File Name:
    tasks.c

  Summary:
    This file contains source code necessary to maintain system's polled tasks.

  Description:
    This file contains source code necessary to maintain system's polled tasks.
    It implements the "SYS_Tasks" function that calls the individual "Tasks"
    functions for all polled MPLAB Harmony modules in the system.

  Remarks:
    This file requires access to the systemObjects global data structure that
    contains the object handles to all MPLAB Harmony module objects executing
    polled in the system.  These handles are passed into the individual module
    "Tasks" functions to identify the instance of the module to maintain.
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
#include "sys_tasks.h"


// *****************************************************************************
// *****************************************************************************
// Section: RTOS "Tasks" Routine
// *****************************************************************************
// *****************************************************************************
TaskHandle_t xSYS_CMD_Tasks;
void lSYS_CMD_Tasks(  void *pvParameters  )
{
    while(1)
    {
        SYS_CMD_Tasks();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}


static void F_USB_DEVICE_Tasks(  void *pvParameters  )
{
    while(true)
    {
                /* USB Device layer tasks routine */
        USB_DEVICE_Tasks(sysObj.usbDevObject0);
        vTaskDelay(10U / portTICK_PERIOD_MS);
    }
}

static void F_DRV_USBHS_Tasks(  void *pvParameters  )
{
    while(true)
    {
                 /* USB FS Driver Task Routine */
        DRV_USBHS_Tasks(sysObj.drvUSBHSObject);
        vTaskDelay(10U / portTICK_PERIOD_MS);
    }
}

/* Handle for the COMMANDS_Tasks. */
TaskHandle_t xCOMMANDS_Tasks;

static void lCOMMANDS_Tasks(  void *pvParameters  )
{   
    while(true)
    {
        COMMANDS_Tasks();
        vTaskDelay(10U / portTICK_PERIOD_MS);
    }
}
/* Handle for the TEST_Tasks. */
TaskHandle_t xTEST_Tasks;

static void lTEST_Tasks(  void *pvParameters  )
{   
    while(true)
    {
        TEST_Tasks();
        vTaskDelay(5U / portTICK_PERIOD_MS);
    }
}
/* Handle for the USB_Tasks. */
TaskHandle_t xUSB_Tasks;

static void lUSB_Tasks(  void *pvParameters  )
{   
    while(true)
    {
        USB_Tasks();
        vTaskDelay(5U / portTICK_PERIOD_MS);
    }
}




// *****************************************************************************
// *****************************************************************************
// Section: System "Tasks" Routine
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void SYS_Tasks ( void )

  Remarks:
    See prototype in system/common/sys_module.h.
*/
void SYS_Tasks ( void )
{
    /* Maintain system services */
    
    (void) xTaskCreate( lSYS_CMD_Tasks,
        "SYS_CMD_TASKS",
        SYS_CMD_RTOS_STACK_SIZE,
        (void*)NULL,
        SYS_CMD_RTOS_TASK_PRIORITY,
        &xSYS_CMD_Tasks
    );





    /* Maintain Device Drivers */
    

    /* Maintain Middleware & Other Libraries */
        /* Create OS Thread for USB_DEVICE_Tasks. */
    (void) xTaskCreate( F_USB_DEVICE_Tasks,
        "USB_DEVICE_TASKS",
        1024,
        (void*)NULL,
        1,
        (TaskHandle_t*)NULL
    );

    /* Create OS Thread for USB Driver Tasks. */
    (void) xTaskCreate( F_DRV_USBHS_Tasks,
        "DRV_USBHS_TASKS",
        1024,
        (void*)NULL,
        1,
        (TaskHandle_t*)NULL
    );



    /* Maintain the application's state machine. */
        /* Create OS Thread for COMMANDS_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lCOMMANDS_Tasks,
                "COMMANDS_Tasks",
                1024,
                NULL,
                1,
                &xCOMMANDS_Tasks);

    /* Create OS Thread for TEST_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lTEST_Tasks,
                "TEST_Tasks",
                256,
                NULL,
                1,
                &xTEST_Tasks);

    /* Create OS Thread for USB_Tasks. */
    (void) xTaskCreate((TaskFunction_t) lUSB_Tasks,
                "USB_Tasks",
                256,
                NULL,
                1,
                &xUSB_Tasks);




    /* Start RTOS Scheduler. */
    
     /**********************************************************************
     * Create all Threads for APP Tasks before starting FreeRTOS Scheduler *
     ***********************************************************************/
    vTaskStartScheduler(); /* This function never returns. */

}

/*******************************************************************************
 End of File
 */
