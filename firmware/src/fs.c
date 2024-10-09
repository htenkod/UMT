/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    fs.c

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
    This structure should be initialized by the FS_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

FS_DATA fsData;


/* Work buffer used by littleFS file system during Format */
uint8_t CACHE_ALIGN work[SYS_FS_FAT_MAX_SS];

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

// *****************************************************************************
/* Function:
    void FS_SYSFSEventHandler
    (
        SYS_FS_EVENT event, 
        void *mountName, 
        uintptr_t context
    )

  Summary:
    This function processes File System Events.

  Description:
    This function processes File System Events.

  Remarks:
    None.
*/    

void FS_SYSFSEventHandler(SYS_FS_EVENT event, void *mountName, uintptr_t context)
{
    switch(event)
    {
        case SYS_FS_EVENT_MOUNT:
            
            if(strcmp((const char *)mountName, FS_MOUNT_NAME) == 0)
            {
                fsData.diskMounted = true;
            }

            break;

        case SYS_FS_EVENT_UNMOUNT:
            
            if(0 == strcmp((const char *)mountName, FS_MOUNT_NAME))
            {
                fsData.diskMounted  = false;
            }
            LED1_Off();
            
            break;
        case SYS_FS_EVENT_MOUNT_WITH_NO_FILESYSTEM:
        {
            fsData.diskFormatRequired = true;
        }
        break;
        
        default:
            break;
    }
}
/*******************************************************************************
  Function:
    void FS_Initialize ( void )

  Remarks:
    See prototype in fs.h.
 */

void FS_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    fsData.state = FS_STATE_MOUNT_WAIT;
    
    SYS_FS_EventHandlerSet((void *)FS_SYSFSEventHandler, (uintptr_t)NULL);

    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void FS_Tasks ( void )

  Remarks:
    See prototype in fs.h.
 */

