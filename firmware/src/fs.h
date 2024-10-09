/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    fs.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "FS_Initialize" and "FS_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "FS_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _FS_H
#define _FS_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
    /* Application's state machine's initial state. */
    FS_STATE_INIT=0,
    FS_STATE_MOUNT_WAIT,
    FS_STATE_FORMAT_DISK,
    FS_STATE_BUS_ENABLE,
    FS_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE,
    FS_STATE_WAIT_FOR_DEVICE_ATTACH,
    FS_STATE_SERVICE_TASKS,
            
    FS_USB_INIT,
            
    /* The app opens the file */
    FS_OPEN_FILE,

    /* The app writes data to the file */
    FS_WRITE_TO_FILE,

    /* The app performs a file sync operation. */
    FS_FLUSH_FILE,

    /* The app checks the file status */
    FS_READ_FILE_STAT,

    /* The app checks the file size */
    FS_READ_FILE_SIZE,

    /* The app does a file seek to the end of the file. */
    FS_DO_FILE_SEEK,

    /* The app checks for EOF */
    FS_CHECK_EOF,

    /* The app does another file seek, to move file pointer to the beginning of
     * the file. */
    FS_DO_ANOTHER_FILE_SEEK,

    /* The app reads and verifies the written data. */
    FS_READ_FILE_CONTENT,

    /* The app closes the file. */
    FS_CLOSE_FILE,

    /* The app unmounts the disk. */
    FS_UNMOUNT_DISK,

    /* The app idles */
    FS_IDLE,

    /* An app error has occurred */
    FS_ERROR
    /* TODO: Define states used by the application state machine. */

} FS_STATES;

        
#define FS_MOUNT_NAME          SYS_FS_MEDIA_IDX0_MOUNT_NAME_VOLUME_IDX0
#define FS_DEVICE_NAME         SYS_FS_MEDIA_IDX0_DEVICE_NAME_VOLUME_IDX0
#define FS_FS_TYPE             FAT


#define FS_FILE_NAME               "newfile.txt"

#define BUFFER_SIZE                (4096U)
// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    /* The application's current state */
    FS_STATES state;

    /* SYS_FS File handle */
    SYS_FS_HANDLE fileHandle;

    /* Read Buffer */
    uint8_t readBuffer[BUFFER_SIZE];

    /* Write Buffer*/
    uint8_t writeBuffer[BUFFER_SIZE];

    SYS_FS_FSTAT fileStatus;

    long fileSize;

    bool diskMounted;

    bool diskFormatRequired;
} FS_DATA;


extern FS_DATA fsData;
// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void FS_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    FS_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    FS_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void FS_Initialize ( void );


/*******************************************************************************
  Function:
    void FS_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    FS_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void FS_Tasks( void );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _FS_H */

/*******************************************************************************
 End of File
 */

