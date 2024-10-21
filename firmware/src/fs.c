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
 #define NO_DELAY 1
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


static volatile int32_t *tmrCxt[2];


#define TMOD_TAP_DR_HDR    7
#define TMOD_TAP_IR_HDR    9
//
// Only TMS
//static uint8_t TAP_RST_5BIT[2][12] = {
//                    {SET, SET, SET, SET, SET, SET, SET, SET, SET, SET, CLR, CLR}, // TMS                    
//};

// TMS and TDO
//0x5B98207C
#define TMOD12_PATTERN_LEN  64
static const uint32_t TMOD12_PATTERN[TMOD12_PATTERN_LEN] = {/*0x7C*/CLR, CLR, CLR, CLR, CLR, SET, SET, SET, SET, SET, SET, SET, SET, SET, SET, CLR, CLR, /*20*/ CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, SET, SET, CLR, CLR, CLR, CLR,
        /*0x98*/CLR, CLR, CLR, CLR, CLR, CLR, SET, SET, SET, SET, CLR, CLR, CLR, CLR, SET, SET, /*5B*/ SET, SET, SET, SET, CLR, CLR, SET, SET, SET, SET, CLR, CLR, SET, SET, CLR};

#define TAP_RESET_CMD_LEN 7
static const uint32_t TAP_RESET[2][TAP_RESET_CMD_LEN] = {
                {CLR, SET, SET, SET, SET, SET, SET},
                {CLR, SET, SET, SET, SET, SET, SET}
                }; 


#define IR_5BIT_CMD_LEN  26
#define IR_5BIT_CMD_IDX  IR_5BIT_CMD_LEN - TMOD_TAP_IR_HDR - 1

static uint32_t CACHE_ALIGN IR_5BIT_STREAM[2][IR_5BIT_CMD_LEN] = {
                    {CLR, SET, SET, SET, SET, CLR, CLR, SET, SET, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, SET, SET, SET, SET, CLR}, // TMS
                    {CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR}  // TDO
                    };


#define DR_8BIT_CMD_LEN  30
#define DR_8BIT_CMD_IDX  DR_8BIT_CMD_LEN - TMOD_TAP_DR_HDR -1

static uint32_t CACHE_ALIGN DR_8BIT_STREAM[3][DR_8BIT_CMD_LEN] = {                    
                    {CLR, SET, SET, SET, SET, CLR, CLR, SET, SET, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, SET, SET, CLR}, // TMS
                    {CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR},  // TDO
                    {CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR}  // TDI
                    };


#define DR_32BIT_CMD_LEN  78
#define DR_32BIT_CMD_IDX  DR_32BIT_CMD_LEN - TMOD_TAP_DR_HDR - 1
static uint32_t CACHE_ALIGN DR_32BIT_STREAM[3][DR_32BIT_CMD_LEN] = {                    
                    {CLR, SET, SET, SET, SET, CLR, CLR, SET, SET, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, SET, SET, CLR}, // TMS
                    {CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR},  // TDO
                    {CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR}  // TDI
                    };

/*MSB First */

#define ICD_REG_LEN     (158 + 20)// dummy 20 bits


#define ICD_REG_HDR_LEN      (20 + TMOD_TAP_DR_HDR)


#define ICD_ADDR_MSB_IDX     (ICD_REG_LEN - ICD_REG_HDR_LEN)
#define ICD_ADDR_LSB_IDX     (ICD_ADDR_MSB_IDX - (60 + 1))

#define ICD_DATA_MSB_IDX     (ICD_ADDR_MSB_IDX - 60)
#define ICD_DATA_LSB_IDX     (ICD_DATA_MSB_IDX - (64))