void FS_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( fsData.state )
    {
        /* Application's initial state. */
        case FS_STATE_MOUNT_WAIT:
        {        
            if (fsData.diskFormatRequired == true)
            {
                /* Mount was successful. Format the disk. */
                fsData.state = FS_STATE_FORMAT_DISK;
            }
            else if (fsData.diskMounted == true)
            {
                /* Mount was successful. Open the file. */
                fsData.state = FS_IDLE;
            }
        }
        break;
        
       
            
        case FS_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE:

            
            break;
        case FS_STATE_FORMAT_DISK:
        {
            SYS_FS_FORMAT_PARAM opt = {.fmt = FM_FAT, .au_size = 0, .align = 0, .n_fat = 0, .n_root = 0};                          
            if (SYS_FS_DriveFormat (FS_MOUNT_NAME, &opt, (void *)work, SYS_FS_FAT_MAX_SS) != SYS_FS_RES_SUCCESS)
            {
                /* Format of the disk failed. */
                SYS_CONSOLE_MESSAGE("Format of the disk failed!\r\n");
            }
            else
            {
                /* Format of the disk success. */
                SYS_CONSOLE_MESSAGE("Format of the disk success!\r\n");                
                /* Format succeeded. Open a file. */
                fsData.state = FS_IDLE;
            }
        }
        break;
        
        case FS_STATE_SERVICE_TASKS:
        {

            break;
        }

        case FS_OPEN_FILE:
        {
            fsData.fileHandle = SYS_FS_FileOpen(FS_MOUNT_NAME"/"FS_FILE_NAME, (SYS_FS_FILE_OPEN_WRITE_PLUS));
            if(fsData.fileHandle == SYS_FS_HANDLE_INVALID)
            {
                /* File open unsuccessful */
                fsData.state = FS_ERROR;
            }
            else
            {
                /* File open was successful. Write to the file. */
                fsData.state = FS_WRITE_TO_FILE;
            }
            break;
        }

        case FS_WRITE_TO_FILE:
            {
            if(SYS_FS_FileWrite (fsData.fileHandle, (void *)fsData.writeBuffer, BUFFER_SIZE) == -1)
            {
                /* Failed to write to the file. */
                fsData.state = FS_ERROR;
            }
            else
            {
                /* File write was successful. */
                fsData.state = FS_FLUSH_FILE;
            }
            break;
        }

        case FS_FLUSH_FILE:
        {
            if (SYS_FS_FileSync(fsData.fileHandle) != 0)
            {
                /* Could not flush the contents of the file. Error out. */
                fsData.state = FS_ERROR;
            }
            else
            {
                /* Check the file status */
                fsData.state = FS_READ_FILE_STAT;
            }
            break;
        }

        case FS_READ_FILE_STAT:
        {
            if(SYS_FS_FileStat(FS_MOUNT_NAME"/"FS_FILE_NAME, &fsData.fileStatus) == SYS_FS_RES_FAILURE)
            {
                /* Reading file status was a failure */
                fsData.state = FS_ERROR;
            }
            else
            {
                /* Read file size */
                fsData.state = FS_READ_FILE_SIZE;
            }
            break;
        }

        case FS_READ_FILE_SIZE:
        {
            fsData.fileSize = SYS_FS_FileSize(fsData.fileHandle);
            if(fsData.fileSize == -1)
            {
                /* Reading file size was a failure */
                fsData.state = FS_ERROR;
            }
            else
            {
                if(fsData.fileSize == fsData.fileStatus.fsize)
                {
                    fsData.state = FS_DO_FILE_SEEK;
                }
                else
                {
                    fsData.state = FS_ERROR;
                }
            }
            break;
        }

        case FS_DO_FILE_SEEK:
        {
            if(SYS_FS_FileSeek( fsData.fileHandle, fsData.fileSize, SYS_FS_SEEK_SET ) == -1)
            {
                /* File seek caused an error */
                fsData.state = FS_ERROR;
            }
            else
        {
                /* Check for End of file */
                fsData.state = FS_CHECK_EOF;
            }
            break;
        }

        case FS_CHECK_EOF:
        {
            if(SYS_FS_FileEOF( fsData.fileHandle ) == false )
            {
                /* Either, EOF is not reached or there was an error
                   In any case, for the application, its an error condition */
                fsData.state = FS_ERROR;
            }
            else
            {
                fsData.state = FS_DO_ANOTHER_FILE_SEEK;
            }
            break;
        }

        case FS_DO_ANOTHER_FILE_SEEK:
        {
            /* Move file pointer to begining of file */
            if(SYS_FS_FileSeek( fsData.fileHandle, 0, SYS_FS_SEEK_SET ) == -1)
            {
                /* File seek caused an error */
                fsData.state = FS_ERROR;
            }
            else
            {
                /* Check for original file content */
                fsData.state = FS_READ_FILE_CONTENT;
            }
            break;
        }

        case FS_READ_FILE_CONTENT:
        {
            if(SYS_FS_FileRead(fsData.fileHandle, (void *)fsData.readBuffer, fsData.fileSize) == -1)
            {
                /* There was an error while reading the file. Close the file
                 * and error out. */
                SYS_FS_FileClose(fsData.fileHandle);
                fsData.state = FS_ERROR;
            }
            else
            {
                if ((fsData.fileSize != BUFFER_SIZE) || (memcmp(fsData.readBuffer, fsData.writeBuffer, BUFFER_SIZE) != 0))
                {
                    /* The written and the read data dont match. */
                    fsData.state = FS_ERROR;
                }
                else
                {
                    /* The test was successful. */
                    fsData.state = FS_CLOSE_FILE;
                }
            }
            break;
        }

        case FS_CLOSE_FILE:
        {
            /* Close the file */
            if (SYS_FS_FileClose(fsData.fileHandle) != 0)
            {
                fsData.state = FS_ERROR;
            }
            else
            {
                fsData.state = FS_UNMOUNT_DISK;
            }
            break;
        }

        case FS_UNMOUNT_DISK:
        {
            /* Unmount the disk */
            if (SYS_FS_Unmount(FS_MOUNT_NAME) != 0)
            {
                fsData.state = FS_ERROR;
            }
            else
            {
                fsData.state = FS_IDLE;
            }
            break;
        }

        case FS_IDLE:
        {
            /* The application comes here when the demo has completed
             * successfully. Glow LED. */
            LED1_On();

            break;
        }

        case FS_ERROR:
        {
            /* The application comes here when the demo has failed.*/
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
