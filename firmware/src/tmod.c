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
/* Application Data */ 

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


#define IR_5BIT_CMD_LEN  28
#define IR_5BIT_CMD_IDX  IR_5BIT_CMD_LEN - TMOD_TAP_IR_HDR - 1

static uint32_t CACHE_ALIGN IR_5BIT_STREAM[2][IR_5BIT_CMD_LEN] = {
                    {CLR, CLR, CLR, SET, SET, SET, SET, CLR, CLR, SET, SET, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, SET, SET, SET, SET, CLR}, // TMS
                    {CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR}  // TDO
                    };


#define DR_8BIT_CMD_LEN  32
#define DR_8BIT_CMD_IDX  DR_8BIT_CMD_LEN - TMOD_TAP_DR_HDR -1

static uint32_t CACHE_ALIGN DR_8BIT_STREAM[3][DR_8BIT_CMD_LEN] = {                    
                    {CLR, CLR, CLR, SET, SET, SET, SET, CLR, CLR, SET, SET, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, SET, SET, CLR}, // TMS
                    {CLR, CLR, CLR, SET, SET, SET, SET, SET, SET, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR},  // TDO
                    {CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR}  // TDI
                    };


#define DR_32BIT_CMD_LEN  80
#define DR_32BIT_CMD_IDX  DR_32BIT_CMD_LEN - TMOD_TAP_DR_HDR - 1
static uint32_t CACHE_ALIGN DR_32BIT_STREAM[3][DR_32BIT_CMD_LEN] = {                    
                    {CLR, CLR, CLR, SET, SET, SET, SET, CLR, CLR, SET, SET, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, SET, SET, CLR}, // TMS
                    {CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR},  // TDO
                    {CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR}  // TDI
                    };


/*MSB First */

#define ICD_REG_LEN     (158 + 10)// dummy 20 bits


#define ICD_REG_HDR_LEN      (20 + TMOD_TAP_DR_HDR)


#define ICD_ADDR_MSB_IDX     (ICD_REG_LEN - ICD_REG_HDR_LEN - 1)
#define ICD_ADDR_LSB_IDX     (ICD_ADDR_MSB_IDX - 60)

#define ICD_DATA_MSB_IDX     (ICD_ADDR_MSB_IDX - 60)
#define ICD_DATA_LSB_IDX     (ICD_DATA_MSB_IDX - 64)

