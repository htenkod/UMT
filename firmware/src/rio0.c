/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include "definitions.h"


 void wifi_spi_write(uint32_t devId, unsigned int spi_addr, unsigned int data)
{
    unsigned int  addr_bit = 0, data_bit = 0, bit_idx = 0;

    wReg32(devId, RIO0_RFSPICTL, (RIO0_EN_BIT_BANG | RIO0_CS_HIGH));
    wReg32(devId, RIO0_RFSPICTL, (RIO0_EN_BIT_BANG | RIO0_CS_HIGH | RIO0_CLK_HIGH ));
    wReg32(devId, RIO0_RFSPICTL, (RIO0_EN_BIT_BANG));
    wReg32(devId, RIO0_RFSPICTL, (RIO0_EN_BIT_BANG | RIO0_CLK_HIGH));
    

    for (bit_idx=0;bit_idx<=7;bit_idx++) {
        addr_bit = (spi_addr>>(7-bit_idx)) & 0x1;
        // Falling edge of clk
        wReg32(devId, RIO0_RFSPICTL, (RIO0_EN_BIT_BANG | (addr_bit << 2 )));               
        // Rising edge of clk
        wReg32(devId, RIO0_RFSPICTL, (RIO0_EN_BIT_BANG | (addr_bit << 2 ) | RIO0_CLK_HIGH));    
    }

    for (bit_idx=0;bit_idx<=15;bit_idx++) {
        data_bit = (data>>(15-bit_idx)) & 0x1;
        // Falling edge of clk with data bit
        wReg32(devId, RIO0_RFSPICTL, (RIO0_EN_BIT_BANG | (data_bit << 2 )));                
        // Rising edge of clk
        wReg32(devId, RIO0_RFSPICTL, (RIO0_EN_BIT_BANG | (data_bit << 2 ) | RIO0_CLK_HIGH));     
    }

    // Rising edge of clk
    wReg32(devId, RIO0_RFSPICTL, (RIO0_EN_BIT_BANG | RIO0_CS_HIGH | RIO0_CLK_HIGH)); 
    // Set the RF override bit and CS_n high
    wReg32(devId, RIO0_RFSPICTL, 0);                                
}

unsigned int wifi_spi_read(uint32_t devId, unsigned int spi_addr)
{
    unsigned int  addr_bit = 0, bit_idx, read_data, cmd_high = 0x08;
    cmd_high = 0x8;
    
    // Set the RF override bit and CS_n high
    wReg32(devId, RIO0_RFSPICTL, (RIO0_EN_BIT_BANG | RIO0_CS_HIGH));
    // Rising edge of clk
    wReg32(devId, RIO0_RFSPICTL, (RIO0_EN_BIT_BANG | RIO0_CS_HIGH | RIO0_CLK_HIGH ));
    // Falling edge of clk with CS going low and command bit 1
    wReg32(devId, RIO0_RFSPICTL, (RIO0_EN_BIT_BANG | cmd_high | (0x1 << 2)));
    // Falling edge of clk with CS going low and command bit 1
    wReg32(devId, RIO0_RFSPICTL, (RIO0_EN_BIT_BANG | cmd_high | (0x1 << 2) | RIO0_CLK_HIGH));    

    for (bit_idx=0;bit_idx<=7;bit_idx++) {
        addr_bit = (spi_addr>>(7-bit_idx)) & 0x1;
        // Falling edge of clk
        wReg32(devId, RIO0_RFSPICTL, ((RIO0_EN_BIT_BANG | cmd_high | (addr_bit << 2 ))));               
        // Rising edge of clk
        wReg32(devId, RIO0_RFSPICTL, ((RIO0_EN_BIT_BANG | cmd_high | (addr_bit << 2 ) | RIO0_CLK_HIGH)));    
    }

    for (bit_idx=0;bit_idx<=16;bit_idx++) {
        // Falling edge of clk
        wReg32(devId, RIO0_RFSPICTL, RIO0_EN_BIT_BANG | cmd_high );             
        // Rising edge of clk
        wReg32(devId, RIO0_RFSPICTL, (RIO0_EN_BIT_BANG | cmd_high | RIO0_CLK_HIGH));     
    }

    // Set the RF override bit and CS_n high
    wReg32(devId, RIO0_RFSPICTL, 0);                              

    //soc_reg_rd(0xBF8C8130,15,0) & 0xFFFF;
    read_data = rReg32(devId, RIO0_RFSPICTL); 
    return read_data;
}



