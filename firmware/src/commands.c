/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    commands.c

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

#include "commands.h"
#include "config/default/definitions.h"

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
    This structure should be initialized by the COMMANDS_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

COMMANDS_DATA commandsData = {
                        .pin_map = {
#ifdef TESTBUS_40PIN							
                        {0, RESERVED, 0, 0, 0},													//1
                        {0, RESERVED, 0, 0, 0},										            //2
                        {0, GPIO | I2C_SDA, 0, 0xBF860000, _PORTA_RA3_MASK},                    //3
                        {0, RESERVED, 0},                                                       //4
                        {0, GPIO | I2C_SCK, 0, 0xBF860000, _PORTA_RA2_MASK},                    //5
                        {0, RESERVED, 0, 0, 0},                                                 //6
                        {0, GPIO, 0, 0xBF860000, _PORTA_RA14_MASK},                             //7
                        {0, GPIO, 0, 0xBF860600, _PORTG_RG6_MASK},                       		//8
                        {0, RESERVED, 0, 0, 0},                                                 //9
                        {0, GPIO | AN, 0, 0xBF860100, _PORTB_RB14_MASK, 0, ADCHS_CH9},             //10
                        {0, GPIO, 0, 0xBF860300, _PORTD_RD0_MASK},                              //11
                        {0, GPIO, 0, 0xBF860500, _PORTF_RF2_MASK},                              //12
                        {0, GPIO, 0, 0xBF860800, _PORTJ_RJ13_MASK},                             //13
                        {0, RESERVED, 0, 0, 0},                                                 //14
                        {0, GPIO | AN, 0, 0xBF860100, _PORTB_RB3_MASK, 0, ADCHS_CH3},              //15
                        {0, GPIO, 0, 0xBF860900, _PORTK_RK1_MASK},                              //16
                        {0, RESERVED, 0, 0, 0},                                                 //17
                        {0, GPIO, 0, 0xBF860900, _PORTK_RK2_MASK},                              //18
                        {0, GPIO | U_TX, 0, 0xBF860500, _PORTF_RF5_MASK, (const uint32_t)&RPF5R/*0xBF801654*/},           //19 U1_Tx
                        {0, RESERVED, 0, 0, 0},                                                 //20
                        {0, GPIO | U_RX, 0, 0xBF860500, _PORTF_RF4_MASK, 2},                    //21 U1_Rx
                        {0, GPIO, 0, 0xBF860900, _PORTK_RK3_MASK},                              //22
                        {0, GPIO | SPI, 0, 0xBF860300, _PORTD_RD1_MASK},                 		//23
                        {0, GPIO, 0, 0xBF860800, _PORTJ_RJ5_MASK},                              //24
                        {0, RESERVED, 0, 0, 0},                                                 //25
                        {0, GPIO, 0, 0xBF860800, _PORTJ_RJ7_MASK},                              //26
                        {0, RESERVED, 0, 0, 0},                                                 //27
                        {0, RESERVED, 0, 0, 0},                                                 //28
                        {0, GPIO, 0, 0xBF860700, _PORTH_RH7_MASK},                              //29
                        {0, RESERVED, 0, 0, 0},                                                 //30
                        {0, GPIO, 0, 0xBF860700, _PORTH_RH9_MASK},                              //31
                        {0, GPIO, 0, 0xBF860700, _PORTH_RH11_MASK},                             //32
                        {0, GPIO, 0, 0xBF860900, _PORTK_RK4_MASK},                              //33
                        {0, RESERVED, 0, 0, 0},                                                 //34
                        {0, GPIO, 0, 0xBF860900, _PORTK_RK5_MASK},                              //35
                        {0, GPIO, 0, 0xBF860900, _PORTK_RK6_MASK},                              //36
                        {0, GPIO, 0, 0xBF860300, _PORTD_RD10_MASK},                             //37
                        {0, GPIO, 0, 0xBF860700, _PORTH_RH15_MASK},                             //38
                        {0, RESERVED, 0, 0, 0},                                                 //39
                        {0, GPIO, 0, 0xBF860800, _PORTJ_RJ3_MASK}                               //40
#else
						{0, RESERVED, 0, 0, 0},		// jtag																	//1
						{0, RESERVED, 0, 0, 0},		// jtag                                                                 //2
						{0, RESERVED, 0, 0, 0},		// jtag                                                                 //3
						{0, RESERVED, 0, 0, 0},		// jtag                                                                 //4
						{0, RESERVED, 0, 0, 0},		// debug                                                                //5
						{0, RESERVED, 0, 0, 0},		// debug                                                                //6
						{0, RESERVED, 0, 0, 0},		// debug                                                                //7
						{0, RESERVED, 0, 0, 0},		// debug                                                                //8
						{0, RESERVED, 0, 0, 0},		// gnd                                                                  //9
						{0, RESERVED, 0, 0, 0},		// gnd                                                                  //10
						{0, RESERVED, 0, 0, 0},		// +5V_EXT                                                              //11
						{0, RESERVED, 0, 0, 0},		// +5V_EXT                                                              //12
						{0, RESERVED, 0, 0, 0},		// mclr                                                                 //13
                        {0, GPIO, 0, _PORTD_START_ADDR_, _PORTD_RD0_MASK}, 		//INT0/RD0	                                //14
						{0, GPIO, 0, _PORTC_START_ADDR_, _PORTC_RC13_MASK},		//secondary osc i/p/RC13		            //15
						{0, GPIO, 0, _PORTG_START_ADDR_, _PORTG_RG13_MASK}, 	//SQID0/RG13						        //16
						{0, GPIO, 0, _PORTC_START_ADDR_, _PORTC_RC14_MASK},		//secondary osc 0/p/RC13		            //17
						{0, GPIO, 0, _PORTG_START_ADDR_, _PORTG_RG12_MASK}, 	//SQID1/RG12						        //18
                        {0, GPIO, 0, _PORTD_START_ADDR_, _PORTD_RD4_MASK}, 		//SQICS0/RD4	                            //19
						{0, GPIO, 0, _PORTG_START_ADDR_, _PORTG_RG14_MASK}, 	//SQID2/RG14						        //20
	                    {0, GPIO, 0, _PORTD_START_ADDR_, _PORTD_RD5_MASK}, 		//SQICS1/RD5		                        //21
						{0, GPIO, 0, _PORTA_START_ADDR_, _PORTA_RA7_MASK},		// SQID3/A7                                 //22
						{0, GPIO, 0, _PORTG_START_ADDR_, _PORTG_RG15_MASK},		// RG15                                     //23
						{0, GPIO, 0, _PORTA_START_ADDR_, _PORTA_RA6_MASK},		// SQICLK/A6                                //24
						{0, GPIO, 0, _PORTJ_START_ADDR_, _PORTJ_RJ5_MASK},		// RJ15                                     //25
						{0, RESERVED, 0, 0, 0},		// nc                                                                   //26
						{0, RESERVED, 0, 0, 0},		// PMA1                                                                 //27
						{0, RESERVED, 0, 0, 0},		// nc                                                                   //28
						{0, GPIO | U_TX, 0, _PORTG_START_ADDR_, _PORTG_RG9_MASK, 0xBF8016A4},		// RG9					//29
						{0, RESERVED, 0, 0, 0},		// nc                                                                   //30
						{0, RESERVED, 0, 0, 0}, 	// PMA3                                                                 //31
						{0, RESERVED, 0, 0, 0},		// nc                                                                   //32
						{0, RESERVED, 0, 0, 0},		// PMA4                                                                 //33
						{0, RESERVED, 0, 0, 0},		// nc                                                                   //34
						{0, GPIO, 0, _PORTA_START_ADDR_, _PORTA_RA5_MASK},		// RA5                                      //35
						{0, RESERVED, 0, 0, 0},		// nc                                                                   //36
						{0, RESERVED, 0, 0, 0},		// PMA6                                                                 //37
						{0, RESERVED, 0, 0, 0},		// nc                                                                   //38
						{0, RESERVED, 0, 0, 0},		// gnd                                                                  //39
						{0, RESERVED, 0, 0, 0},		// gnd                                                                  //40
						{0, RESERVED, 0, 0, 0},		// 32_VDD																//41
						{0, RESERVED, 0, 0, 0}, 	// 32_VDD                                                               //42
						{0, RESERVED, 0, 0, 0},		// PMA7                                                                 //43
						{0, RESERVED, 0, 0, 0},		// nc                                                                   //44
						{0, RESERVED, 0, 0, 0},		// PMA8                                                                 //45
						{0, RESERVED, 0, 0, 0},		// nc                                                                   //46
						{0, RESERVED, 0, 0, 0},		// PMA9                                                                 //47
						{0, RESERVED, 0, 0, 0},		// nc                                                                   //48
						{0, GPIO | U_RX, 0, _PORTB_START_ADDR_, _PORTB_RB8_MASK},	// RB8                                  //49
						{0, RESERVED, 0, 0, 0},		// nc						                                            //50
						{0, GPIO, 0, _PORTB_START_ADDR_, _PORTB_RB12_MASK},	// RB12                                         //51
						{0, RESERVED, 0, 0, 0},		// nc						                                            //52
						{0, GPIO, 0, _PORTB_START_ADDR_, _PORTC_RC2_MASK},	// RC2                                          //53
						{0, RESERVED, 0, 0, 0},		// nc                                                                   //54
						{0, GPIO, 0, _PORTJ_START_ADDR_, _PORTJ_RJ13_MASK},	// RJ13                                         //55
						{0, RESERVED, 0, 0, 0},		// nc                                                                   //56
						{0, GPIO, 0, _PORTA_START_ADDR_, _PORTA_RA4_MASK},	// RA4                                          //57
						{0, RESERVED, 0, 0, 0},		// nc                                                                   //58
						{0, GPIO | U_RX, 0, _PORTD_START_ADDR_, _PORTD_RD9_MASK, 0x0000},	// RD9                          //59
						{0, RESERVED, 0, 0, 0},		// nc                                                                   //60
						{0, GPIO, 0, _PORTK_START_ADDR_, _PORTK_RK0_MASK},	// RK0                                          //61
						{0, GPIO, 0, _PORTH_START_ADDR_, _PORTH_RH2_MASK},	// RH2                                          //62
						{0, GPIO, 0, _PORTK_START_ADDR_, _PORTK_RK3_MASK},	// RK3                                          //63
						{0, GPIO, 0, _PORTH_START_ADDR_, _PORTH_RH4_MASK},	// RH3                                          //64
						{0, GPIO, 0, _PORTK_START_ADDR_, _PORTK_RK4_MASK},	// RK4                                          //65
						{0, RESERVED, 0, 0, 0},		// nc                                                                   //66
						{0, GPIO, 0, _PORTK_START_ADDR_, _PORTK_RK5_MASK},	// RK5                                          //67
						{0, GPIO, 0, _PORTC_START_ADDR_, _PORTC_RC3_MASK},	// RC3						                    //68
						{0, RESERVED, 0, 0, 0},		// nc                                                                   //69
						{0, RESERVED, 0, 0, 0},		// gnd                                                                  //70
						{0, RESERVED, 0, 0, 0}, 	// 32_VDD                                                               //71
						{0, RESERVED, 0, 0, 0}, 	// 32_VDD                                                               //72
						{0, GPIO, 0, _PORTK_START_ADDR_, _PORTK_RK6_MASK},	// RK6                                          //73
						{0, GPIO, 0, _PORTJ_START_ADDR_, _PORTJ_RJ4_MASK},	// RJ4                                          //74
						{0, GPIO, 0, _PORTK_START_ADDR_, _PORTK_RK7_MASK},	// RK7                                          //75
						{0, GPIO, 0, _PORTJ_START_ADDR_, _PORTJ_RJ5_MASK},	// RJ5						                    //76
						{0, GPIO, 0, _PORTJ_START_ADDR_, _PORTJ_RJ3_MASK},	// RJ3											//77	
						{0, GPIO, 0, _PORTJ_START_ADDR_, _PORTJ_RJ6_MASK},	// RJ6						                    //78
						{0, GPIO, 0, _PORTH_START_ADDR_, _PORTH_RH15_MASK},	// RH15                                         //79
						{0, GPIO, 0, _PORTJ_START_ADDR_, _PORTJ_RJ7_MASK},	// RJ7                                          //80
						{0, GPIO, 0, _PORTJ_START_ADDR_, _PORTJ_RJ12_MASK},	// RJ12                                         //81
						{0, GPIO, 0, _PORTA_START_ADDR_, _PORTA_RA3_MASK},	// RA3                                          //82
						{0, GPIO, 0, _PORTJ_START_ADDR_, _PORTJ_RJ10_MASK},	// RJ10						                    //83
						{0, GPIO, 0, _PORTH_START_ADDR_, _PORTH_RH11_MASK},	// RH11                                         //84
						{0, GPIO, 0, _PORTH_START_ADDR_, _PORTH_RH14_MASK},	// RH14                                         //85
						{0, GPIO, 0, _PORTJ_START_ADDR_, _PORTJ_RJ2_MASK},	// RJ2						                    //86
						{0, GPIO | AN, 0, _PORTG_START_ADDR_, _PORTG_RG6_MASK},		// AN14/RG6                             //87
						{0, GPIO | AN | U_TX, 0, _PORTB_START_ADDR_, _PORTB_RB14_MASK},	// AN9/RB14                         //88
						{0, GPIO | AN, 0, _PORTG_START_ADDR_, _PORTG_RG7_MASK},		// AN13/RG7                             //89
						{0, GPIO | AN, 0, _PORTB_START_ADDR_, _PORTB_RB15_MASK},	// AN10/RB15                            //90
						{0, GPIO | AN, 0, _PORTG_START_ADDR_, _PORTG_RG8_MASK},		// AN12/RG8                             //91
						{0, GPIO | AN, 0, _PORTH_START_ADDR_, _PORTH_RH6_MASK},		// AN42/RH6						        //92
						{0, GPIO | AN, 0, _PORTE_START_ADDR_, _PORTE_RE8_MASK},		// AN25/RE8                             //93
                        {0, GPIO | AN, 0, _PORTD_START_ADDR_, _PORTD_RD14_MASK}, 	// AN32/RD14                            //94
						{0, GPIO | AN, 0, _PORTE_START_ADDR_, _PORTE_RE9_MASK},		// AN26/RE9                             //95
                        {0, GPIO | AN, 0, _PORTD_START_ADDR_, _PORTD_RD15_MASK}, 	// AN33/RD15                            //96
						{0, RESERVED, 0, 0, 0}, 	// 32_VDD                                                               //97
						{0, RESERVED, 0, 0, 0}, 	// 32_VDD                                                               //98
						{0, RESERVED, 0, 0, 0},		// gnd                                                                  //99
						{0, RESERVED, 0, 0, 0},		// gnd                                                                  //100
						{0, GPIO | AN, 0, _PORTB_START_ADDR_, _PORTB_RB5_MASK},		// AN45/RB5                             //101
                        {0, GPIO, 0, _PORTF_START_ADDR_, _PORTF_RF3_MASK}, 			// RF3                                  //102
						{0, GPIO | AN, 0, _PORTB_START_ADDR_, _PORTB_RB4_MASK},		// AN4/RB4                              //103
                        {0, GPIO, 0, _PORTF_START_ADDR_, _PORTF_RF2_MASK}, 			// RF2                                  //104
						{0, GPIO | AN, 0, _PORTB_START_ADDR_, _PORTB_RB3_MASK},		// AN3/RB3                              //105
                        {0, GPIO, 0, _PORTF_START_ADDR_, _PORTF_RF8_MASK}, 			// RF8                                  //106
						{0, GPIO | AN | U_RX, 0, _PORTB_START_ADDR_, _PORTB_RB2_MASK},		// AN3/RB2                      //107
                        {0, GPIO, 0, _PORTH_START_ADDR_, _PORTH_RH9_MASK}, 			// RH9                                  //108
						{0, GPIO, 0, _PORTA_START_ADDR_, _PORTA_RA9_MASK},			// VREF-/RA9							//109
                        {0, GPIO, 0, _PORTH_START_ADDR_, _PORTH_RH10_MASK}, 		// RH10                                 //110
						{0, GPIO, 0, _PORTA_START_ADDR_, _PORTA_RA10_MASK},			// VREF+/RA10                           //111
						{0, GPIO, 0, _PORTA_START_ADDR_, _PORTA_RA2_MASK},			// SCL2/RA2                             //112
						{0, GPIO, 0, _PORTH_START_ADDR_, _PORTH_RH0_MASK},			// RH0                                  //113
						{0, GPIO, 0, _PORTD_START_ADDR_, _PORTD_RD10_MASK},			// SCK4/RD10                            //114
						{0, GPIO, 0, _PORTH_START_ADDR_, _PORTH_RH1_MASK},			// RH1                                  //115
						{0, GPIO, 0, _PORTH_START_ADDR_, _PORTH_RH12_MASK},			// RH12					                //116
						{0, GPIO | AN, 0, _PORTB_START_ADDR_, _PORTB_RB10_MASK},	// AN5/RB10                             //117
						{0, GPIO, 0, _PORTD_START_ADDR_, _PORTD_RD1_MASK}, 			// SCK1/RD1                             //118
						{0, GPIO | AN, 0, _PORTB_START_ADDR_, _PORTB_RB11_MASK},	// AN6/RB11                             //119
                        {0, GPIO, 0, _PORTJ_START_ADDR_, _PORTJ_RJ0_MASK}, 			// RJ0						            //120
						{0, GPIO | AN, 0, _PORTB_START_ADDR_, _PORTB_RB12_MASK},	// AN7/RB12                             //121
						{0, GPIO, 0, _PORTD_START_ADDR_, _PORTD_RD7_MASK}, 			// RD7                                  //122
						{0, GPIO | AN, 0, _PORTB_START_ADDR_, _PORTB_RB13_MASK},	// AN8/RB13                             //123
						{0, GPIO | I2C_SCK, 0, _PORTA_START_ADDR_, _PORTA_RA14_MASK},	// SCL1/RA14                        //124
						{0, RESERVED, 0, 0, 0}, 	// nc                                                                   //125
						{0, GPIO | I2C_SDA, 0, _PORTA_START_ADDR_, _PORTA_RA14_MASK},	// SDA1/RA15                        //126
						{0, RESERVED, 0, 0, 0}, 	// 32_VDD                                                               //127
						{0, RESERVED, 0, 0, 0}, 	// 32_VDD                                                               //128
						{0, RESERVED, 0, 0, 0}, 	// gnd                                                                  //129
						{0, RESERVED, 0, 0, 0}, 	// gnd						                                            //130
						{0, RESERVED, 0, 0, 0}, 	// pmd15                                                                //131
						{0, RESERVED, 0, 0, 0}, 	// pmd4												                    //132
						{0, RESERVED, 0, 0, 0}, 	// pmd15                                                                //133
						{0, RESERVED, 0, 0, 0}, 	// pmd3                                                                 //134
						{0, RESERVED, 0, 0, 0}, 	// pmd15                                                                //135
						{0, RESERVED, 0, 0, 0}, 	// pmd2                                                                 //136
						{0, RESERVED, 0, 0, 0}, 	// pmd11                                                                //137
						{0, RESERVED, 0, 0, 0}, 	// pmd1                                                                 //138
						{0, RESERVED, 0, 0, 0}, 	// pmd12                                                                //139
						{0, RESERVED, 0, 0, 0}, 	// pmd0                                                                 //140
						{0, RESERVED, 0, 0, 0}, 	// pmd13                                                                //141
						{0, RESERVED, 0, 0, 0}, 	// pmd4                                                                 //142
						{0, RESERVED, 0, 0, 0}, 	// pmd14                                                                //143
						{0, RESERVED, 0, 0, 0}, 	// pmd4                                                                 //144
						{0, RESERVED, 0, 0, 0}, 	// pmd15                                                                //145
						{0, RESERVED, 0, 0, 0}, 	// pmd4                                                                 //146
						{0, RESERVED, 0, 0, 0}, 	// pmd10                                                                //147
						{0, RESERVED, 0, 0, 0}, 	// pmd10                                                                //148
						{0, RESERVED, 0, 0, 0}, 	// pmd10                                                                //149
						{0, RESERVED, 0, 0, 0}, 	// pmd9                                                                 //150
						{0, RESERVED, 0, 0, 0}, 	// pmd5                                                                 //151
						{0, RESERVED, 0, 0, 0}, 	// pmd8                                                                 //152
						{0, RESERVED, 0, 0, 0}, 	// pmd6                                                                 //153
						{0, RESERVED, 0, 0, 0}, 	// pmd7                                                                 //154
						{0, GPIO, 0, _PORTC_START_ADDR_, _PORTC_RC4_MASK},	// RC4                                          //155
						{0, RESERVED, 0, 0, 0}, 	// nc                                                                   //156
						{0, RESERVED, 0, 0, 0}, 	// 5V EXT                                                               //157
						{0, RESERVED, 0, 0, 0}, 	// 5V EXT                                                               //158
						{0, RESERVED, 0, 0, 0}, 	// gnd                                                                  //159
						{0, RESERVED, 0, 0, 0}, 	// gnd						                                            //160
						{0, RESERVED, 0, 0, 0}, 	// nc                                                                   //161
						{0, RESERVED, 0, 0, 0}, 	// nc                                                                   //162
						{0, GPIO | AN, 0, _PORTB_START_ADDR_, _PORTB_RB14_MASK},	// AN9/RB14                             //163
						{0, RESERVED, 0, 0, 0}, 	// nc                                                                   //164
						{0, GPIO | AN, 0, _PORTB_START_ADDR_, _PORTB_RB5_MASK},	// AN45/RB5						            //165
						{0, GPIO | AN, 0, _PORTG_START_ADDR_, _PORTG_RG8_MASK},	// AN12/RG8						            //166
						{0, GPIO | AN, 0, _PORTG_START_ADDR_, _PORTG_RG6_MASK},	// AN14/RG6                                 //167
						{0, GPIO, 0, _PORTD_START_ADDR_, _PORTD_RD7_MASK},		// RD7                                      //168
#endif						                                                                                                  
                    }                                                                                                         
};



// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


void cmdPinSet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char **argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
        
    //requires both pin# and value
    if(argc != 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM "Usage:- pinset <pin> <0/1>\r\n");
        return;
    }
        
    
    uint32_t pinNum = atoi(argv[1]) - 1;
    uint32_t pinVal = atoi(argv[2]);    
    
    //Check if GPIO support available
    if(!commandsData.pin_map[pinNum].inuse && (commandsData.pin_map[pinNum].support & GPIO))
    {
        *((volatile uint32_t *)((volatile char *)(commandsData.pin_map[pinNum].gpio_reg) + 0x14)) = commandsData.pin_map[pinNum].gpio_mask;
                        
        if(pinVal)
            *((volatile uint32_t *)((volatile char *)(commandsData.pin_map[pinNum].gpio_reg) + 0x28)) = commandsData.pin_map[pinNum].gpio_mask;
        else
            *((volatile uint32_t *)((volatile char *)(commandsData.pin_map[pinNum].gpio_reg) + 0x24)) =  commandsData.pin_map[pinNum].gpio_mask;  
                        
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** SUCCESS ***\r\n");   
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** RESERVED ***\r\n");
    }
       
}
void cmdPinGet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char **argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
        
    //requires both pin# and value
    if(argc != 2 )
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM "Usage:- pinget <pin> \r\n");
        return;
    }
        
    
    uint32_t pinNum = atoi(argv[1]) - 1;
    
    //Check if GPIO support available
    if(commandsData.pin_map[pinNum].support & GPIO)
    {                                
        *((volatile uint32_t *)((volatile char *)(commandsData.pin_map[pinNum].gpio_reg) + 0x18)) = commandsData.pin_map[pinNum].gpio_mask;
        
        bool pin_state = *((volatile uint32_t *)((volatile char *)(commandsData.pin_map[pinNum].gpio_reg) + 0x30)) & commandsData.pin_map[pinNum].gpio_mask ;
        
        (*pCmdIO->pCmdApi->print)(cmdIoParam, LINE_TERM " *** %d ***\r\n", pin_state);   
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** RESERVED ***\r\n");
    }       
}