static uint32_t CACHE_ALIGN ICD_REG_STREAM[3][ICD_REG_LEN] = {                    
                    {CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, SET, SET, SET, SET, CLR, CLR, SET, SET, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, SET, SET, CLR}, // TMS 
                    {CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, SET, SET, SET, SET, SET, SET, SET, SET, SET, SET, SET, SET, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR, CLR}, // TDO
                    };



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
        /* clock line */
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
                      
    /* Each bit is transmitted twice on the clock sizof(uint32_t) * 2 */
    if(*length)
    {     
        (*length)--;                           
        /* keep inverting the clock */
        *((volatile uint32_t *)((volatile char *)tmsPin->gpio_reg + IR_5BIT_STREAM[0][*length])) =  tmsPin->gpio_mask;          
        *((volatile uint32_t *)((volatile char *)tdoPin->gpio_reg + IR_5BIT_STREAM[1][*length])) =  tdoPin->gpio_mask;          
        /* clock line  */
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
        /* clock line, keep inverting the clock */
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
        /* clock line, keep inverting the clock */
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

uint32_t TMOD_TAP_ICDREG(uint32_t devId, uint32_t addr, uint32_t data, ICDREG_OP_t op)
{   
    volatile int32_t length = ICD_REG_LEN;     
    uint32_t icdReg = 0;
#if NO_DELAY

#else    
    int32_t devIdx = devId;
    tmrCxt[0] = &devIdx;
    tmrCxt[1] = &length;           
//    SYS_TIME_HANDLE triggerTmr = 0;
#endif    
    uint32_t idx = 0 ; // 

    if(gUmtCxt.devList[devId].devType != UMT_DEV_TMOD)
        return -1;
        
   
    idx = ICD_DATA_LSB_IDX;
    if(op == ICDREG_OP_WR)
    {
//        for(volatile uint32_t i  = ICD_DATA_LSB_IDX; i < ICD_DATA_MSB_IDX; i+=2)
        while(data)
        {   
            ICD_REG_STREAM[1][idx++] = (data & 1)?(SET):(CLR);    
            ICD_REG_STREAM[1][idx++] = (data & 1)?(SET):(CLR);
            data = data >> 1; // bit 0             
        }        
        
        while(idx < ICD_DATA_MSB_IDX)
        {
            ICD_REG_STREAM[1][idx++] = CLR; 
        }
        
        ICD_REG_STREAM[1][ICD_REG_LEN-(TMOD_TAP_DR_HDR + 11)] = SET;    
        ICD_REG_STREAM[1][ICD_REG_LEN-(TMOD_TAP_DR_HDR + 12)] = SET; 
    }
    else
    {
        ICD_REG_STREAM[1][ICD_REG_LEN-(TMOD_TAP_DR_HDR + 11)] = CLR;    
        ICD_REG_STREAM[1][ICD_REG_LEN-(TMOD_TAP_DR_HDR + 12)] = CLR;    
    }
    
    idx = ICD_ADDR_LSB_IDX;    
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
    
      
#ifdef NO_DELAY    
    while(length)
    {
        length--;     
        
        *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devId].pinLink[PIN_TMS]->gpio_reg) + ICD_REG_STREAM[0][length])) =  gUmtCxt.devList[devId].pinLink[PIN_TMS]->gpio_mask;          
        *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devId].pinLink[PIN_TDO]->gpio_reg) + ICD_REG_STREAM[1][length])) =  gUmtCxt.devList[devId].pinLink[PIN_TDO]->gpio_mask;                          
        *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devId].pinLink[PIN_TCK]->gpio_reg) + 0x3C)) = gUmtCxt.devList[devId].pinLink[PIN_TCK]->gpio_mask;               
    }    
    
    if(op == ICDREG_OP_RD)
    {        
//        CORETIMER_DelayUs(1);
        
        for(int32_t idx = ICD_REG_LEN - 1; idx >= 0; idx--)
        {
            *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devId].pinLink[PIN_TMS]->gpio_reg) + ICD_REG_STREAM[0][idx])) =  gUmtCxt.devList[devId].pinLink[PIN_TMS]->gpio_mask;                  
//            *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devId].pinLink[PIN_TDO]->gpio_reg) + CLR)) =  gUmtCxt.devList[devId].pinLink[PIN_TDO]->gpio_mask;                          
            *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devId].pinLink[PIN_TCK]->gpio_reg) + 0x3C)) = gUmtCxt.devList[devId].pinLink[PIN_TCK]->gpio_mask;            
            ICD_REG_STREAM[2][idx] = *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devId].pinLink[PIN_TDI]->gpio_reg) + 0x20)) & gUmtCxt.devList[devId].pinLink[PIN_TDI]->gpio_mask;        
        }
        
        for(volatile uint32_t i  = ICD_DATA_MSB_IDX - 1; i > ICD_DATA_LSB_IDX; i-=2)
        {   
            icdReg = icdReg << 1;
            icdReg |= (ICD_REG_STREAM[2][i])?1:0;                
        }
        
    }      
#else        
    TMR2_CallbackRegister(Inject_ICD_REG, (uintptr_t)tmrCxt);
    TMR2_Start();
    while(length > 0);           
    TMR2_Stop();
#endif
    
        
    return icdReg;
}

uint32_t EJTAG_TAP_RD(uint32_t devId, uint32_t dReg)
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
        
    for(int i = 0; i <  DR_32BIT_CMD_LEN; i++)
    {
        DR_32BIT_STREAM[1][i] = CLR; 
        DR_32BIT_STREAM[2][i] = 0;
    }

    TMR2_CallbackRegister(Inject_32BIT_DR, (uintptr_t)tmrCxt);
    TMR2_Start();
//        triggerTmr = SYS_TIME_CallbackRegisterUS(Inject_32BIT_DR, (uintptr_t)tmrCxt, 2, SYS_TIME_PERIODIC);
    
    
    //Wait until we send complete pattern
    while(length > 0);
    TMR2_Stop();
    