static uint32_t CACHE_ALIGN ICD_REG_STREAM[3][ICD_REG_LEN] = {                    
                    {CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, SET, SET, SET, SET, CLR, CLR, SET, SET, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, SET, SET, CLR}, // TMS 
                    {CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, SET, SET, SET, SET, SET, SET, SET, SET, SET, SET, SET, SET, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR}, // TDO
                    };


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
    fsData.triggerTmodFlash = false;
    
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
                /* Go to Idle and wait for Flash trigger. */
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
            sprintf((char *)work, "%s/%s", FS_MOUNT_NAME, fsData.fileName);
            fsData.fileHandle = SYS_FS_FileOpen((const char *)work, (SYS_FS_FILE_OPEN_READ));
            if(fsData.fileHandle == SYS_FS_HANDLE_INVALID)
            {
                /* File open unsuccessful */
                fsData.state = FS_ERROR;
            }
            else
            {
                /* File open was successful. Read the file. */
                fsData.state = FS_READ_FILE_STAT;
            }
            break;
        }
        
        case FS_READ_FILE_STAT:
        {
            sprintf((char *)work, "%s/%s", FS_MOUNT_NAME, fsData.fileName);
            if(SYS_FS_FileStat((const char *)work, &fsData.fileStatus) == SYS_FS_RES_FAILURE)
            {
                /* Reading file status was a failure */
                fsData.state = FS_ERROR;
            }
            else
            {
                SYS_CONSOLE_PRINT("File Size = %d\r\n", fsData.fileStatus.fsize);
                /* Read file size */
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
            
            uint32_t cnt = 0;
            uint32_t remBytes = 0;
            size_t numOfBytes = ((fsData.fileStatus.fsize - fsData.readCount) > BUFFER_SIZE)?BUFFER_SIZE:(fsData.fileStatus.fsize - fsData.readCount);
            
            size_t readSz = SYS_FS_FileRead(fsData.fileHandle, (void *)fsData.readBuffer, numOfBytes*sizeof(uint32_t));
//            SYS_CONSOLE_PRINT("Read Size = %d\r\n", readSz);
            if(readSz < 0)
            {
                /* There was an error while reading the file. Close the file
                 * and error out. */
                SYS_FS_FileClose(fsData.fileHandle);
                fsData.state = FS_ERROR;
            }
            else
            {    
                switch(gUmtCxt.devList[fsData.tapId].devId)
                {
                    case 0x9B8F053:
                    {
                        uint32_t secSz = readSz/sizeof(uint32_t);
                        remBytes = readSz % sizeof(uint32_t);
                        
                        //write first 16 bytes at address 0
                        if(!fsData.readCount)
                        {
                            
                            TMOD_TAP_ICDREG(fsData.tapId, fsData.readCount, fsData.readBuffer[cnt++], ICDREG_OP_WR, false);   
                            TMOD_TAP_ICDREG(fsData.tapId, fsData.readCount+4, fsData.readBuffer[cnt++], ICDREG_OP_WR, false);   
                                                       
                            TMOD_TAP_ICDREG(fsData.tapId, 0x00000, 0, ICDREG_OP_RD, true);
                            TMOD_TAP_ICDREG(fsData.tapId, 0x00004, 0, ICDREG_OP_RD, false);
                        }
                        
                        for(cnt = 0; cnt < secSz; cnt++)
                        {
                            TMOD_TAP_ICDREG(fsData.tapId, fsData.flashAddr+(cnt*4), fsData.readBuffer[cnt], ICDREG_OP_WR, false);                    
                        }                   
                        
                        

                        fsData.readCount += readSz;
                        fsData.flashAddr += readSz;
                        
                        break;
                    }
                }
                        
                                
                if ((fsData.fileStatus.fsize == fsData.readCount))
                {                 
                    remBytes = (sizeof(uint32_t) - remBytes) * 8;
                    if(remBytes)
                    {                           
                        volatile uint32_t word = (fsData.readBuffer[cnt] << remBytes);
                        word = word >> remBytes;
                        TMOD_TAP_ICDREG(fsData.tapId, fsData.flashAddr+(cnt*4), word, ICDREG_OP_WR, false); 
                    }
                    
                    TMOD_TAP_DR(fsData.tapId, MCHP_CMD_ASSERT);
                    TMOD_TAP_DR(fsData.tapId, MCHP_CMD_DEASSERT);
                    
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
                fsData.state = FS_IDLE;
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
            if(fsData.triggerTmodFlash)
            {                
                fsData.triggerTmodFlash = false;                                
                fsData.state = FS_OPEN_FILE;
                
            }

            break;
        }

        case FS_ERROR:
        {
            /* The application comes here when the demo has failed.*/
            break;
        }
        /* TODO: implement your application state machine.*/

        case FS_TMOD_FLASH:
        {
            
            
            
            break;
        }
        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


void Inject_TMOD12(uintptr_t context)
{
    int32_t *devIdx = (int32_t *)((int32_t *)context)[0];
    int32_t *length = (int32_t *)((int32_t *)context)[1];
    
    UMT_DEV_t *tmodDev = &gUmtCxt.devList[*devIdx];
    PIN_MAP_t *pgcPin = tmodDev->pinLink[PIN_PGC];
    PIN_MAP_t *pgdPin = tmodDev->pinLink[PIN_PGD];
               
    
    // each bit is transmitted twice on the clock sizof(uint32_t) * 2
    if(*length)
    {     
        //keep inverting the clock
        *((volatile uint32_t *)((volatile char *)(pgcPin->gpio_reg) + 0x3C)) = pgcPin->gpio_mask;
        (*length)--;                           
        *((volatile uint32_t *)((volatile char *)(pgdPin->gpio_reg) + TMOD12_PATTERN[*length])) =  pgdPin->gpio_mask;                          
    }   
}

void Inject_RESET(uintptr_t context)
{     
    int32_t *devIdx = (int32_t *)((int32_t *)context)[0];
    int32_t *length = (int32_t *)((int32_t *)context)[1];
    
    UMT_DEV_t *tmodDev = &gUmtCxt.devList[*devIdx];
    PIN_MAP_t *tckPin = tmodDev->pinLink[PIN_TCK];
    PIN_MAP_t *tmsPin = tmodDev->pinLink[PIN_TMS];
    PIN_MAP_t *tdoPin = tmodDev->pinLink[PIN_TDO];
    
    if(*length)
    {     
        (*length)--;                                   
        // clock line
        *((volatile uint32_t *)((char *)(tckPin->gpio_reg + 0x3C))) = tckPin->gpio_mask;      
        *((volatile uint32_t *)((char *)(tmsPin->gpio_reg) + TAP_RESET[0][*length])) =  tmsPin->gpio_mask;        
        *((volatile uint32_t *)((char *)(tdoPin->gpio_reg) + TAP_RESET[1][*length])) =  tdoPin->gpio_mask;        
    }   
}


void Inject_IR(uintptr_t context)
{     
    int32_t *devIdx = (int32_t *)((int32_t *)context)[0];
    int32_t *length = (int32_t *)((int32_t *)context)[1];
    
    UMT_DEV_t *tmodDev = &gUmtCxt.devList[*devIdx];
    PIN_MAP_t *tckPin = tmodDev->pinLink[PIN_TCK];
    PIN_MAP_t *tmsPin = tmodDev->pinLink[PIN_TMS];
    PIN_MAP_t *tdoPin = tmodDev->pinLink[PIN_TDO];
                      
     // each bit is transmitted twice on the clock sizof(uint32_t) * 2
    if(*length)
    {     
        (*length)--;                           
        //keep inverting the clock                      
        *((volatile uint32_t *)((volatile char *)tmsPin->gpio_reg + IR_5BIT_STREAM[0][*length])) =  tmsPin->gpio_mask;          
        *((volatile uint32_t *)((volatile char *)tdoPin->gpio_reg + IR_5BIT_STREAM[1][*length])) =  tdoPin->gpio_mask;          
        // clock line
        *((volatile uint32_t *)((volatile char *)tckPin->gpio_reg + 0x2C)) = tckPin->gpio_mask;    
        
    }   
}

void Inject_8BIT_DR(uintptr_t context)
{               
    int32_t *devIdx = (int32_t *)((int32_t *)context)[0];
    int32_t *length = (int32_t *)((int32_t *)context)[1];   

    UMT_DEV_t *tmodDev = &gUmtCxt.devList[*devIdx];
    PIN_MAP_t *tckPin = tmodDev->pinLink[PIN_TCK];
    PIN_MAP_t *tmsPin = tmodDev->pinLink[PIN_TMS];
    PIN_MAP_t *tdoPin = tmodDev->pinLink[PIN_TDO];
    PIN_MAP_t *tdiPin = tmodDev->pinLink[PIN_TDI];
                          
    if(*length)
    {     
        (*length)--;                           
        *((volatile uint32_t *)((volatile char *)(tmsPin->gpio_reg) + DR_8BIT_STREAM[0][*length])) =  tmsPin->gpio_mask;          
        *((volatile uint32_t *)((volatile char *)(tdoPin->gpio_reg) + DR_8BIT_STREAM[1][*length])) =  tdoPin->gpio_mask;                          
        // clock line, keep inverting the clock                      
        *((volatile uint32_t *)((volatile char *)(tckPin->gpio_reg + 0x3C))) = tckPin->gpio_mask;    
        DR_8BIT_STREAM[2][*length] = *((volatile uint32_t *)((volatile char *)(tdiPin->gpio_reg) + 0x20)) & tdiPin->gpio_mask;
        
    }   
          
}

void Inject_32BIT_DR(uintptr_t context)
{               
    int32_t *devIdx = (int32_t *)((int32_t *)context)[0];
    int32_t *length = (int32_t *)((int32_t *)context)[1];    

    UMT_DEV_t *tmodDev = &gUmtCxt.devList[*devIdx];
    PIN_MAP_t *tckPin = tmodDev->pinLink[PIN_TCK];
    PIN_MAP_t *tmsPin = tmodDev->pinLink[PIN_TMS];
    PIN_MAP_t *tdoPin = tmodDev->pinLink[PIN_TDO];
    PIN_MAP_t *tdiPin = tmodDev->pinLink[PIN_TDI];
                  
    
    if(*length)
    {     
        (*length)--;
        *((volatile uint32_t *)((volatile char *)(tmsPin->gpio_reg) + DR_32BIT_STREAM[0][*length])) =  tmsPin->gpio_mask;          
        *((volatile uint32_t *)((volatile char *)(tdoPin->gpio_reg) + DR_32BIT_STREAM[1][*length])) =  tdoPin->gpio_mask;                          
        // clock line, keep inverting the clock                      
        *((volatile uint32_t *)((volatile char *)(tckPin->gpio_reg) + 0x3C)) = tckPin->gpio_mask;        
        DR_32BIT_STREAM[2][*length] = *((volatile uint32_t *)((volatile char *)(tdiPin->gpio_reg) + 0x20)) & tdiPin->gpio_mask;
        
    }   
          
}

void Inject_ICD_REG(uintptr_t context)
{               
    int32_t *devIdx = (int32_t *)((int32_t *)context)[0];
    int32_t *length = (int32_t *)((int32_t *)context)[1];    

//    UMT_DEV_t *tmodDev = &gUmtCxt.devList[*devIdx];
//    PIN_MAP_t *tckPin = tmodDev->pinLink[PIN_TCK];
//    PIN_MAP_t *tmsPin = tmodDev->pinLink[PIN_TMS];
//    PIN_MAP_t *tdoPin = tmodDev->pinLink[PIN_TDO];
//    PIN_MAP_t *tdiPin = tmodDev->pinLink[PIN_TDI];
                  
    // clock line, keep inverting the clock                      
        
    if(*length )
    {
        (*length)--;               
        *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[*devIdx].pinLink[PIN_TMS]->gpio_reg) + ICD_REG_STREAM[0][*length])) =  gUmtCxt.devList[*devIdx].pinLink[PIN_TMS]->gpio_mask;          
        *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[*devIdx].pinLink[PIN_TDO]->gpio_reg) + ICD_REG_STREAM[1][*length])) =  gUmtCxt.devList[*devIdx].pinLink[PIN_TDO]->gpio_mask;                              
        *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[*devIdx].pinLink[PIN_TCK]->gpio_reg) + 0x2C)) = gUmtCxt.devList[*devIdx].pinLink[PIN_TCK]->gpio_mask;
        //        ICD_REG_STREAM[2][*length] = *((volatile uint32_t *)((volatile char *)(tdiPin->gpio_reg) + 0x20)) & tdiPin->gpio_mask;
    }   
          
}