void cmdPinReset(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char **argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
        
    //requires both pin# and value
    if(argc != 2 )
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM "Usage:- pinreset <pin> \r\n");
        return;
    }
        
    
    uint32_t pinNum = atoi(argv[1]) - 1;
    
    //Check if GPIO support available
    if(commandsData.pin_map[pinNum].inuse)
    {                                
        *((volatile uint32_t *)((volatile char *)(commandsData.pin_map[pinNum].gpio_reg) + 0x18)) = commandsData.pin_map[pinNum].gpio_mask;
        
        commandsData.pin_map[pinNum].inuse = 0;
        
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** SUCCESS ***\r\n");  
        
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** FAIL ***\r\n");
    }     
}



/* ADC Commands*/
void cmdAdcUp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char **argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
        
    //requires both pin# and value
    if(argc != 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM "Usage:- adcup <adcpin>\r\n");
        return;
    }
    
    uint32_t adcpin = atoi(argv[1]) - 1;
    
    //Check if GPIO support available
    if(!commandsData.pin_map[adcpin].inuse && commandsData.pin_map[adcpin].support & AN)
    {
        /* enable in use*/
        commandsData.pin_map[adcpin].inuse = 1;
                
        /* Enable the Analog select bit*/
        *((volatile uint32_t *)((volatile char *)(commandsData.pin_map[adcpin].gpio_reg + 0x08))) = commandsData.pin_map[adcpin].gpio_mask;        
        
        // Make it input by setting the TRIS register
        *((volatile uint32_t *)((volatile char *)(commandsData.pin_map[adcpin].gpio_reg) + 0x18)) = commandsData.pin_map[adcpin].gpio_mask;       
                
        switch(commandsData.pin_map[adcpin].adc_channel)
        {
            case ADCHS_CH3:
            {
                /* ADC 3 */
        
                ADCANCONbits.ANEN3 = 1;      // Enable the clock to analog bias
                while(ADCANCONbits.WKRDY3 == 0U) // Wait until ADC is ready
                {
                    /* Nothing to do */
                }
                ADCCON3bits.DIGEN3 = 1;      // Enable ADC
            }
            break;
            
            default:
                (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** INVALID PIN ***\r\n");
                break;
        }
                
        TMR3_Start();                    
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** SUCCESS ***\r\n");           
        
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** RESERVED ***\r\n");
    }
    
    
}