//    while(length > 0);
//    SYS_TIME_TimerDestroy(triggerTmr);
    
    for(i  = (DR_32BIT_CMD_IDX - 66 + 1); i < DR_32BIT_CMD_IDX - 2; i+=2)
    {
        tapReg = tapReg << 1;
        tapReg |= (DR_32BIT_STREAM[2][i] == 0)?0:1;        
    }
    
     
    return tapReg;

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
    
    if(dReg && dReg < 0x100 && dReg != 0xC0)
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
int32_t TMOD_TAP_Idle(uint32_t devId)
{
    volatile int32_t length = 12; //TAP_RESET_CMD_LEN;
    volatile int32_t devIdx  = devId;            
    tmrCxt[0] = &devIdx;
    tmrCxt[1] = &length; 
    
    if(gUmtCxt.devList[devId].devType != UMT_DEV_TMOD)
        return -1;   
    
    
    for(int cnt = 0; cnt < length; cnt++)
    {        
        *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devIdx].pinLink[PIN_TDO]->gpio_reg) + CLR)) = gUmtCxt.devList[devIdx].pinLink[PIN_TDO]->gpio_mask;
        *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devIdx].pinLink[PIN_TMS]->gpio_reg) + CLR)) = gUmtCxt.devList[devIdx].pinLink[PIN_TMS]->gpio_mask;
        CORETIMER_DelayUs(1);
        *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devIdx].pinLink[PIN_TCK]->gpio_reg) + 0x3C)) = gUmtCxt.devList[devIdx].pinLink[PIN_TCK]->gpio_mask;                
    }
    
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
        *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devIdx].pinLink[PIN_TDO]->gpio_reg) + SET)) = gUmtCxt.devList[devIdx].pinLink[PIN_TDO]->gpio_mask;
        *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devIdx].pinLink[PIN_TMS]->gpio_reg) + SET)) = gUmtCxt.devList[devIdx].pinLink[PIN_TMS]->gpio_mask;
        CORETIMER_DelayUs(1);
        *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devIdx].pinLink[PIN_TCK]->gpio_reg) + 0x3C)) = gUmtCxt.devList[devIdx].pinLink[PIN_TCK]->gpio_mask;                
    }
    *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devIdx].pinLink[PIN_TMS]->gpio_reg) + CLR)) = gUmtCxt.devList[devIdx].pinLink[PIN_TMS]->gpio_mask;
    *((volatile uint32_t *)((volatile char *)(gUmtCxt.devList[devIdx].pinLink[PIN_TDO]->gpio_reg) + CLR)) = gUmtCxt.devList[devIdx].pinLink[PIN_TDO]->gpio_mask;
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
    PIN_MAP_t *tdiPin = gUmtCxt.devList[devId].pinLink[PIN_TDI];
    
    *((volatile uint32_t *)((char *)mclrPin->gpio_reg + CLR)) = mclrPin->gpio_mask;        
    CORETIMER_DelayMs(1);
    *((volatile uint32_t *)((char *)mclrPin->gpio_reg + SET)) = mclrPin->gpio_mask;    
    CORETIMER_DelayMs(5);
  
    // set the MCLR LOW
    *((volatile uint32_t *)((char *)mclrPin->gpio_reg + CLR)) = mclrPin->gpio_mask;    
    CORETIMER_DelayUs(30);
    
#ifdef NO_DELAY
    TMR2_CallbackRegister(Inject_TMOD12, (uintptr_t)tmrCxt);
    TMR2_Start();
    while(length > 0);                
    TMR2_Stop();