uint32_t TMOD_TAP_ICDREG(uint32_t devId, uint32_t addr, uint32_t data, ICDREG_OP_t operation, bool reset)
{   
    volatile int32_t length = ICD_REG_LEN;                
#if NO_DELAY

#else    
    int32_t devIdx = devId;
    tmrCxt[0] = &devIdx;
    tmrCxt[1] = &length;           
    SYS_TIME_HANDLE triggerTmr = 0;
#endif    
    uint32_t idx = 0 ; // 

    if(gUmtCxt.devList[devId].devType != UMT_DEV_TMOD)
        return -1;
        
   
    idx = ICD_DATA_LSB_IDX;
    if(operation == ICDREG_OP_WR)
    {
        while(data)
        {
            ICD_REG_STREAM[1][idx++] = (data & 1)?(SET):(CLR);    
            ICD_REG_STREAM[1][idx++] = (data & 1)?(SET):(CLR);
            data = data >> 1; // bit 0
        }
    }
    while(idx < ICD_DATA_MSB_IDX)
    {
        ICD_REG_STREAM[1][idx++] = CLR; 
    }
    
    idx = ICD_ADDR_LSB_IDX +1;
    addr = addr >> 2;
    while(addr)
    {
        ICD_REG_STREAM[1][idx++] = (addr & 1)?(SET):(CLR);    
        ICD_REG_STREAM[1][idx++] = (addr & 1)?(SET):(CLR);
        addr = addr >> 1; // bit 0
    }
    
    while(idx < ICD_ADDR_MSB_IDX)
    {
        ICD_REG_STREAM[1][idx++] = CLR; 
    } 
    
    if(operation == ICDREG_OP_RD)
    {
        ICD_REG_STREAM[1][ICD_REG_LEN-(TMOD_TAP_DR_HDR + 11)] = CLR;    
        ICD_REG_STREAM[1][ICD_REG_LEN-(TMOD_TAP_DR_HDR + 12)] = CLR;                
    }
    else
    {
        ICD_REG_STREAM[1][ICD_REG_LEN-(TMOD_TAP_DR_HDR + 11)] = SET;    
        ICD_REG_STREAM[1][ICD_REG_LEN-(TMOD_TAP_DR_HDR + 12)] = SET;                
    }
    
    if(reset)
    {
        
        ICD_REG_STREAM[1][ICD_REG_LEN-(TMOD_TAP_DR_HDR + 9)] = CLR;    
        ICD_REG_STREAM[1][ICD_REG_LEN-(TMOD_TAP_DR_HDR + 10)] = CLR;  
        ICD_REG_STREAM[1][ICD_REG_LEN-(TMOD_TAP_DR_HDR + 7)] = SET;    
        ICD_REG_STREAM[1][ICD_REG_LEN-(TMOD_TAP_DR_HDR + 8)] = SET;  
    }
    else
    {
        ICD_REG_STREAM[1][ICD_REG_LEN-(TMOD_TAP_DR_HDR + 9)] = SET;    
        ICD_REG_STREAM[1][ICD_REG_LEN-(TMOD_TAP_DR_HDR + 10)] = SET;  
        ICD_REG_STREAM[1][ICD_REG_LEN-(TMOD_TAP_DR_HDR + 7)] = CLR;    
        ICD_REG_STREAM[1][ICD_REG_LEN-(TMOD_TAP_DR_HDR + 8)] = CLR; 
    }
    
#ifdef NO_DELAY    
    while(length)
    {
        length--;     
        
        *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devId].pinLink[PIN_TMS]->gpio_reg) + ICD_REG_STREAM[0][length])) =  gUmtCxt.devList[devId].pinLink[PIN_TMS]->gpio_mask;          
        *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devId].pinLink[PIN_TDO]->gpio_reg) + ICD_REG_STREAM[1][length])) =  gUmtCxt.devList[devId].pinLink[PIN_TDO]->gpio_mask;                          
        *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devId].pinLink[PIN_TCK]->gpio_reg) + 0x3C)) = gUmtCxt.devList[devId].pinLink[PIN_TCK]->gpio_mask;
        ICD_REG_STREAM[2][length] = *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devId].pinLink[PIN_TDI]->gpio_reg) + 0x20)) & gUmtCxt.devList[devId].pinLink[PIN_TDI]->gpio_mask;
        
    }
    