static void posc_xtal_startup_config_before(uint32_t devId)
{
    /* make sure we properly reset SPI to a known state */
    wReg32(devId, RIO0_RFSPICTL_SET, 0x80000022);
    /* make sure we properly take out of reset */
    wReg32(devId, RIO0_RFSPICTL_SET, 0x80000002);

    // Read counter used only for debug and testing
    /* MBIAS filter and A31 analog_test */ //if (wifi_spi_read (0x85) != 0xF0) {Error, Stop};
    wifi_spi_write(devId, 0x85, 0x00F2); 
    
    /* A31 Analog test */// if (wifi_spi_read (0x84) != 0x1) {Error, Stop};
    wifi_spi_write(devId, 0x84, 0x0001); 
    
    /* MBIAS reference adjustment */ //if (wifi_spi_read (0x1e) != 0x510) {Error, Stop};
    wifi_spi_write(devId, 0x1e, 0x510); 
    
    /* XTAL LDO feedback divider (1.3+v) */ //if (wifi_spi_read (0x82) != 0x6000) {Error, Stop};
    wifi_spi_write(devId, 0x82, 0x6000); 
    

}
static void posc_xtal_startup_config_after(uint32_t devId)
{
    /* xtal LDO feedback divider (1.3+ v) */
    wifi_spi_write(devId, 0x85, 0x00F4); 


	
}

uint32_t RIO0_FLASH_PAGE_Write(uint32_t devId, uint32_t address, uint8_t *data)
{
    volatile uint32_t statusReg = 0;    
    /* Write Enable */    
//    wReg32(devId, 0x1F820130, 0x00000100);//'CE 10
	wReg32(devId, 0x1F820130, 0x00000000);//'CE 00    
	wReg32(devId, 0x1F801620, 0x00000006);//dev ID     
//	wReg32(devId, 0x1F820130, 0x00000000);//'CE 00
	wReg32(devId, 0x1F820130, 0x00000100);//'CE 01
    
    
    /* Bit 1 is WEL write enable latch*/
    while((statusReg & 0x02) != 2) 
        statusReg = RIO0_FLASH_Status(devId);

        /* Page program */    
//  wReg32(devId, 0x1F820130, 0x00000100);//'CE 10
	wReg32(devId, 0x1F820130, 0x00000000);//'CE 00    
	wReg32(devId, 0x1F801620, 0x00000002);//dev ID     
    
    wReg32(devId, 0x1F801620, ((address >> 16) & 0xFF)); 
    wReg32(devId, 0x1F801620, ((address >> 8) & 0xFF));
    wReg32(devId, 0x1F801620, (address & 0xFF));
    
    /* Switch to 32 bit mode */
//	wReg32(devId, 0x1F801604, 0x00008000); //'SPI0CON SET OFF
//	wReg32(devId, 0x1F801600, 0x00800B20); //'SPI0CON setup 32bit mode
//  wReg32(devId, 0x1F801630, 0x00000000);  //SPI0BRG setup
//	wReg32(devId, 0x1F801608, 0x00008000); //'SPI0CON SET ON

//    wReg32(devId, 0x1F801620, (0x02000000 + address));

	for(unsigned long idx = 0; idx < 256; idx++)
	{
//        wReg32(devId, 0x1F801604, 0x00008000);  //SPI0CON SET OFF
//        wReg32(devId, 0x1F801608, 0x00008000);  //SPI0CON SET ON
		wReg32(devId, 0x1F801620, data[idx]);		
	}

//	wReg32(devId, 0x1F820130, 0x00000000);//'CE 01
	wReg32(devId, 0x1F820130, 0x00000100);//'CE 01    
    
//	wReg32(devId, 0x1F801604, 0x00008000);//'SPI0CON SET OFF
//	wReg32(devId, 0x1F801600, 0x00800320);//'SPI0CON setup
//	wReg32(devId, 0x1F801608, 0x00008000);//'SPI0CON SET ON
    
    /* Bit 0 is WIP write in progress */
    while((statusReg & 0x03) != 0) 
        statusReg = RIO0_FLASH_Status(devId);

    return 0;
}