#else        
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
#endif
    
    CORETIMER_DelayUs(30);
    *((volatile uint32_t *)((char *)mclrPin->gpio_reg + SET)) = mclrPin->gpio_mask;
    CORETIMER_DelayUs(30);
    
    
    // input pins
    *((volatile uint32_t *)((volatile char *)(tdiPin->gpio_reg) + 0x18)) = tdiPin->gpio_mask;
    
    return 0;
       
}
// Triger the TMOD12
int32_t TMOD_TAP_Init(uint32_t devId)
{
//    TMOD_Pattern(devId);
    TMOD_TAP_Reset(devId);    
    gUmtCxt.devList[devId].devId = TMOD_TAP_DR(devId, DUMMY_READ);        

    switch(gUmtCxt.devList[devId].devId  & 0x0FFFFFFF)
    {
        case CHIMERA_CHIP_ID:
        {
            static int32_t timeout = 256;                                            

            /* Erase the CHIP */
            TMOD_TAP_IR(devId, CHIP_TAP_MCHP_CMD);  
            TMOD_TAP_DR(devId, CHIP_TAP_CHIPE_ERASE);                        
            TMOD_TAP_DR(devId, MCHP_CMD_DEASSERT);
            while((timeout-- > 0) && (TMOD_TAP_DR(devId, DUMMY_READ) != 0x88));   

            /* Enter the ICDREG mode */
            TMOD_TAP_IR(devId, CHIP_TAP_ICDREG);   

            /* SRAM Start Address*/
            wReg32(devId, 0x20000000, 0xABCD1234);
            if(rReg32(devId, 0x20000000) == 0xABCD1234)
            {
                SYS_CONSOLE_PRINT("TMOD12 entry success!\r\n");                          
            }                    

            /* write first 16 bytes at address 0 */                     
            break;
        }

        case RIO0_CHIP_ID:
        {                                
            TMOD_TAP_Reset(devId);
            TMOD_TAP_IR(devId, CHIP_TAP_SELECT_CHIP_TAP);    
            TMOD_TAP_IR(devId, CHIP_TAP_MCHP_CMD);  
            TMOD_TAP_IR(devId, CHIP_TAP_EJTAG_SELECT);
            TMOD_TAP_Idle(devId);
            TMOD_TAP_IR(devId, CHIP_TAP_ALTRESET); 
            TMOD_TAP_Idle(devId);
            TMOD_TAP_IR(devId, CHIP_TAP_SELECT_CHIP_TAP);    
            TMOD_TAP_IR(devId, CHIP_TAP_MCHP_CMD); 
            TMOD_TAP_DR(devId, MCHP_CMD_DEASSERT);
            TMOD_TAP_Idle(devId);
            TMOD_TAP_IR(devId, CHIP_TAP_SELECT_CHIP_TAP);
            TMOD_TAP_IR(devId, CHIP_TAP_ICDREG);   

            uint32_t chipId = rReg32(devId, 0x1F800060);
            SYS_CONSOLE_PRINT("Chip ID = 0x%X\r\n", chipId);

            wReg32(devId, 0x00001000, 0xABCD1234);
            if(rReg32(devId, 0x00001000) == 0xABCD1234)
            {
                SYS_CONSOLE_PRINT("TMOD12 entry success!\r\n");                                           
            }

            break;
        }
    }
    
//    TMOD_TAP_Reset(devId); 
//    TMOD_TAP_ICDREG(devId, 0x20000000, 0x00000000, ICDREG_OP_RD);
         
//    CORETIMER_DelayMs(10);
//    TMOD_TAP_ICDREG(devId, 0x20000000, 0xABCD1234, ICDREG_OP_WR);
//    CORETIMER_DelayMs(1);
//    TMOD_TAP_ICDREG(devId, 0x20000000, 0x00000000, ICDREG_OP_RD);
//    CORETIMER_DelayMs(1);
//    TMOD_TAP_ICDREG(devId, 0x20000000, 0x00000000, ICDREG_OP_RD);
//    
//    TMOD_TAP_ICDREG(devId, 0x01000000, 0x00000000, ICDREG_OP_RD);
//    CORETIMER_DelayMs(1);
//
//    TMOD_TAP_ICDREG(devId, 0x01000000, 0x00000000, ICDREG_OP_RD);
    
    if(!gUmtCxt.devList[devId].devId)
        return -1;    
    
    return 0;
           
    
}

void EJTAG_OPCODE_WR(uint32_t devId, uint32_t opcode)
{
    /* Write higher 16-bits of address */
    TMOD_TAP_IR(devId, 0x0A);    
    TMOD_TAP_DR(devId, 0x8004D000);
    TMOD_TAP_IR(devId, 0x09);    
    TMOD_TAP_DR(devId, opcode);
    TMOD_TAP_IR(devId, 0x0A);
    TMOD_TAP_DR(devId, 0x8000C000);
    
}

