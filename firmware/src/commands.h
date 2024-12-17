/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    commands.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "COMMANDS_Initialize" and "COMMANDS_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "COMMANDS_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _COMMANDS_H
#define _COMMANDS_H

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
#define BANNER "\r\n\
 #     # #     # #######     #####  ####### #     # ####### ###  #####     #     #                      \r\n\
 #     # ##   ##    #       #     # #     # ##    # #        #  #     #    ##   ##  ####  #####  ###### \r\n\
 #     # # # # #    #       #       #     # # #   # #        #  #          # # # # #    # #    # #      \r\n\
 #     # #  #  #    #       #       #     # #  #  # #####    #  #  ####    #  #  # #    # #    # #####  \r\n\
 #     # #     #    #       #       #     # #   # # #        #  #     #    #     # #    # #    # #      \r\n\
 #     # #     #    #       #     # #     # #    ## #        #  #     #    #     # #    # #    # #      \r\n\
  #####  #     #    #        #####  ####### #     # #       ###  #####     #     #  ####  #####  ###### \r\n\
  \r\n"                                                                                                        
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
    COMMANDS_STATE_INIT=0,
    COMMANDS_STATE_SERVICE_TASKS,
    /* TODO: Define states used by the application state machine. */

} COMMANDS_STATES;



/* Maximum device contexts, can be UART/I2C/SPI etc..*/
#define UMT_DEV_MAX         10
#define UMT_DEV_PIN_MAX		10

// #define TESTBUS_40PIN

#ifndef TESTBUS_40PIN
#define TESTBUS_168PIN
#endif

#ifdef TESTBUS_168PIN
#define PINS_MAX    168
#else
#define PINS_MAX    40
#endif

#define RESERVED    0x01
#define GPIO        0x02
#define U_TX        0x04
#define U_RX        0x08
#define I2C_SCK     0x10
#define I2C_SDA     0x20
#define AN         	0x40
#define SPI			0x80


#define _PORTA_START_ADDR_	0xBF860000
#define _PORTB_START_ADDR_	0xBF860100
#define _PORTC_START_ADDR_	0xBF860200
#define _PORTD_START_ADDR_	0xBF860300
#define _PORTE_START_ADDR_	0xBF860400
#define _PORTF_START_ADDR_	0xBF860500
#define _PORTG_START_ADDR_	0xBF860600
#define _PORTH_START_ADDR_	0xBF860700
#define _PORTJ_START_ADDR_	0xBF860800
#define _PORTK_START_ADDR_	0xBF860900


#define CLR         0x34
#define SET         0x38

typedef enum
{
    UMT_DEV_UNKNOWN = -1,
    UMT_DEV_UART = 0,
    UMT_DEV_I2C,
    UMT_DEV_TMOD,
    UMT_DEV_ADC,    
    UMT_DEV_INVALID
}UMT_DEV_TYPE_t;



typedef enum
{
    JTAG_TAP_RESET,
    JTAG_IR_SCAN,
    JTAG_DR_SCAN,
    JTAG_EXIT
}JTAG_TMS_PULSE_t;


typedef enum
{
    PIN_TCK = 0,
    PIN_TMS,
    PIN_TDO,
    PIN_TDI,
    PIN_PGC,
    PIN_PGD,
    PIN_MCLR    
}TMOD_PINS;

typedef enum
{
    TAP_STATE_IDLE,
    TAP_STATE_IR_SCAN,
    TAP_STATE_DR_SCAN,
    TAP_STATE_IR_SHIFT,
    TAP_STATE_DR_SHIFT,    
    TAP_STATE_IR_EXIT,
    TAP_STATE_EXIT,
}TAP_STATE_t;

typedef struct
{
    uint32_t inuse;
    uint32_t support;    
    uint32_t mode;
    const uint32_t gpio_reg;
    const uint32_t gpio_mask;
    const uint32_t pps_reg_val;
    uint32_t    adc_channel;
}PIN_MAP_t;