uint32_t RIO0_FLASH_Status(uint32_t devId)
{
    uint32_t status = 0;
//    wReg32(devId, 0x1F820130, 0x00000100); //'CE 10
    wReg32(devId, 0x1F820130, 0x00000000); //'CE 10    
    wReg32(devId, 0x1F801620, 0x00000005); //'command
    wReg32(devId, 0x1F801604, 0x00008000);  //SPI0CON SET OFF
    wReg32(devId, 0x1F801608, 0x00008000);  //SPI0CON SET ON
    wReg32(devId, 0x1F801620, 0x00000000);//    
    status = rReg32(devId, 0x1F801620);
//    SYS_CONSOLE_PRINT("S-Reg 0x%X= \r\n", status);
//    wReg32(devId, 0x1F820130, 0x00000000); //'CE 01
    wReg32(devId, 0x1F820130, 0x00000100); //'CE 01
    
    return status;
}


uint32_t RIO0_FLASH_Reset(uint32_t devId)
{    

    wReg32(devId, 0x1F820130, 0x00000100); //'CE 10
    wReg32(devId, 0x1F820130, 0x00000000); //'CE 10    
    wReg32(devId, 0x1F801620, 0x00000066); //'command
    wReg32(devId, 0x1F801604, 0x00008000);  //SPI0CON SET OFF
    wReg32(devId, 0x1F801608, 0x00008000);  //SPI0CON SET ON
    wReg32(devId, 0x1F820130, 0x00000000); //'CE 01
    wReg32(devId, 0x1F820130, 0x00000100); //'CE 01
    
    wReg32(devId, 0x1F820130, 0x00000100); //'CE 10
    wReg32(devId, 0x1F820130, 0x00000000); //'CE 10    
    wReg32(devId, 0x1F801620, 0x00000099); //'command
    wReg32(devId, 0x1F801604, 0x00008000);  //SPI0CON SET OFF
    wReg32(devId, 0x1F801608, 0x00008000);  //SPI0CON SET ON
    wReg32(devId, 0x1F820130, 0x00000000); //'CE 01
    wReg32(devId, 0x1F820130, 0x00000100); //'CE 01
    
   
    return 0;
}

uint32_t RIO0_FLASH_CHIP_Erase(uint32_t devId)
{
    /*- Chip erase time: 6 seconds typical */
    /* Write Enable */    
    
    wReg32(devId, 0x1F820130, 0x00000100);//'CE 10
	wReg32(devId, 0x1F820130, 0x00000000);//'CE 00    
	wReg32(devId, 0x1F801620, 0x00000006);//dev ID    
    wReg32(devId, 0x1F801604, 0x00008000);  //SPI0CON SET OFF
    wReg32(devId, 0x1F801608, 0x00008000);  //SPI0CON SET ON  
	wReg32(devId, 0x1F820130, 0x00000000);//'CE 00
	wReg32(devId, 0x1F820130, 0x00000100);//'CE 01
    
    volatile uint32_t statusReg = 0;    
    /* Bit 2 is WEL write enable latch*/
    while((statusReg & 0x02) != 2) 
        statusReg = RIO0_FLASH_Status(devId);
    
    
    wReg32(devId, 0x1F820130, 0x00000100);//'CE 10
	wReg32(devId, 0x1F820130, 0x00000000);//'CE 00    
	wReg32(devId, 0x1F801620, 0x000000C7);//dev ID    
    wReg32(devId, 0x1F801604, 0x00008000);  //SPI0CON SET OFF
    wReg32(devId, 0x1F801608, 0x00008000);  //SPI0CON SET ON  
	wReg32(devId, 0x1F820130, 0x00000000);//'CE 00
	wReg32(devId, 0x1F820130, 0x00000100);//'CE 01 
    
    /* Bit 2 is WIP Write In Progress*/
    
    while((statusReg & 0x03) != 0) 
        statusReg = RIO0_FLASH_Status(devId);
    
//    SYS_CONSOLE_MESSAGE("Flash Erase Successful!\r\n");
    return 0;
    
}