//    for(int i = 0; i < 20; i ++)
//        *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devId].pinLink[PIN_TCK]->gpio_reg) + 0x2C)) = gUmtCxt.devList[devId].pinLink[PIN_TCK]->gpio_mask;
//    
#else    
    
//    TMR2_CallbackRegister(Inject_ICD_REG, (uintptr_t)tmrCxt);
//    TMR2_Start();
    
    
    triggerTmr = SYS_TIME_CallbackRegisterUS(Inject_ICD_REG, (uintptr_t)tmrCxt, 100, SYS_TIME_PERIODIC);
    //Wait until we send complete pattern
    while(length > 0);
                         
//    *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devIdx].pinLink[PIN_TCK]->gpio_reg) + SET)) = gUmtCxt.devList[devIdx].pinLink[PIN_TCK]->gpio_mask;
        

    SYS_TIME_TimerDestroy(triggerTmr);
//    TMR2_Stop();
#endif
    
    return 0;
    
}


uint32_t TMOD_TAP_DR(uint32_t devId, uint32_t dReg)
{    
    int32_t devIdx = devId;
    volatile int32_t length = DR_32BIT_CMD_LEN;                
    tmrCxt[0] = &devIdx;
    tmrCxt[1] = &length;           
//    SYS_TIME_HANDLE triggerTmr = 0;
    volatile uint32_t tapReg = 0;
    volatile int i;
    
    if(gUmtCxt.devList[devId].devType != UMT_DEV_TMOD)
        return -1;
    
    if(dReg && dReg < 0x100)
    {
        uint32_t idx = 0;
        while(dReg)
        {
            DR_8BIT_STREAM[1][DR_8BIT_CMD_IDX-idx++] = (dReg & 1)?(SET):(CLR);    
            DR_8BIT_STREAM[1][DR_8BIT_CMD_IDX-idx++] = (dReg & 1)?(SET):(CLR);
            dReg = dReg >> 1; // bit 0
        }
        for(int i = idx; i < 16; i++)
        {
            DR_8BIT_STREAM[1][DR_8BIT_CMD_IDX-i] = CLR; 
        }
        
        length = DR_8BIT_CMD_LEN;

        TMR2_CallbackRegister(Inject_8BIT_DR, (uintptr_t)tmrCxt);
        TMR2_Start();
        
//        triggerTmr = SYS_TIME_CallbackRegisterUS(Inject_8BIT_DR, (uintptr_t)tmrCxt, 1, SYS_TIME_PERIODIC);
    }
    else
    {
        uint32_t idx = 0;
        while(dReg)
        {
            DR_32BIT_STREAM[1][DR_32BIT_CMD_IDX-idx++] = (dReg & 1)?(SET):(CLR);    
            DR_32BIT_STREAM[1][DR_32BIT_CMD_IDX-idx++] = (dReg & 1)?(SET):(CLR);
            dReg = dReg >> 1; // bit 0
        }
        for(int i = idx; i <  64; i++)
        {
            DR_32BIT_STREAM[1][DR_32BIT_CMD_IDX-i] = CLR; 
        }
                
        
        TMR2_CallbackRegister(Inject_32BIT_DR, (uintptr_t)tmrCxt);
        TMR2_Start();
//        triggerTmr = SYS_TIME_CallbackRegisterUS(Inject_32BIT_DR, (uintptr_t)tmrCxt, 2, SYS_TIME_PERIODIC);
    }
    
    //Wait until we send complete pattern
    while(length > 0);
    TMR2_Stop();
    
//    while(length > 0);
//    SYS_TIME_TimerDestroy(triggerTmr);
    
    for(i  = (DR_32BIT_CMD_IDX - 64 + 1); i < DR_32BIT_CMD_IDX; i++)
    {
        tapReg = tapReg << 1;
        tapReg |= (DR_32BIT_STREAM[2][i++] != 0)?1:0;        
    }
    
     
    return tapReg;

}