void cmdAdcGet(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char **argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
        
    //requires both pin# and value
    if(argc != 2)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM "Usage:- adcget <adcget>\r\n");
        return;
    }
    
    uint32_t adcpin = atoi(argv[1]) - 1;
    
    //Check if GPIO support available
    if(commandsData.pin_map[adcpin].inuse && commandsData.pin_map[adcpin].support & AN)
    {
        if(ADCHS_ChannelResultIsReady(commandsData.pin_map[adcpin].adc_channel))
        {
            uint16_t adcCnt = ADCHS_ChannelResultGet(commandsData.pin_map[adcpin].adc_channel);
            
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "ADC Count = %d\r\n", adcCnt); 
            
        }
    
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** SUCCESS ***\r\n");   
        
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** RESERVED ***\r\n");
    }
    
    
    
    
}

/* I2C Commands*/
void cmdI2cUp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char **argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
        
    //requires both pin# and value
    if(argc != 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM "Usage:- i2cup <scl pin> <sda pin>\r\n");
        return;
    }
    
}
void cmdI2cWrite(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char **argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
        
    //requires both pin# and value
    if(argc != 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM "Usage:- i2cwr <adcpin>\r\n");
        return;
    }
    
}

void cmdI2cRead (SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char **argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
        
    //requires both pin# and value
    if(argc != 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM "Usage:- i2crd <adcpin>\r\n");
        return;
    }
    
}
void cmdI2cDown(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char **argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
        
    //requires both pin# and value
    if(argc != 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM "Usage:- i2cdown <adcpin>\r\n");
        return;
    }
    
}