typedef struct
{    
    UMT_DEV_TYPE_t  		devType;
    uint32_t    			pinCnt;                    
    uint32_t    			devId;
    volatile void        	*devFuncPtr;
    PIN_MAP_t   *pinLink[UMT_DEV_PIN_MAX];   

}UMT_DEV_t;



typedef struct 
{
    UMT_DEV_t   devList[UMT_DEV_MAX];
    int32_t    devCnt;    
}UMT_CXT_t;


typedef struct 
{
    UART_FUNC_Handler_t *uartList[5];
    int32_t             uartCnt;    
}UART_CXT_t;



//
//typedef union {
//  struct {    
//    uint32_t DATA:32;
//    uint32_t ADDR:30;
//    uint32_t BE0:1;
//    uint32_t BE1:1;
//    uint32_t BE2:1;
//    uint32_t BE3:1;
//    uint32_t W:1;
//    uint32_t CMD:2;
//    uint32_t MODE:1;
//    uint32_t STATUS:2;
//  };
//  struct {
//    uint8_t ICD_B[9];
//  };
//} __ICDREGbits_t;

/*
 TABLE 2-3: J12 CONNECTIONS
 Note: Since the MCP2221 receive (RX) line shares the same pin as SW3, SW3
 cannot be used in an application where the MCP2221 is also used.
 ******************************************************************************
Function 1  | Function 2    |   Pin     |   Pin     |   Function 2  | Function 1
 ******************************************************************************
   --       |     +3V3      |    1      |    2      |     +5V       |   ?
   RA3      |    SDA23      |    3      |    4      |     +5V       |   ?
   RA2      |    SCL2       |    5      |    6      |     GND       |   ?
   RA14     |     --        |    7      |    8      |     UxTX      |   RG6
   --       |     GND       |    9      |    10     |     UxRX      |   RB14
   RD0      |     --        |    11     |    12     |      --       |   RF2
   RJ13     |     --        |    13     |    14     |     GND       |   ?
   RB3      |     --        |    15     |    16     |      --       |   RK1
   --       |    +3V3       |    17     |    18     |      --       |   RK2
   RF5      |    MOSI       |    19     |    20     |     GND       |   --
   RF4      |    MISO       |    21     |    22     |      --       |   RK3
   RD1      |    SCLK       |    23     |    24     |      --       |   RJ5
   --       |    GND        |    25     |    26     |      --       |   RJ7
   --       |   No Connect  |    27     |    28     |   No Connect  |   ?
   RH7      |     --        |    29     |    30     |     GND       |   ?
   RH9      |     --        |    31     |    32     |      --       |   RH11
   RK4      |     --        |    33     |    34     |     GND       |   ?
   RK5      |     --        |    35     |    36     |      --       |   RK6
   RD10     |     --        |    37     |    38     |      --       |   RH15
   --       |     GND       |    39     |    40     |      --       |   RJ3
 *****************************************************************************
 */


    
              
/*ADC MACROs*/            

#define ADC_VREF                (3.3f)
#define ADC_MAX_COUNT           (4095)
            

#define PIN_SET_CLR(pPin, Offset) *((volatile uint32_t *)((volatile char *)(pPin.gpio_reg) +  Offset)) = pPin.gpio_mask;

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
    COMMANDS_STATES state;
    
    PIN_MAP_t   pin_map[PINS_MAX];

    /* TODO: Define any additional data used by the application. */

} COMMANDS_DATA;


extern UMT_CXT_t gUmtCxt;
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
    void COMMANDS_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    COMMANDS_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    COMMANDS_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void COMMANDS_Initialize ( void );


/*******************************************************************************
  Function:
    void COMMANDS_Tasks ( void )

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
    COMMANDS_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void COMMANDS_Tasks( void );

extern void U1RX_Callback_Handler( uintptr_t context );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _COMMANDS_H */

/*******************************************************************************
 End of File
 */