uint32_t TMOD_TAP_IR(uint32_t devId, uint32_t iReg)
{   
    int32_t devIdx = devId;
    volatile int32_t length = IR_5BIT_CMD_LEN;                
    tmrCxt[0] = &devIdx;
    tmrCxt[1] = &length; 
           
    if(gUmtCxt.devList[devId].devType != UMT_DEV_TMOD)
        return -1;
    
    IR_5BIT_STREAM[1][IR_5BIT_CMD_IDX] = (iReg & 1)?(SET):(CLR);    
    IR_5BIT_STREAM[1][IR_5BIT_CMD_IDX-1] = (iReg & 1)?(SET):(CLR);
    iReg = iReg >> 1; // bit 0
    IR_5BIT_STREAM[1][IR_5BIT_CMD_IDX-2] = (iReg & 1)?(SET):(CLR);    
    IR_5BIT_STREAM[1][IR_5BIT_CMD_IDX-3] = (iReg & 1)?(SET):(CLR);    
    iReg = iReg >> 1; // bit 1
    IR_5BIT_STREAM[1][IR_5BIT_CMD_IDX-4] = (iReg & 1)?(SET):(CLR);    
    IR_5BIT_STREAM[1][IR_5BIT_CMD_IDX-5] = (iReg & 1)?(SET):(CLR);    
    iReg = iReg >> 1; // bit 2
    IR_5BIT_STREAM[1][IR_5BIT_CMD_IDX-6] = (iReg & 1)?(SET):(CLR);    
    IR_5BIT_STREAM[1][IR_5BIT_CMD_IDX-7] = (iReg & 1)?(SET):(CLR);    
    iReg = iReg >> 1; // bit 3
    IR_5BIT_STREAM[1][IR_5BIT_CMD_IDX-8] = (iReg & 1)?(SET):(CLR);    
    IR_5BIT_STREAM[1][IR_5BIT_CMD_IDX-9] = (iReg & 1)?(SET):(CLR);    
    iReg = iReg >> 1; // bit 4
    IR_5BIT_STREAM[1][IR_5BIT_CMD_IDX-10] = (iReg & 1)?(SET):(CLR);    
    IR_5BIT_STREAM[1][IR_5BIT_CMD_IDX-11] = (iReg & 1)?(SET):(CLR);         

    
    TMR2_CallbackRegister(Inject_IR, (uintptr_t)tmrCxt);
    TMR2_Start();
    while(length > 0);
    TMR2_Stop();    
      
//    SYS_TIME_HANDLE triggerTmr = SYS_TIME_CallbackRegisterUS(Inject_IR, (uintptr_t)tmrCxt, 2, SYS_TIME_PERIODIC);
//    //Wait until we send complete pattern
//    while(length > 0);
//    SYS_TIME_TimerDestroy(triggerTmr);
     
    return 0;
}