void cmdUartUp(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char **argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
        
    //requires both pin# and value
    if(argc < 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM "Usage:- uartup <txpin> <rxpin> [baud rate] [rd size] [rd size]\r\n");
        return;
    }
        
    
    uint32_t txpin = atoi(argv[1]) - 1;
    uint32_t rxpin = atoi(argv[2]) - 1;

    
    if(argc > 3)
    {
        uint32_t baud = atoi(argv[3]);
    
        UART_SERIAL_SETUP uSetup;

        uSetup.baudRate = baud;
        uSetup.dataWidth = UART_DATA_8_BIT;
        uSetup.parity = UART_PARITY_NONE;
        uSetup.stopBits = 1;
        
        if(argc > 4)
            uSetup.rdBuffSz = atoi(argv[4]);
        
        if(argc > 5)
            uSetup.wrBuffSz = atoi(argv[5]);

        UART1_SerialSetup(&uSetup, 0);
    }
	
    //Check if GPIO support available
    if(commandsData.pin_map[txpin].support & U_TX &&
            commandsData.pin_map[rxpin].support & U_RX && !commandsData.pin_map[txpin].inuse && !commandsData.pin_map[rxpin].inuse)
    {
        // make PPS for UART2
        /* Unlock system for PPS configuration */
        SYSKEY = 0x00000000U;
        SYSKEY = 0xAA996655U;
        SYSKEY = 0x556699AAU;
                
        CFGCONbits.IOLOCK = 0U;
        // output pin
        
        *((volatile uint32_t *)((volatile char *)(commandsData.pin_map[txpin].pps_reg_val))) = 1;        
//        RPF5R = 1;

        // Make it input by setting the TRIS register
        *((volatile uint32_t *)((volatile char *)(commandsData.pin_map[rxpin].gpio_reg) + 0x18)) = commandsData.pin_map[rxpin].gpio_mask;       
                
        // input pin
        U1RXR = commandsData.pin_map[rxpin].pps_reg_val;
        //U1RXR = 2;
        
        /* Lock back the system after PPS configuration */
        CFGCONbits.IOLOCK = 1U;

        SYSKEY = 0x00000000U;
        

        /* enable in use*/
        commandsData.pin_map[txpin].inuse = 1;
        
        commandsData.pin_map[rxpin].inuse = 1;
        
        
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** SUCCESS ***\r\n");           
        
    }
    else
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** RESERVED ***\r\n");
    }
	
    
}