uint32_t EJTAG_Write(uint32_t devId, uint32_t addr, uint32_t data)
{
    /* Write higher 16-bits of address */
//    
//    TMOD_TAP_IR(devId, 0x0A);
//    TMOD_TAP_DR(devId, 0x0004D000);
//    TMOD_TAP_IR(devId, 0x09);    
    EJTAG_OPCODE_WR(devId, (((addr >> 16) & 0x0000FFFF) | 0x3C100000));
//    TMOD_TAP_IR(devId, 0x0A);
//    TMOD_TAP_DR(devId, 0x0000C000);
    
    /* Write lower 16-bits of address */
//    TMOD_TAP_IR(devId, 0x0A);
//    TMOD_TAP_DR(devId, 0x0004D000);
//    TMOD_TAP_IR(devId, 0x09);
    EJTAG_OPCODE_WR(devId, (((addr >> 0) & 0x0000FFFF) | 0x3C080000));    
//    TMOD_TAP_IR(devId, 0x0A);
//    TMOD_TAP_DR(devId, 0x0000C000);
    
    /* Write higher 16-bits of data */
//    TMOD_TAP_IR(devId, 0x0A);
//    TMOD_TAP_DR(devId, 0x0004D000);
//    TMOD_TAP_IR(devId, 0x09);
    EJTAG_OPCODE_WR(devId, (((data >> 16) & 0x0000FFFF) | 0x35080000));    
//    TMOD_TAP_IR(devId, 0x0A);
//    TMOD_TAP_DR(devId, 0x0000C000);
    
    /* Write lower 16-bits of data */
//    TMOD_TAP_IR(devId, 0x0A);
//    TMOD_TAP_DR(devId, 0x0004D000);
//    TMOD_TAP_IR(devId, 0x09);
    EJTAG_OPCODE_WR(devId, (((data >> 0) & 0x0000FFFF) | 0xAE080000));        
//    TMOD_TAP_IR(devId, 0x0A);
//    TMOD_TAP_DR(devId, 0x0000C000);
//    
   
    return 0;
}


uint32_t EJTAG_Read(uint32_t devId, uint32_t addr)
{
    EJTAG_OPCODE_WR(devId, 0x3C13FF20);
    EJTAG_OPCODE_WR(devId, (((addr >> 16) & 0x0000FFFF) | 0x3C080000));
    EJTAG_OPCODE_WR(devId, (((addr >> 0) & 0x0000FFFF) | 0x35080000));
    EJTAG_OPCODE_WR(devId, 0x8D090000);
    EJTAG_OPCODE_WR(devId, 0xAE690000);
    EJTAG_OPCODE_WR(devId, 0x00000000);
    TMOD_TAP_IR(devId, 0x0E);
    
    return EJTAG_TAP_RD(devId, 0x00000000);
    
}

uint32_t EJTAG_Enter(uint32_t devId, bool mclr)
{    
    if(mclr)
    {        
        PIN_MAP_t *mclrPin = gUmtCxt.devList[devId].pinLink[PIN_MCLR];    
        /* Trigger a Master Reset*/
        *((volatile uint32_t *)((char *)mclrPin->gpio_reg + CLR)) = mclrPin->gpio_mask;       
        CORETIMER_DelayMs(1);        
        TMOD_TAP_Idle(devId);        
        TMOD_TAP_IR(devId, CHIP_TAP_EJTAG_SELECT); // 0x05
        TMOD_TAP_Idle(devId);
        TMOD_TAP_IR(devId, CHIP_TAP_ALTRESET); // 0x0C
        TMOD_TAP_Idle(devId);
        CORETIMER_DelayMs(1);
        *((volatile uint32_t *)((char *)mclrPin->gpio_reg + SET)) = mclrPin->gpio_mask;                     
    }
    TMOD_TAP_Idle(devId);    
    TMOD_TAP_IR(devId, CHIP_TAP_EJTAG_SELECT);        
    TMOD_TAP_Idle(devId);    
    TMOD_TAP_IR(devId, CHIP_TAP_SELECTALT);
    CORETIMER_DelayUs(1);
    
    TMOD_TAP_DR(devId, 0x0004D000);    
    TMOD_TAP_DR(devId, 0x8004D000);
       
    return 0;
}

uint32_t EJTAG_Exit(uint32_t devId, bool mclr)
{    
    
    TMOD_TAP_Init(devId);    
    
    return 0;
}



/*******************************************************************************
 End of File
 */
