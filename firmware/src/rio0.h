/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _EXAMPLE_FILE_NAME_H    /* Guard against multiple inclusion */
#define _EXAMPLE_FILE_NAME_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


    /* ************************************************************************** */
    /* ************************************************************************** */
    /* Section: Constants                                                         */
    /* ************************************************************************** */
    /* ************************************************************************** */

    /*  A brief description of a section can be given directly below the section
        banner.
     */


    /* ************************************************************************** */
    /** Descriptive Constant Name

      @Summary
        Brief one-line summary of the constant.
    
      @Description
        Full description, explaining the purpose and usage of the constant.
        <p>
        Additional description in consecutive paragraphs separated by HTML 
        paragraph breaks, as necessary.
        <p>
        Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.
    
      @Remarks
        Any additional remarks
     */


#define RIO0_FW_ROM_DST_OFFSET  6  
    
#define RIO0_FW_IMG_SIZE_OFFSET 7   
    
#define RIO0_ENABLE_REFCLK		1

#define EXAMPLE_CONSTANT 0
    
#define RIO0_SYSKEY             0x1F800080

#define RIO0_RFSPICTL           0x1F8C8020
#define RIO0_RFSPICTL_SET       0x1F8C8028
#define RIO0_EN_BIT_BANG        0x80000000
#define RIO0_CS_HIGH            0x00000002 
#define RIO0_CLK_HIGH           0x00000001
    
    
#define RIO0_PPS_RPA9R_SET      0x1F801A24
    
    
#define RIO0_REFO1CON           0x1F8012A0    
    
#define RIO0_REFO1CON_SET       0x1F8012A8        
    
#define RIO0_REFO2CON           0x1F8012C0
    
#define RIO0_REFO2CON_SET       0x1F8012C8

#define RIO0_EWPLL_PWRON        0x808
    
#define RIO0_CLKDIAG            0x1F8013B0
        
#define RIO0_CLKDIAG_CLR        0x1F8013B4
    
#define RIO0_SPLL_CON           0x1F801220
    
#define RIO0_SPLL_CON_CLR       0x1F801224
    
#define RIO0_SPLL_CON_SET       0x1F801228
    
    
#define RIO0_CFGCON0_CLR        0x1F800004    
    
#define RIO0_CFGCON3            0x1F800030        
    
#define RIO0_CFGCON3_CLR        0x1F800034    
    
#define RIO0_CFGCON3_SET        0x1F800038    
    
#define RIO0_CFGCON2            0x1F800020

#define RIO0_CFGCON2_CLR        0x1F800024
    
#define RIO0_CFGCON2_SET        0x1F800028

#define RIO0_OSCCON             0x1F801200    
    
#define RIO0_OSCCON_CLR         0x1F801204
    
#define RIO0_OSCCON_SET         0x1F801208 
    
#define RIO0_CLKSTAT            0x1F801390
    
#define RIO0_PB1DIV             0x1F801320

#define RIO0_PWRCONSET			0x1F801298

    
void RIO0_SYS_Initialize(uint32_t devId);

void RIO0_SYS_Initialize_bkp(uint32_t devId);

void RIO0_FLASH_Initialize(uint32_t devId);

uint32_t RIO0_FLASH_CHIP_Erase(uint32_t devId);

uint32_t RIO0_FLASH_Reset(uint32_t devId);

uint32_t RIO0_FLASH_ID_Read(uint32_t devId);

uint32_t RIO0_FLASH_PAGE_Write(uint32_t devId, uint32_t address, uint8_t *data);

uint32_t RIO0_FLASH_Status(uint32_t devId);


    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