void cmdUartDown(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char **argv)
{


}


void cmdUartread(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char **argv)
{
    //	static int printBuffPtr = 0;
    uint8_t tmpBuf[128];
    
    const void* cmdIoParam = pCmdIO->cmdIoParam;
        
    //requires both pin# and value
    if(argc != 3)
    {
        (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM "Usage:- uartrd <idx> <bytes>\r\n");
        return;
    }
        
    
    uint32_t uartIdx = atoi(argv[1]) - 1;
    uint32_t readBytes = atoi(argv[2]) - 1;
	
    uartIdx = uartIdx;
    
    size_t rLen = UART1_Read(tmpBuf, (readBytes > 128)?128:readBytes);    
    tmpBuf[rLen] = 0;
    
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, (const char *)tmpBuf );   
    
    
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** SUCCESS ***\r\n");   
    
}


void cmdUartWrite(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char **argv)
{
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    
    for(int i = 2; i < argc; i++)
        UART1_Write((uint8_t*)argv[i], strlen(argv[i]));

    
    UART1_Write((uint8_t*)LINE_TERM, sizeof(LINE_TERM));
    
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, LINE_TERM " *** SUCCESS ***\r\n");   
    
    
}

static const SYS_CMD_DESCRIPTOR    moduleCmdsTbl[]=
{
    /* GPIO Commands */
    {"pinset",   cmdPinSet,   ": Sets the given pin state\r\nExample:- pinset <pin> <0/1>\r\n"},
    {"pinget",   cmdPinGet,   ": Gets the given pin state\r\nExample:- pinget <pin>\r\n"},   
    {"pinreset",   cmdPinReset,   ": Resets the given pin state\r\nExample:- pinreset <pin>\r\n"},   
    
    /* UART Commands */
    {"uartup",   cmdUartUp,   ": Brings up the given UART\r\nExample:- uartUp <txpin> <rxpin> [baud rate] [max size]\r\n"},   
    {"uartdown", cmdUartDown,   ": Brings down the given UART\r\nExample:- uartDown <txpin> <rxpin>\r\n"},   
    {"uartrd",   cmdUartread,   ": Reads from the given UART\r\nExample:- uartrd <idx>\r\n"},   
    {"uartwr",   cmdUartWrite,   ": Writes to the given UART\r\nExample:- uartwr <idx> <data>\r\n"},   
    
    /* ADC Commands */
    {"adcup",   cmdAdcUp,   ": Brings Up the given ADC interface\r\nExample:- adcup <adcpin>\r\n"},   
    {"adcget",   cmdAdcGet,   ": Reads the given ADC pin\r\nExample:- adcget <adcpin>\r\n"},   
    /* I2C Commands */
    {"i2cup",   cmdI2cUp,   ": Brings up the given UART\r\nExample:- i2cUp <scl pin> <sda pin>\r\n"},   
    {"i2cdown", cmdI2cDown,   ": Brings down the given UART\r\nExample:- i2cDown <txpin> <rxpin>\r\n"},   
    {"i2crd",   cmdI2cRead,   ": Reads from the given UART\r\nExample:- i2crd <idx>\r\n"},   
    {"i2cwr",   cmdI2cWrite,   ": Writes to the given UART\r\nExample:- i2cwr <idx> <data>\r\n"},   
    
    
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
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void COMMANDS_Initialize ( void )

  Remarks:
    See prototype in commands.h.
 */

void COMMANDS_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    commandsData.state = COMMANDS_STATE_INIT;
      
//    SYS_CONSOLE_PRINT("%X %X\r\n", DEVSN0, DEVSN1);
        
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void COMMANDS_Tasks ( void )

  Remarks:
    See prototype in commands.h.
 */

void COMMANDS_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( commandsData.state )
    {
        /* Application's initial state. */
        case COMMANDS_STATE_INIT:
        {
            bool appInitialized = true;


            if (appInitialized)
            {
//                SYS_CONSOLE_MESSAGE(BANNER);
                if(SYS_CMD_ADDGRP(moduleCmdsTbl, sizeof(moduleCmdsTbl)/sizeof(*moduleCmdsTbl),
                        "test", ": Module test commands") == false)                
                {
                    SYS_CONSOLE_MESSAGE("Failed to added the console commands\r\n");
                }
                commandsData.state = COMMANDS_STATE_SERVICE_TASKS;
            }
            break;
        }

        case COMMANDS_STATE_SERVICE_TASKS:
        {

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
