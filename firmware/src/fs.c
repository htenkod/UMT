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
#include <sys/attribs.h>
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

//#define RIO0_FLASH_PROG

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
                if(fsData.sysCmdDev)
                    (fsData.sysCmdDev->pCmdApi->msg)(fsData.sysCmdDev->cmdIoParam, "File %s not available!\r\n");
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
//                if(fsData.sysCmdDev)
//                    (fsData.sysCmdDev->pCmdApi->print)(fsData.sysCmdDev->cmdIoParam, "File Size = %d\r\n", fsData.fileStatus.fsize);
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
            volatile uint32_t remBytes = 0;
            volatile size_t numOfBytes = ((fsData.fileStatus.fsize - fsData.readCount) > BUFFER_SIZE*sizeof(uint32_t))?BUFFER_SIZE*sizeof(uint32_t):(fsData.fileStatus.fsize - fsData.readCount);
            
            size_t readSz = SYS_FS_FileRead(fsData.fileHandle, (void *)fsData.readBuffer, numOfBytes);
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
                
                switch(gUmtCxt.devList[fsData.tapId].devId & DEV_MASK_ID_MASK)
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
//                            rReg32(fsData.tapId, 0x44012400);

                            /* Start sequence */                            
                            wReg32(fsData.tapId, 0x44000600, 0x00000000);
                            wReg32(fsData.tapId, 0x44000600, 0x00000002);
                            wReg32(fsData.tapId, 0x44000600, 0x00004002);
                            /* destination address */
                            wReg32(fsData.tapId, 0x44000630, 0);
                            /* DATA0 */
                            wReg32(fsData.tapId, 0x44000640, fsData.readBuffer[0]);
                            /* DATA1 */
                            wReg32(fsData.tapId, 0x44000650, fsData.readBuffer[1]);
                            /* DATA2 */
                            wReg32(fsData.tapId, 0x44000660, fsData.readBuffer[2]);
                            /* DATA3 */
                            wReg32(fsData.tapId, 0x44000670, fsData.readBuffer[3]);
                            /* transfer sequence */
                            wReg32(fsData.tapId, 0x44000620, 0x00000000);
                            wReg32(fsData.tapId, 0x44000620, 0xAA996655);
                            wReg32(fsData.tapId, 0x44000620, 0x556699AA);                            
                            wReg32(fsData.tapId, 0x440006F0, 0x80000000);                           
                            wReg32(fsData.tapId, 0x44000620, 0x00000000);
                            wReg32(fsData.tapId, 0x44000620, 0xAA996655);
                            wReg32(fsData.tapId, 0x44000620, 0x556699AA);                           
                            wReg32(fsData.tapId, 0x44000600, 0x0000C002);


                            volatile uint32_t nvmCon = 0x8000;
                            while(nvmCon & 0x8000)
                                 nvmCon = rReg32(fsData.tapId, 0x44000600);
                                
                            
                        }
                        
                        for(cnt = 0; cnt < secSz; cnt++)
                        {                            
                            wReg32(fsData.tapId, fsData.flashAddr+(cnt*4), fsData.readBuffer[cnt]);                                
                        }                   

                        fsData.readCount += secSz*sizeof(uint32_t);
                        fsData.flashAddr += secSz*sizeof(uint32_t);
                        
                                        
                        if(remBytes)
                        {                           
//                                volatile uint32_t word = (fsData.readBuffer[cnt] << remBytes);
//                                word = word >> remBytes;                                
                            wReg32(fsData.tapId, fsData.flashAddr, fsData.readBuffer[cnt-1]);                                
                            fsData.readCount += remBytes;
                        }
                            

                        
                        if(fsData.fileStatus.fsize == fsData.readCount)
                        {
//                            SYS_CONSOLE_PRINT("Firmware load complete!\r\n");

                            PIN_MAP_t *mclrPin = gUmtCxt.devList[fsData.tapId].pinLink[PIN_MCLR];                     
                            *((volatile uint32_t *)((char *)mclrPin->gpio_reg + CLR)) = mclrPin->gpio_mask;    
                            CORETIMER_DelayUs(100);
                            *((volatile uint32_t *)((char *)mclrPin->gpio_reg + SET)) = mclrPin->gpio_mask;    

                            if(fsData.sysCmdDev)
                            {
                                (fsData.sysCmdDev->pCmdApi->print)(fsData.sysCmdDev->cmdIoParam, LINE_TERM ERROR, 0);
                                (fsData.sysCmdDev->pCmdApi->msg)(fsData.sysCmdDev->cmdIoParam,  LINE_TERM DONE); 
                            }
                            
                            /* The test was successful. */
                            fsData.state = FS_CLOSE_FILE;
                        }
                        
                        
                        break; /*CHIMERA_CHIP_ID*/
                    }
                    
                    case RIO0_CHIP_ID:
                    {

                        if (fsData.sramLoad)
                        {                 
//                            SYS_CONSOLE_PRINT("SRAM Load %X\r\n", fsData.flashAddr);
                            uint32_t wordSz = readSz/sizeof(uint32_t);
                            for(cnt = 0; cnt < wordSz; cnt++)
                            {
                                wReg32(fsData.tapId, fsData.flashAddr+(cnt*4), fsData.readBuffer[cnt]); 
                                CORETIMER_DelayUs(5);                                
                            }              
                            fsData.flashAddr += wordSz*sizeof(uint32_t); 
                        }
                        else
                        {
                            if(!fsData.readCount)
                            {
//                                SYS_CONSOLE_PRINT("Firmware Image Size %d\r\n", fsData.readBuffer[RIO0_FW_IMG_SIZE_OFFSET]);
//                                SYS_CONSOLE_PRINT("Firmware Image Dest Addr 0x%X\r\n", fsData.readBuffer[RIO0_FW_ROM_DST_OFFSET] & 0x00FFFFFF);
//                              
                                                                
                                
                            }
                                                        
#ifdef RIO0_FLASH_PROG                                                            
                            uint32_t secSz = readSz/256;
                            for(uint32_t blkNum = 0; blkNum < secSz; blkNum++)
                            {                                                        
                                RIO0_FLASH_PAGE_Write(fsData.tapId, fsData.flashAddr, (uint8_t *)&fsData.readBuffer[blkNum*64]);                             
                                fsData.flashAddr += 256; 
                            }   
#else                       
                            uint32_t wordSz = readSz/sizeof(uint32_t);     
                            for(cnt = 0; cnt < wordSz; cnt++)
                            {
                                wReg32(fsData.tapId, fsData.flashAddr+(cnt*4), fsData.readBuffer[cnt]); 
                                CORETIMER_DelayUs(5);
                            }                                                        
                            fsData.flashAddr += wordSz*sizeof(uint32_t);                             
#endif
                            
                        }
                        
                        fsData.readCount += readSz;
                        
                        
                        if ((fsData.fileStatus.fsize - fsData.fwOffset) == fsData.readCount)
                        {
//                            if(fsData.sysCmdDev)
//                                (fsData.sysCmdDev->pCmdApi->msg)(fsData.sysCmdDev->cmdIoParam, "Firmware load complete!\r\n");
                            
                            if (fsData.sramLoad)
                            {
                                EJTAG_Enter(fsData.tapId, true);  
                                
                                uint32_t chipId = EJTAG_Read(fsData.tapId, 0xBF800060);                                
                                if(fsData.sysCmdDev)
                                    (fsData.sysCmdDev->pCmdApi->print)(fsData.sysCmdDev->cmdIoParam, INFO "eJTAG Chip ID = 0x%X\r\n", chipId);
                                
                                EJTAG_OPCODE_WR(fsData.tapId, (0x3C020000 | ((fsData.jumpAddr >> 16) & 0xFFFF)));  // load upper immediate 0x3C02A008
                                EJTAG_OPCODE_WR(fsData.tapId, (0x34420000 | (fsData.jumpAddr & 0xFFFF)));  // or immediate 0x34420200                             
                                EJTAG_OPCODE_WR(fsData.tapId, 0x4082C000);  //MTC0 V0 DEPC
                                EJTAG_OPCODE_WR(fsData.tapId, 0x000000C0); // EHB                                                             
//                                /* DERET*/
                                EJTAG_OPCODE_WR(fsData.tapId, 0x4200001F);                                  
                                
                                if(fsData.sysCmdDev)
                                {
                                    (fsData.sysCmdDev->pCmdApi->print)(fsData.sysCmdDev->cmdIoParam, LINE_TERM ERROR, 0);
                                    (fsData.sysCmdDev->pCmdApi->msg)(fsData.sysCmdDev->cmdIoParam,  LINE_TERM DONE); 
                                }
                            }
                            else
                            {
                                
#ifdef RIO0_FLASH_PROG      
                                PIN_MAP_t *mclrPin = gUmtCxt.devList[fsData.tapId].pinLink[PIN_MCLR];                     
                                *((volatile uint32_t *)((char *)mclrPin->gpio_reg + CLR)) = mclrPin->gpio_mask;    
                                CORETIMER_DelayUs(100);
                                *((volatile uint32_t *)((char *)mclrPin->gpio_reg + SET)) = mclrPin->gpio_mask;    
#endif
                                
                                /*Load Boot loader*/                                                               
                                wReg32(fsData.tapId, 0x000B0000, fsData.fileStatus.fsize);
                                wReg32(fsData.tapId, 0x000B0004, fsData.targetAddr);
                                                                
                                FS_DEV_PROGRAM(fsData.tapId, 0x80200, 0x0, true, "hut_ate_rio0.Xproduction_bl.bin", fsData.sysCmdDev);
                                
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
                        
//            if (fsData.flashAddr - 0xA0000200 == fsData.readCount)
            {
                /* Close the file */
                if (SYS_FS_FileClose(fsData.fileHandle) != 0)
                {
                    SYS_CONSOLE_MESSAGE("File Close Error!\r\n");
                    fsData.state = FS_ERROR;
                }
                else
                {
                    fsData.state = FS_IDLE;
                }
                
            }            
            
            break;
        }

        case FS_FLASH_DUMP:
        {
            for(uint32_t cnt = 0; cnt < fsData.dumpSz; cnt+=4)
            {
                readBytes.word = rReg32(fsData.tapId, fsData.flashAddr + cnt);                 
                extern size_t UART2_Write(uint8_t* pWrBuffer, const size_t size );                      
                UART2_Write(&readBytes.bytes.b0, 1);
                UART2_Write(&readBytes.bytes.b1, 1);
                UART2_Write(&readBytes.bytes.b2, 1);
                UART2_Write(&readBytes.bytes.b3, 1);
                while(UART2_WriteCountGet())
                {
                    vTaskDelay(1U / portTICK_PERIOD_MS);

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
            if(fsData.triggerState != FS_IDLE)
            {                
                fsData.state = fsData.triggerState;
                fsData.triggerState = FS_IDLE;                                
            }

            break;
        }
        
        case FS_DEVICE_ERASE:
        {
            switch(gUmtCxt.devList[fsData.tapId].devId & DEV_MASK_ID_MASK)
            {
                case RIO0_CHIP_ID:
                {                    
                    if(!fsData.sramLoad)
                    {                                                                
                        RIO0_SYS_Initialize(fsData.tapId);
                        RIO0_FLASH_Initialize(fsData.tapId);                                                                                
                        RIO0_FLASH_Reset(fsData.tapId);          
                        volatile uint32_t flashId = RIO0_FLASH_ID_Read(fsData.tapId);
                        while(flashId != 0x1C7015) {
                            flashId = RIO0_FLASH_ID_Read(fsData.tapId);                            
                        }
                        
                        RIO0_SYS_Initialize(fsData.tapId);
                        RIO0_FLASH_Initialize(fsData.tapId);                                                 
                        if(fsData.sysCmdDev)
                            (fsData.sysCmdDev->pCmdApi->print)(fsData.sysCmdDev->cmdIoParam, INFO "Flash ID = 0x%X\r\n", flashId); 
                        
                        RIO0_FLASH_CHIP_Erase(fsData.tapId);  
                        if(fsData.sysCmdDev)
                        {
//                            (fsData.sysCmdDev->pCmdApi->msg)(fsData.sysCmdDev->cmdIoParam, "Flash Erase Successful!\r\n");                                 
                            (fsData.sysCmdDev->pCmdApi->print)(fsData.sysCmdDev->cmdIoParam, LINE_TERM ERROR, 0);
                            (fsData.sysCmdDev->pCmdApi->msg)(fsData.sysCmdDev->cmdIoParam,  LINE_TERM DONE); 
                        }
                        
                    }                    
                    break;
                }                
            }
            fsData.state = FS_IDLE; 
            
            break;
        }
        
        case FS_DEVICE_INIT:
        {
            switch(gUmtCxt.devList[fsData.tapId].devId & DEV_MASK_ID_MASK)
            {
                case CHIMERA_CHIP_ID:
                {
                    
                }
                case RIO0_CHIP_ID:
                {                    
                    if(!fsData.sramLoad)
                    {        
                                                    
                    }
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

int32_t FS_DEV_ERASE(uint32_t devId, void *cmdNode)
{
    if(gUmtCxt.devList[devId].devType != UMT_DEV_TMOD)
        return -1;   
    
    fsData.tapId = devId; 
    fsData.sysCmdDev = cmdNode;
    fsData.sramLoad = 0;    
    fsData.triggerState = FS_DEVICE_ERASE;    
      
    return 0;
}

int32_t FS_DEV_PROGRAM(uint32_t devId, uint32_t sof, uint32_t offset, bool sramLoad, char *fileName, void *cmdNode)
{
    if(gUmtCxt.devList[devId].devType != UMT_DEV_TMOD)
        return -1;   
    
    fsData.tapId = devId; 
    if(sramLoad)
        fsData.flashAddr = sof; 
    else
        fsData.flashAddr = 0x0;
    
    fsData.targetAddr = sof;
    fsData.jumpAddr = sof | 0xA0000000;
    fsData.fwOffset = offset;
    fsData.sramLoad = sramLoad;
    fsData.readCount = 0; 
    fsData.sysCmdDev = cmdNode;
    /* copy the file name */
    sprintf(fsData.fileName, "%s", fileName);
    fsData.triggerState = FS_OPEN_FILE;
    
    return 0;
}
/*******************************************************************************
 End of File
 */