uint32_t RIO0_FLASH_ID_Read(uint32_t devId)
{    
    uint8_t manId = 0, memType = 0, capSize = 0;
	
	wReg32(devId, 0x1F820130, 0x00000100);//'CE 10
	wReg32(devId, 0x1F820130, 0x00000000);//'CE 00    
	wReg32(devId, 0x1F801620, 0x0000009F);//dev ID    
    wReg32(devId, 0x1F801604, 0x00008000);  //SPI0CON SET OFF
    wReg32(devId, 0x1F801608, 0x00008000);  //SPI0CON SET ON
    wReg32(devId, 0x1F801620, 0x00000000);//dev ID    
    manId = rReg32(devId, 0x1F801620);      
    wReg32(devId, 0x1F801604, 0x00008000);  //SPI0CON SET OFF
    wReg32(devId, 0x1F801608, 0x00008000);  //SPI0CON SET ON
    wReg32(devId, 0x1F801620, 0x00000000);//dev ID        
    memType = rReg32(devId, 0x1F801620);     
    wReg32(devId, 0x1F801604, 0x00008000);  //SPI0CON SET OFF
    wReg32(devId, 0x1F801608, 0x00008000);  //SPI0CON SET ON
    wReg32(devId, 0x1F801620, 0x00000000);//dev ID    
    capSize = rReg32(devId, 0x1F801620);            
    
	wReg32(devId, 0x1F820130, 0x00000000);//'CE 01
	wReg32(devId, 0x1F820130, 0x00000100);//'CE 01    
    
	return (manId << 16 | memType << 8 | capSize);
		
}

    
void RIO0_FLASH_Initialize(uint32_t devId)
{  
    /* Enable 3.3V supply for stacked SPI0 die */
    wReg32(devId, RIO0_PWRCONSET, 0x00000101);
    
	wReg32(devId, 0x1F800094, 0x00000001);  //PMD1 CLR
	wReg32(devId, 0x1F820104, 0x00000100);  //ANSELB CLR
	wReg32(devId, 0x1F820138, 0x00000100);  //LATB CLR
	wReg32(devId, 0x1F820144, 0x00000100);  //ODCB CLR
	wReg32(devId, 0x1F820184, 0x00000100);  //CNENB CLR
	wReg32(devId, 0x1F820164, 0x00000100);  //CNPDB CLR
	wReg32(devId, 0x1F820154, 0x00000100);  //CNPUB CLR
	wReg32(devId, 0x1F820114, 0x00000100);  //TRISB CLR
	wReg32(devId, 0x1F801600, 0x00000320);  //SPI0CON setup
	wReg32(devId, 0x1F801640, 0x00000000);  //SPI0CON2 setup
	wReg32(devId, 0x1F801630, 0x00000000);  //SPI0BRG setup
	wReg32(devId, 0x1F801608, 0x00008000);  //SPI0CON SET ON
	wReg32(devId, 0x1F820130, 0x00000100);  //CE init
	wReg32(devId, 0x1F820130, 0x00000100);  //CE init
	wReg32(devId, 0x1F820130, 0x00000100);  //CE init
	wReg32(devId, 0x1F820130, 0x00000100);  //CE init

}



