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
#include "tmod.h"
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


typedef union byteWord
            {
                uint32_t word;
                struct {
                    uint8_t b0;
                    uint8_t b1;
                    uint8_t b2;
                    uint8_t b3;                                        
                }bytes;                                
            }BYTE_WORD_t;
            

BYTE_WORD_t readBytes;

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
                SYS_CONSOLE_PRINT("File %s not available!\r\n");
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
            if(SYS_FS_FileSeek( fsData.fileHandle, fsData.fwOffset, SYS_FS_SEEK_SET ) == -1)
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
            if(readSz < 0)
            {
                /* There was an error while reading the file. Close the file
                 * and error out. */
                SYS_FS_FileClose(fsData.fileHandle);
                fsData.state = FS_ERROR;
            }
            else
            {                    
                remBytes = readSz % sizeof(uint32_t);                
                
                switch(gUmtCxt.devList[fsData.tapId].devId & 0x0FFFFFFF)
                {
                    case CHIMERA_CHIP_ID:
                    {
                        uint32_t secSz = readSz/sizeof(uint32_t);
                        /* write first 16 bytes at address 0 */
                        if(!fsData.readCount)
                        {
                                                        
                            wReg32(fsData.tapId, 0x440000B0, 0x00000000);
                            wReg32(fsData.tapId, 0x440000B0, 0xAA996655);
                            wReg32(fsData.tapId, 0x440000B0, 0x556699AA);                            
                            wReg32(fsData.tapId, 0x44012400, 0x00700000);
                            rReg32(fsData.tapId, 0x44012400);
                            
                            /* Start sequence */                            
                            wReg32(fsData.tapId, 0x44000600, 0x00000000);
                            wReg32(fsData.tapId, 0x44000600, 0x00000002);
                            wReg32(fsData.tapId, 0x44000600, 0x00004002);
                            /* destination address */
                            wReg32(fsData.tapId, 0x44000630, 0x00000000);
                            /* DATA0 */
                            wReg32(fsData.tapId, 0x44000640, fsData.readBuffer[cnt++]);
                            /* DATA1 */
                            wReg32(fsData.tapId, 0x44000650, fsData.readBuffer[cnt++]);
                            /* DATA2 */
                            wReg32(fsData.tapId, 0x44000660, fsData.readBuffer[cnt++]);
                            /* DATA3 */
                            wReg32(fsData.tapId, 0x44000670, fsData.readBuffer[cnt++]);
                            /* transfer sequence */
                            wReg32(fsData.tapId, 0x44000620, 0x00000000);
                            wReg32(fsData.tapId, 0x44000620, 0xAA996655);
                            wReg32(fsData.tapId, 0x44000620, 0x556699AA);                            
                            wReg32(fsData.tapId, 0x440006F0, 0x80000000);                           
                            wReg32(fsData.tapId, 0x44000620, 0x00000000);
                            wReg32(fsData.tapId, 0x44000620, 0xAA996655);
                            wReg32(fsData.tapId, 0x44000620, 0x556699AA);                           
                            wReg32(fsData.tapId, 0x44000600, 0x0000C002);
                        }
                        
                        for(cnt = 0; cnt < secSz; cnt++)
                        {
                            wReg32(fsData.tapId, fsData.flashAddr+(cnt*4), fsData.readBuffer[cnt]);                    
                        }                   

                        fsData.readCount += readSz;
                        fsData.flashAddr += readSz;   
                        
                        if ((fsData.fileStatus.fsize == fsData.readCount))
                        {                 
                            remBytes = (sizeof(uint32_t) - remBytes) * 8;
                            if(remBytes)
                            {                           
                                volatile uint32_t word = (fsData.readBuffer[cnt] << remBytes);
                                word = word >> remBytes;
                                wReg32(fsData.tapId, fsData.flashAddr+(cnt*4), word); 
                            }

                            PIN_MAP_t *mclrPin = gUmtCxt.devList[fsData.tapId].pinLink[PIN_MCLR];                     
                            *((volatile uint32_t *)((char *)mclrPin->gpio_reg + CLR)) = mclrPin->gpio_mask;    
                            CORETIMER_DelayUs(100);
                            *((volatile uint32_t *)((char *)mclrPin->gpio_reg + SET)) = mclrPin->gpio_mask;    

                            /* The test was successful. */
                            fsData.state = FS_CLOSE_FILE;
                        }
                        
                        break; /*CHIMERA_CHIP_ID*/
                    }
                    
                    case RIO0_CHIP_ID:
                    {

                        if (fsData.sramLoad)
                        {                 
                            SYS_CONSOLE_PRINT("SRAM Load %X\r\n", fsData.flashAddr);
                            uint32_t secSz = readSz/sizeof(uint32_t);
                            for(cnt = 0; cnt < secSz; cnt++)
                            {
                                wReg32(fsData.tapId, fsData.flashAddr+(cnt*4), fsData.readBuffer[cnt]);                                
                            }              
                            fsData.flashAddr += readSz; 
                        }
                        else
                        {
                            if(!fsData.readCount)
                            {
                                SYS_CONSOLE_PRINT("Firmware Image Size %d\r\n", fsData.readBuffer[RIO0_FW_IMG_SIZE_OFFSET]);
                                SYS_CONSOLE_PRINT("Firmware Image Dest Addr 0x%X\r\n", fsData.readBuffer[RIO0_FW_ROM_DST_OFFSET] & 0x00FFFFFF);
                            }
                            
                        
                            uint32_t secSz = readSz/256;
                            for(uint32_t blkNum = 0; blkNum < secSz; blkNum++)
                            {                                                        
                                RIO0_FLASH_PAGE_Write(fsData.tapId, fsData.flashAddr, (uint8_t *)&fsData.readBuffer[blkNum*64]);                             
                                fsData.flashAddr += 256; 
                            }                            
                        }
                        
                        fsData.readCount += readSz;
                        
                        
                        if ((fsData.fileStatus.fsize - fsData.fwOffset) == fsData.readCount)
                        {
                            SYS_CONSOLE_PRINT("Firmware load complete!\r\n");
                            
                            if (fsData.sramLoad)
                            {
                                fsData.flashAddr = 0x200;
                                
//                                EJTAG_Enter(fsData.tapId, false);
                                
//                                EJTAG_OPCODE_WR(fsData.tapId, 0x3C19A000);
//                                EJTAG_OPCODE_WR(fsData.tapId, 0x37390200);
//                                EJTAG_OPCODE_WR(fsData.tapId, 0x0320C008);
//                                EJTAG_OPCODE_WR(fsData.tapId, 0x00000000);
//                                                                                               
//                                EJTAG_OPCODE_WR(fsData.tapId, 0x3C02A000);
//                                EJTAG_OPCODE_WR(fsData.tapId, 0x34420200);
//                                EJTAG_OPCODE_WR(fsData.tapId, 0x4082C000);
//                                EJTAG_OPCODE_WR(fsData.tapId, 0x000000C0);
//                                /* DERET*/
//                                EJTAG_OPCODE_WR(fsData.tapId, 0x4200001F);     
                                 
                                
                            }
                            else
                            {
                               PIN_MAP_t *mclrPin = gUmtCxt.devList[fsData.tapId].pinLink[PIN_MCLR];                     
                                *((volatile uint32_t *)((char *)mclrPin->gpio_reg + CLR)) = mclrPin->gpio_mask;    
                                CORETIMER_DelayUs(100);
                                *((volatile uint32_t *)((char *)mclrPin->gpio_reg + SET)) = mclrPin->gpio_mask;    
                            }
                            /* The test was successful. */
                            fsData.state = FS_CLOSE_FILE;
                        
                        }
                        
                        break;
                    }
                    
                }
                        
                                
                
            }
            break;
        }
		case FS_RIO0_FLASH_ID:
		{

			
            fsData.state = FS_IDLE;

			break;
		}

		
        case FS_CLOSE_FILE:
        {
            
        
            readBytes.word = rReg32(fsData.tapId, fsData.flashAddr);                                
            
//            readBytes.word = EJTAG_Read(fsData.tapId, fsData.flashAddr);
            
            
            extern size_t UART2_Write(uint8_t* pWrBuffer, const size_t size );
            
            
            UART2_Write(&readBytes.bytes.b0, 1);
            UART2_Write(&readBytes.bytes.b1, 1);
            UART2_Write(&readBytes.bytes.b2, 1);
            UART2_Write(&readBytes.bytes.b3, 1);
            
            
            
            
            fsData.flashAddr += 4;  
            
            
            if (fsData.flashAddr - 0xA0000200 == fsData.readCount)
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
                fsData.state = FS_DEVICE_INIT;
            }

            break;
        }
        
        case FS_DEVICE_INIT:
        {
            switch(gUmtCxt.devList[fsData.tapId].devId & 0x0FFFFFFF)
            {
                case CHIMERA_CHIP_ID:
                {
                    static int32_t timeout = 256;                                            

                    /* Erase the CHIP */
                    TMOD_TAP_IR(fsData.tapId, CHIP_TAP_MCHP_CMD);  
                    TMOD_TAP_DR(fsData.tapId, CHIP_TAP_CHIPE_ERASE);                        
                    TMOD_TAP_DR(fsData.tapId, MCHP_CMD_DEASSERT);
                    while((timeout-- > 0) && (TMOD_TAP_DR(fsData.tapId, DUMMY_READ) != 0x88));   

                    /* Enter the ICDREG mode */
                    TMOD_TAP_IR(fsData.tapId, CHIP_TAP_ICDREG);                               
                    /* write first 16 bytes at address 0 */                     
                    break;
                }

                case RIO0_CHIP_ID:
                {                    
                    TMOD_TAP_Reset(fsData.tapId);
                    TMOD_TAP_IR(fsData.tapId, CHIP_TAP_SELECT_CHIP_TAP);
                    TMOD_TAP_Reset(fsData.tapId);
                    TMOD_TAP_IR(fsData.tapId, CHIP_TAP_MCHP_CMD);  
                    TMOD_TAP_IR(fsData.tapId, CHIP_TAP_EJTAG_SELECT);
                    TMOD_TAP_Reset(fsData.tapId);
                    
                    TMOD_TAP_DR(fsData.tapId, CHIP_TAP_ALTRESET); 

                    TMOD_TAP_Reset(fsData.tapId);
                    TMOD_TAP_IR(fsData.tapId, CHIP_TAP_SELECT_CHIP_TAP);
                    TMOD_TAP_Reset(fsData.tapId);
                    TMOD_TAP_IR(fsData.tapId, CHIP_TAP_MCHP_CMD); 
                    
                    TMOD_TAP_DR(fsData.tapId, MCHP_CMD_DEASSERT);
                    
                    TMOD_TAP_Reset(fsData.tapId);
                    TMOD_TAP_IR(fsData.tapId, CHIP_TAP_SELECT_CHIP_TAP);
                    TMOD_TAP_IR(fsData.tapId, CHIP_TAP_ICDREG);   

                    uint32_t chipId = rReg32(fsData.tapId, 0x1F800060);
                    SYS_CONSOLE_PRINT("Chip ID = 0x%X\r\n", chipId);

                    wReg32(fsData.tapId, 0x00001000, 0xABCD1234);
                    if(rReg32(fsData.tapId, 0x00001000) == 0xABCD1234)
                    {
                        SYS_CONSOLE_PRINT("TMOD12 entry success!\r\n");                            
                    }

                    RIO0_SYS_Initialize(fsData.tapId);
                    RIO0_FLASH_Initialize(fsData.tapId);                                                                                
                    RIO0_FLASH_Reset(fsData.tapId);          
                    volatile uint32_t flashId = RIO0_FLASH_ID_Read(fsData.tapId);
                    while(flashId != 0x1C7015)
                        flashId = RIO0_FLASH_ID_Read(fsData.tapId);
                    
                    

                    RIO0_SYS_Initialize(fsData.tapId);
                    RIO0_FLASH_Initialize(fsData.tapId);                         
                    SYS_CONSOLE_PRINT("SPLLCON = 0x%X\r\n", rReg32(fsData.tapId, RIO0_SPLL_CON));
                    SYS_CONSOLE_PRINT("SPI0BRG = 0x%X\r\n", rReg32(fsData.tapId, 0x1F801630));
                    SYS_CONSOLE_PRINT("Flash ID = 0x%X\r\n", flashId); 
                    RIO0_FLASH_CHIP_Erase(fsData.tapId);

                    break;
                }
            }
            fsData.state = FS_OPEN_FILE;

            /* End of Switch */
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


int32_t FS_TMOD_Trigger(uint32_t devId, uint32_t sof, uint32_t offset, bool sramLoad, char *fileName)
{
    if(gUmtCxt.devList[devId].devType != UMT_DEV_TMOD)
        return -1;   
    
    fsData.tapId = devId; 
    fsData.flashAddr = sof;
    fsData.fwOffset = offset;
    fsData.sramLoad = sramLoad;
    /* copy the file name */
    sprintf(fsData.fileName, "%s", fileName);
    fsData.triggerTmodFlash = true;
    
    return 0;
}
/*******************************************************************************
 End of File
 */
