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
    "TMOD_TAP_Initialize" and "TMOD_TAP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "TMOD_TAP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _TMOD_TAP_H
#define _TMOD_TAP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "tmod.h"
#include "config/default/peripheral/coretimer/plib_coretimer.h"

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

typedef enum 
{
    ICDREG_OP_RD,
    ICDREG_OP_WR,        
}ICDREG_OP_t;



#define DUMMY_READ              0x00
/* MCHP Commands*/
#define MCHP_CMD_USER_ID        0x01
#define MCHP_CMD_READ_STATUS    0x02
#define MCHP_CMD_DEASSERT       0xD0
#define MCHP_CMD_ASSERT         0xD1


/* CHIP TAP Commands*/
#define CHIP_TAP_HIGH_Z             0x00
#define CHIP_TAP_ID_CODE            0x01
#define CHIP_TAP_PRELOAD            0x02
#define CHIP_TAP_ICDREG             0x03
#define CHIP_TAP_SELECT_CHIP_TAP    0x04            
#define CHIP_TAP_EJTAG_SELECT       0x05
#define CHIP_TAP_EXTEST             0x06
#define CHIP_TAP_MCHP_CMD           0x07
#define CHIP_TAP_MCHP_STATUS        0x08
#define CHIP_TAP_SELECTALT          0x0A
#define CHIP_TAP_ALTRESET           0x0C
#define CHIP_TAP_CHIPE_ERASE        0xFC


#define DEV_MASK_ID_MASK    0x0FF00000
#define RIO0_CHIP_ID        0x09C00000
#define CHIMERA_CHIP_ID     0x09B00000

uint32_t EJTAG_Enter(uint32_t devId, bool mclr);

uint32_t EJTAG_Exit(uint32_t devId, bool mclr);

void EJTAG_OPCODE_WR(uint32_t devId, uint32_t opcode);

uint32_t EJTAG_Write(uint32_t devId, uint32_t addr, uint32_t data);

uint32_t EJTAG_Read(uint32_t devId, uint32_t addr);

uint32_t EJTAG_TAP_RD(uint32_t devId, uint32_t dReg);


int32_t TMOD_TAP_Init(uint32_t devId);

int32_t TMOD_TAP_Reset(uint32_t devId);

int32_t TMOD_TAP_Idle(uint32_t devId);

int32_t TMOD_Pattern(uint32_t devId);

uint32_t TMOD_TAP_IR(uint32_t devId, uint32_t iReg);

uint32_t TMOD_TAP_DR(uint32_t devId, uint32_t dReg);

uint32_t TMOD_TAP_ICDREG(uint32_t devId, uint32_t addr, uint32_t data, ICDREG_OP_t opMode);

#define wReg32(devId, addr, data)	TMOD_TAP_ICDREG(devId, addr, data, ICDREG_OP_WR)

#define rReg32(devId, addr)	TMOD_TAP_ICDREG(devId, addr, 0x0, ICDREG_OP_RD)

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _TMOD_TAP_H */

/*******************************************************************************
 End of File
 */