void RIO0_SYS_Initialize(uint32_t devId)
{                      
    wReg32(devId, RIO0_CFGCON0_CLR, 0x0000C000); // CFGCON0 CLR CFGLOCK
    wReg32(devId, RIO0_CFGCON0_CLR, 0x00003000); // CFGCON0 CLR IOLOCK/PMDLOCK   
    
        /* Disable POSC */
    wReg32(devId, RIO0_CFGCON2_SET, 0x300);  
    CORETIMER_DelayMs(1);
    
    /* Enable POSC */
    wReg32(devId, RIO0_CFGCON2_CLR, 0x300);   
    
    CORETIMER_DelayUs(1);
    
#ifdef RIO0_ENABLE_REFCLK
    wReg32(devId, 0x1F820004, 0x00000200);  //'ANSELACLR 'RA9 set to digital
    wReg32(devId, 0x1F820034, 0x00000200);  //'LATACLR 'RA9 to a zero to start
    wReg32(devId, 0x1F820014, 0x00000200);  //'TRISACLR 'RA9 set to output pin
    wReg32(devId, 0x1F801A24, 0x00000003);  //'RPA9R 'set PORTA PIN 9 to be used by REF2O module
#endif        
    
    wReg32(devId, RIO0_CFGCON2_CLR, 0x8C009000); 
    /* Software Clock Switching Enabled */
    wReg32(devId, RIO0_CFGCON2_SET, 0x00004000); 
    
    wReg32(devId, RIO0_PB1DIV, 0x00008000);
    /* Disable POSC */
    wReg32(devId, RIO0_CFGCON2_SET, 0x300);   
    
    CORETIMER_DelayUs(1);

	posc_xtal_startup_config_before(devId);
    CORETIMER_DelayUs(1);    
	
    /* Enable POSC */
    wReg32(devId, RIO0_CFGCON2_CLR, 0x300);   
    
    CORETIMER_DelayUs(1);
    
    while ((rReg32(devId, RIO0_CLKSTAT) & 0x4) == 0x0)
        ;
        
	posc_xtal_startup_config_after(devId);
    
    /* make sure all plls and posc is not stopped    */
    wReg32(devId, RIO0_CLKDIAG_CLR, 0xF5);       
    
    while ((rReg32(devId, RIO0_CLKSTAT) & 0x4) == 0x0)
    ;

    /* SYS Unlock */    
    do
    {
        wReg32(devId, RIO0_SYSKEY, 0x00000000);
        wReg32(devId, RIO0_SYSKEY, 0xAA996655);
        wReg32(devId, RIO0_SYSKEY, 0x556699AA);
    }
    while(rReg32(devId, RIO0_SYSKEY) != 0x00000001 );

    wReg32(devId, RIO0_CFGCON3, 0x8F0F8F3C);
    
	wReg32(devId, RIO0_SPLL_CON, 0x0996100F); //  0x01C85008 0x0996100F
    
//    wReg32(devId, RIO0_CFGCON3_CLR, 0x800080FF);                
//    wReg32(devId, RIO0_CFGCON3_SET, 0x06000014);
    
    
        
//    while ((rReg32(devId, RIO0_CLKSTAT) & 0x1904) != 0x1904)
//        ;
   
	wReg32(devId, RIO0_OSCCON_SET, 0x00000100);
	wReg32(devId, RIO0_OSCCON_SET, 0x00000001);

//    while ((rReg32(devId, RIO0_OSCCON) & 0xF000) != 0x1000)
//        ;
   
#ifdef RIO0_ENABLE_REFCLK	
    wReg32(devId, 0x1F8000A4, 0x20000000);  //'PMD2CLR ' make sure REF2O module is enabled
    wReg32(devId, RIO0_REFO2CON_SET, 0x00281001);
    wReg32(devId, RIO0_REFO2CON_SET, 0x00008000);
#endif
        
}