// Reset the Test mode
int32_t TMOD_TAP_Reset(uint32_t devId)
{
    volatile int32_t length = 6; //TAP_RESET_CMD_LEN;
    volatile int32_t devIdx  = devId;            
    tmrCxt[0] = &devIdx;
    tmrCxt[1] = &length; 
    
    if(gUmtCxt.devList[devId].devType != UMT_DEV_TMOD)
        return -1;   
    
    
    for(int cnt = 0; cnt < length; cnt++)
    {        
        *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devIdx].pinLink[PIN_TMS]->gpio_reg) + SET)) = gUmtCxt.devList[devIdx].pinLink[PIN_TMS]->gpio_mask;
        CORETIMER_DelayUs(1);
        *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devIdx].pinLink[PIN_TCK]->gpio_reg) + 0x3C)) = gUmtCxt.devList[devIdx].pinLink[PIN_TCK]->gpio_mask;                
    }
    *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devIdx].pinLink[PIN_TMS]->gpio_reg) + CLR)) = gUmtCxt.devList[devIdx].pinLink[PIN_TMS]->gpio_mask;
//        
//    TMR2_CallbackRegister(Inject_RESET, (uintptr_t)tmrCxt);
//    TMR2_Start();
//    while(length > 0);
//    TMR2_Stop();
////    
//    SYS_TIME_HANDLE triggerTmr = SYS_TIME_CallbackRegisterUS(Inject_RESET, (uintptr_t)tmrCtxt, 1, SYS_TIME_PERIODIC);
//    //Wait until we send complete pattern
//    while(length > 0);       
//    SYS_TIME_TimerDestroy(triggerTmr);
    
    return 0;
}
int32_t TMOD_Pattern(uint32_t devId)
{
    volatile int32_t devIdx = devId;
    volatile int32_t length = TMOD12_PATTERN_LEN;        
    tmrCxt[0] = &devIdx;
    tmrCxt[1] = &length;
    
    PIN_MAP_t *mclrPin = gUmtCxt.devList[devId].pinLink[PIN_MCLR];
        
    // set the MCLR LOW
    *((volatile uint32_t *)((char *)mclrPin->gpio_reg + CLR)) = mclrPin->gpio_mask;    
    CORETIMER_DelayMs(1);
    
    SYS_TIME_HANDLE handle = SYS_TIME_CallbackRegisterUS(Inject_TMOD12, (uintptr_t)tmrCxt, 10, SYS_TIME_PERIODIC);       
    if (handle != SYS_TIME_HANDLE_INVALID)
    {
        while(length > 0);                
        SYS_TIME_TimerDestroy(handle);
        
    }
    else
    {
        return -1;
    }
//    TMR2_CallbackRegister(Inject_TMOD12, (uintptr_t)tmrCxt);
//    TMR2_Start();
    
//    TMR2_Stop();
    
    // set the MCLR HIGH
//    
    CORETIMER_DelayMs(1);
    *((volatile uint32_t *)((char *)mclrPin->gpio_reg + SET)) = mclrPin->gpio_mask;
    
   
//    SYS_TIME_TimerDestroy(dealyTmr);  
    
    return 0;
       
}
// Triger the TMOD12
int32_t TMOD_TAP_Init(uint32_t devId)
{
    TMOD_Pattern(devId);
    TMOD_TAP_Reset(devId);    
    TMOD_TAP_IR(devId, CHIP_TAP_MCHP_CMD);    
    TMOD_TAP_DR(devId, MCHP_CMD_DEASSERT);           
//    TMOD_TAP_IR(devId, 0x01);        
    TMOD_TAP_Reset(devId);    
    gUmtCxt.devList[devId].devId = TMOD_TAP_DR(devId, DUMMY_READ);                
//    TMOD_TAP_Reset(devId); 
    TMOD_TAP_IR(devId, CHIP_TAP_ICDREG);       
//    TMOD_TAP_Reset(devId); 
//    TMOD_TAP_ICDREG(devId, 0x20000000, 0x00000000, ICDREG_OP_RD);
         
    CORETIMER_DelayMs(10);
    TMOD_TAP_ICDREG(devId, 0x20000000, 0xABCD1234, ICDREG_OP_WR, false);
    CORETIMER_DelayMs(1);
    TMOD_TAP_ICDREG(devId, 0x20000000, 0x00000000, ICDREG_OP_RD, true);
    CORETIMER_DelayMs(1);
    TMOD_TAP_ICDREG(devId, 0x20000000, 0x00000000, ICDREG_OP_RD, true);
    
    TMOD_TAP_ICDREG(devId, 0x20000000, 0x00000000, ICDREG_OP_RD, false);
    CORETIMER_DelayMs(1);

    TMOD_TAP_ICDREG(devId, 0x20000000, 0x00000000, ICDREG_OP_RD, false);
    
    if(!gUmtCxt.devList[devId].devId)
        return -1;    
    
    return 0;
           
    
}

int32_t TMOD_FLASH_Trigger(uint32_t devId, uint32_t sof, char *fileName)
{
    if(gUmtCxt.devList[devId].devType != UMT_DEV_TMOD)
        return -1;   
    
    fsData.tapId = devId; 
    fsData.flashAddr = sof;
    /* copy the file name */
    sprintf(fsData.fileName, "%s", fileName);
    fsData.triggerTmodFlash = true;
    
    return 0;
}
/*******************************************************************************
 End of File
 */