void RIO0_SYS_Initialize_bkp(uint32_t devId)
{
    
    /* SYS Unlock */
    wReg32(devId, RIO0_SYSKEY, 0x00000000);
    wReg32(devId, RIO0_SYSKEY, 0xAA996655);
    wReg32(devId, RIO0_SYSKEY, 0x556699AA);
    
//    //Posc_xtal_startup_config_before
//    wReg32(devId, 0x1F8C8028, 0x80000022);//RFSPICTL 'make sure we properly reset SPI to a known state
//    wReg32(devId, 0x1F8C8028, 0x80000002);//RFSPICTL 'make sure we properly take out of reset
    
    posc_xtal_startup_config_before(devId);
    wReg32(devId, 0x1F800020, 0x7F7FFC38);//Enable POSC
    posc_xtal_startup_config_after(devId);

    //--------------- Rio0 initial setup, includes sys pll setting for 200 Mhz Core speed and 40Mhz SPI0
    wReg32(devId, 0x1F820004, 0x0000021D); //CORETIMER_DelayUs(1);//'ANSELACLR 'RA9 set to digital
    wReg32(devId, 0x1F820034, 0x0000021D); //CORETIMER_DelayUs(1);//'LATACLR 'RA9 to a zero to start
    wReg32(devId, 0x1F820014, 0x0000021D); //CORETIMER_DelayUs(1);//'TRISACLR 'RA9 set to output pin

    wReg32(devId, 0x1F8000A4, 0x20000000); //CORETIMER_DelayUs(1);//'PMD2CLR ' make sure REF2O module is enabled   
    wReg32(devId, 0x1F801A24, 0x00000003); //CORETIMER_DelayUs(1);//'RPA9R 'set PORTA PIN 9 to be used by REF2O module
    wReg32(devId, 0x1F8012C0, 0x00000000); //CORETIMER_DelayUs(1);//'REFO2CON 'REF2O is used for output PLL frequency
    wReg32(devId, 0x1F800020, 0x7F7FFC38); //CORETIMER_DelayUs(1);//'enable POSC

    wReg32(devId, 0x1F801224, 0x08000000); //CORETIMER_DelayUs(1);//'SPLLCONCLR 'clear SPLLICLK to set POSC40MHz as input FREQ
    wReg32(devId, 0x1F801224, 0x07000000); //CORETIMER_DelayUs(1);//'SPLLCONCLR 'clear SPLLBWSEL
    wReg32(devId, 0x1F801224, 0x00FF0000); //CORETIMER_DelayUs(1);//'SPLLCONCLR 'clear SPLLFBDIV
    wReg32(devId, 0x1F801224, 0x00007000); //CORETIMER_DelayUs(1);//'SPLLCONCLR 'clear SPLLREFDIV
    wReg32(devId, 0x1F801224, 0x0000003F); //CORETIMER_DelayUs(1);//'SPLLCONCLR 'clear SPLL1POSTDIV
    wReg32(devId, 0x1F801224, 0x00000080); //CORETIMER_DelayUs(1);//'SPLLCONCLR 'clear SPLL1FRC to set SPLL VCO Post Divider output is used to drive spll1_clk
    wReg32(devId, 0x1F801228, 0x01C85008); //CORETIMER_DelayUs(1);//'SPLLCONSET 'setting for 200 MHz
    wReg32(devId, 0x1F801224, 0x00000200); //CORETIMER_DelayUs(1);//'SPLLCONCLR 'turn on SPLL
    wReg32(devId, 0x1F801320, 0x00008001); //CORETIMER_DelayUs(1);//'PB1DIV
    wReg32(devId, 0x1F800034, 0x000000FF); //CORETIMER_DelayUs(1);//'CFGCON3CLR
    wReg32(devId, 0x1F800038, 0x00000014); //CORETIMER_DelayUs(1);//'CFGCON3SET  'SPLL2 = (SYSPLL->200MHz/2) / 0x14 = 80MHz
    wReg32(devId, 0x1F801208, 0x00000100); //CORETIMER_DelayUs(1);//'OSCCONSET request switch to sys pll
    wReg32(devId, 0x1F801208, 0x00000001); //CORETIMER_DelayUs(1);//'OSCCONSET execute request
    // you can monitor the clock based on what pin used as set above
    wReg32(devId, 0x1F8012C0, 0x00289000); //CORETIMER_DelayUs(1);//'REFO2CON 'SPLL2 expect around 1 MHz  setting of REFO3CON divider set to 80 (80*1 MHz = 80 Mhz)
    //--------------- Rio0 initial setup, includes sys pll setting for 200 Mhz Core speed and 40Mhz SPI0
//
//    //--------------- SPI0 related settings
    wReg32(devId, 0x1F801298, 0x00000101); //CORETIMER_DelayUs(1);//PWRCONSET SPI0 monitor and power
//    //wReg32(devId, 0x1F801290, 0x00000101); //PWRCON SPI0 monitor and power
    
    wReg32(devId, 0x1F800094, 0x00000001); //CORETIMER_DelayUs(1);//PMD1 CLR
    wReg32(devId, 0x1F820104, 0x00000100); //CORETIMER_DelayUs(1);//ANSELB CLR
    wReg32(devId, 0x1F820138, 0x00000100); //CORETIMER_DelayUs(1);//LATB CLR
    wReg32(devId, 0x1F820144, 0x00000100); //CORETIMER_DelayUs(1);//ODCB CLR
    wReg32(devId, 0x1F820184, 0x00000100); //CORETIMER_DelayUs(1);//CNENB CLR
    wReg32(devId, 0x1F820164, 0x00000100); //CORETIMER_DelayUs(1);//CNPDB CLR
    wReg32(devId, 0x1F820154, 0x00000100); //CORETIMER_DelayUs(1);//CNPUB CLR
    wReg32(devId, 0x1F820114, 0x00000100); //CORETIMER_DelayUs(1);//TRISB CLR
    wReg32(devId, 0x1F801600, 0x00000320); //CORETIMER_DelayUs(1);//SPI0CON setup
    wReg32(devId, 0x1F801640, 0x00000000); //CORETIMER_DelayUs(1);//SPI0CON2 setup
    wReg32(devId, 0x1F801630, 0x00000000); //CORETIMER_DelayUs(1);//SPI0BRG setup
    wReg32(devId, 0x1F801608, 0x00008000); //CORETIMER_DelayUs(1);//SPI0CON SET ON
    wReg32(devId, 0x1F820130, 0x00000100); //CORETIMER_DelayUs(1);//CE init
    wReg32(devId, 0x1F820130, 0x00000100); //CORETIMER_DelayUs(1);//CE init
    wReg32(devId, 0x1F820130, 0x00000100); //CORETIMER_DelayUs(1);//CE init
    wReg32(devId, 0x1F820130, 0x00000100); //CORETIMER_DelayUs(1);//CE init
//    //--------------- SPI0 related settings
//
//    // //--------------- get the device ID of the stacked flash -----------------------
    wReg32(devId, 0x1F820130, 0x00000100); //CORETIMER_DelayUs(1);//'CE 10
    wReg32(devId, 0x1F820130, 0x00000000); //CORETIMER_DelayUs(1);//'CE 10
    wReg32(devId, 0x1F801620, 0x0000009F); //CORETIMER_DelayUs(1);//dev ID
    wReg32(devId, 0x1F801620, 0x0); //CORETIMER_DelayUs(1);
    rReg32(devId, 0x1F801620); //CORETIMER_DelayUs(1);        
    wReg32(devId, 0x1F801620, 0x0); //CORETIMER_DelayUs(1);
    rReg32(devId, 0x1F801620); //CORETIMER_DelayUs(1);    
    wReg32(devId, 0x1F801620, 0x0); //CORETIMER_DelayUs(1);    
    rReg32(devId, 0x1F801620); //CORETIMER_DelayUs(1);
    
    wReg32(devId, 0x1F820130, 0x00000000); //CORETIMER_DelayUs(1);//'CE 01
    wReg32(devId, 0x1F820130, 0x00000100); //CORETIMER_DelayUs(1);//'CE 01
//    // //--------------- get the device ID of the stacked flash -----------------------
//
//    //'''''''''''''''''''''''''''Flash_ClearBPR
//    wReg32(devId, 0x1F820130, 0x00000100); //CORETIMER_DelayUs(1);//'CE 10
//    wReg32(devId, 0x1F820130, 0x00000000); //CORETIMER_DelayUs(1);//'CE 10
//    wReg32(devId, 0x1F801620, 0x00000098); //CORETIMER_DelayUs(1);//'command
//    wReg32(devId, 0x1F801604, 0x00008000); //CORETIMER_DelayUs(1);
//    wReg32(devId, 0x1F801608, 0x00008000); //CORETIMER_DelayUs(1);
//    wReg32(devId, 0x1F820130, 0x00000000); //CORETIMER_DelayUs(1);//'CE 01
//    wReg32(devId, 0x1F820130, 0x00000100); //CORETIMER_DelayUs(1);//'CE 01
//    //'''''''''''''''''''''''''''''''''''''''''''''''''   
//
//    //'-------------------------------RESET ENABLED
//    wReg32(devId, 0x1F820130, 0x00000100); //CORETIMER_DelayUs(1);//'CE 10
//    wReg32(devId, 0x1F820130, 0x00000000); //CORETIMER_DelayUs(1);//'CE 10
//    wReg32(devId, 0x1F801620, 0x00000066); //CORETIMER_DelayUs(1);//'command
//    wReg32(devId, 0x1F820130, 0x00000000); //CORETIMER_DelayUs(1);//'CE 01
//    wReg32(devId, 0x1F820130, 0x00000100); //CORETIMER_DelayUs(1);//'CE 01
//    wReg32(devId, 0x1F820130, 0x00000100); //CORETIMER_DelayUs(1);//'CE 10
//    wReg32(devId, 0x1F820130, 0x00000000); //CORETIMER_DelayUs(1);//'CE 10
//    wReg32(devId, 0x1F801620, 0x00000099); //CORETIMER_DelayUs(1);//'command
//    wReg32(devId, 0x1F820130, 0x00000000); //CORETIMER_DelayUs(1);//'CE 01
//    wReg32(devId, 0x1F820130, 0x00000100); //CORETIMER_DelayUs(1);//'CE 01
//    //'-------------------------------RESET ENABLED
//
//
//    //'''''''''''''''''''''''''''Write Enable
//    wReg32(devId, 0x1F820130, 0x00000100); //CORETIMER_DelayUs(1);//'CE 10
//    wReg32(devId, 0x1F820130, 0x00000000); //CORETIMER_DelayUs(1);//'CE 10
//    wReg32(devId, 0x1F801620, 0x00000006); //CORETIMER_DelayUs(1);//'command
//    wReg32(devId, 0x1F820130, 0x00000000); //CORETIMER_DelayUs(1);//'CE 01
//    wReg32(devId, 0x1F820130, 0x00000100); //CORETIMER_DelayUs(1);//'CE 01
//    //'''''''''''''''''''''''''''''''''''''''''''''''''   
//    
//    
//            //'''''''''''''''''''''''''''Status
//    for(int i = 0; i < 10; i++)
//    {
//        wReg32(devId, 0x1F820130, 0x00000100); //CORETIMER_DelayUs(1);//'CE 10
//        wReg32(devId, 0x1F820130, 0x00000000); //CORETIMER_DelayUs(1);//'CE 10
//        wReg32(devId, 0x1F801620, 0x00000005); //CORETIMER_DelayUs(1);//'command
//        rReg32(devId, 0x1F801620);
//        wReg32(devId, 0x1F820130, 0x00000000); //CORETIMER_DelayUs(1);//'CE 01
//        wReg32(devId, 0x1F820130, 0x00000100); //CORETIMER_DelayUs(1);//'CE 01
//    }
//    //'''''''''''''''''''''''''''''''''''''''''''''''''   
//    
//    //'''''''''''''''''''''''''''Write Enable
//    wReg32(devId, 0x1F820130, 0x00000100); //CORETIMER_DelayUs(1);//'CE 10
//    wReg32(devId, 0x1F820130, 0x00000000); //CORETIMER_DelayUs(1);//'CE 10
//    wReg32(devId, 0x1F801620, 0x00000060); //CORETIMER_DelayUs(1);//'command
//    wReg32(devId, 0x1F820130, 0x00000000); //CORETIMER_DelayUs(1);//'CE 01
//    wReg32(devId, 0x1F820130, 0x00000100); //CORETIMER_DelayUs(1);//'CE 01
//    
//    
//    for(int i = 0; i < 100; i++)
//    {
//        wReg32(devId, 0x1F820130, 0x00000100); //CORETIMER_DelayUs(1);//'CE 10
//        wReg32(devId, 0x1F820130, 0x00000000); //CORETIMER_DelayUs(1);//'CE 10
//        wReg32(devId, 0x1F801620, 0x00000005); //CORETIMER_DelayUs(1);//'command
//        rReg32(devId, 0x1F801620);
//        wReg32(devId, 0x1F820130, 0x00000000); //CORETIMER_DelayUs(1);//'CE 01
//        wReg32(devId, 0x1F820130, 0x00000100); //CORETIMER_DelayUs(1);//'CE 01
//    }
//    
    
}


/* *****************************************************************************
 End of File
 */
