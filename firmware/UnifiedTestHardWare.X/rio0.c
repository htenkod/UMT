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
#include "peripheral/coretimer/plib_coretimer.h"


 void wifi_spi_write(uint32_t devId, unsigned int spi_addr, unsigned int data)
{
    unsigned int  addr_bit = 0, data_bit = 0, bit_idx = 0;

    wReg32(devId, RIO0_RFSPICTL, (RIO0_EN_BIT_BANG | RIO0_CS_HIGH);
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

unsigned int wifi_spi_read(unsigned int spi_addr)
{
    unsigned int  addr_bit = 0, bit_idx, read_data, cmd_high = 0x08;
    cmd_high = 0x8;
    
    // Set the RF override bit and CS_n high
    wReg32(devId, RIO0_RFSPICTL, (RIO0_EN_BIT_BANG | RIO0_CS_HIGH);
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
        wReg32(devId, RIO0_RFSPICTL, RIO0_EN_BIT_BANG | cmd_high )));             
        // Rising edge of clk
        wReg32(devId, RIO0_RFSPICTL, ((RIO0_EN_BIT_BANG | cmd_high | RIO0_CLK_HIGH)));     
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
    wReg32(devId, RIO0_RFSPICTL, 0x80000022);
    /* make sure we properly take out of reset */
    wReg32(devId, RIO0_RFSPICTL, 0x80000002);

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
static void posc_xtal_startup_config_after(void)
{
    /* xtal LDO feedback divider (1.3+ v) */
    wifi_spi_write(0x85, 0x00F4); 
}
    

void RIO0_SYS_Initialize(uint32_t devId)
{
    
    
/*
    tmod12_A_D_Write(0x1F800080, 0x00000000); delayMicroseconds(1); //'SYSKEY
    tmod12_A_D_Write(0x1F800080, 0xAA996655); delayMicroseconds(1); //'SYSKEY
    tmod12_A_D_Write(0x1F800080, 0x556699AA); delayMicroseconds(1); //'SYSKEY
    //Posc_xtal_startup_config_before
    tmod12_A_D_Write(0x1F8C8028, 0x80000022);//RFSPICTL 'make sure we properly reset SPI to a known state
    tmod12_A_D_Write(0x1F8C8028, 0x80000002);//RFSPICTL 'make sure we properly take out of reset
    rio_wifi_spi_write_TMOD12(0x85,0x00F2); //MBIAS Filter, XTAL LDO on debug and testing
    delay(1);
    rio_wifi_spi_write_TMOD12(0x84,0x0001); //31 analog test
    delay(1);
    rio_wifi_spi_write_TMOD12(0x1E,0x0510); //MBIAS reference adjustment
    delay(1);
    rio_wifi_spi_write_TMOD12(0x82,0x6000); //xtal LDO feedback divider (1.3+ v)
    delay(1);
    tmod12_A_D_Write(0x1F800020, 0x7F7FFC38);//Enable POSC
    delay(1);
    rio_wifi_spi_write_TMOD12(0x85,0x00F4);
    delay(1);

    //--------------- Rio0 initial setup, includes sys pll setting for 200 Mhz Core speed and 40Mhz SPI0
    tmod12_A_D_Write(0x1F820004, 0x00000200); delayMicroseconds(1); //'ANSELACLR 'RA9 set to digital
    tmod12_A_D_Write(0x1F820034, 0x00000200); delayMicroseconds(1); //'LATACLR 'RA9 to a zero to start
    tmod12_A_D_Write(0x1F820014, 0x00000200); delayMicroseconds(1); //'TRISACLR 'RA9 set to output pin
    tmod12_A_D_Write(0x1F800080, 0x00000000); delayMicroseconds(1); //'SYSKEY
    tmod12_A_D_Write(0x1F800080, 0xAA996655); delayMicroseconds(1); //'SYSKEY
    tmod12_A_D_Write(0x1F800080, 0x556699AA); delayMicroseconds(1); //'SYSKEY
    tmod12_A_D_Write(0x1F8000A4, 0x20000000); delayMicroseconds(1); //'PMD2CLR ' make sure REF2O module is enabled   
    tmod12_A_D_Write(0x1F801A24, 0x00000003); delayMicroseconds(1); //'RPA9R 'set PORTA PIN 9 to be used by REF2O module
    tmod12_A_D_Write(0x1F8012C0, 0x00000000); delayMicroseconds(1); //'REFO2CON 'REF2O is used for output PLL frequency
    tmod12_A_D_Write(0x1F800020, 0x7F7FFC38); delayMicroseconds(1); //'enable POSC
    tmod12_A_D_Write(0x1F800080, 0x00000000); delayMicroseconds(1); //'SYSKEY
    tmod12_A_D_Write(0x1F800080, 0xAA996655); delayMicroseconds(1); //'SYSKEY
    tmod12_A_D_Write(0x1F800080, 0x556699AA); delayMicroseconds(1); //'SYSKEY
    tmod12_A_D_Write(0x1F801224, 0x08000000); delayMicroseconds(1); //'SPLLCONCLR 'clear SPLLICLK to set POSC40MHz as input FREQ
    tmod12_A_D_Write(0x1F801224, 0x07000000); delayMicroseconds(1); //'SPLLCONCLR 'clear SPLLBWSEL
    tmod12_A_D_Write(0x1F801224, 0x00FF0000); delayMicroseconds(1); //'SPLLCONCLR 'clear SPLLFBDIV
    tmod12_A_D_Write(0x1F801224, 0x00007000); delayMicroseconds(1); //'SPLLCONCLR 'clear SPLLREFDIV
    tmod12_A_D_Write(0x1F801224, 0x0000003F); delayMicroseconds(1); //'SPLLCONCLR 'clear SPLL1POSTDIV
    tmod12_A_D_Write(0x1F801224, 0x00000080); delayMicroseconds(1); //'SPLLCONCLR 'clear SPLL1FRC to set SPLL VCO Post Divider output is used to drive spll1_clk
    tmod12_A_D_Write(0x1F801228, 0x01C85008); delayMicroseconds(1); //'SPLLCONSET 'setting for 200 MHz
    tmod12_A_D_Write(0x1F801224, 0x00000200); delayMicroseconds(1); //'SPLLCONCLR 'turn on SPLL
    tmod12_A_D_Write(0x1F801320, 0x00008001); delayMicroseconds(1); //'PB1DIV
    tmod12_A_D_Write(0x1F800034, 0x000000FF); delayMicroseconds(1); //'CFGCON3CLR
    tmod12_A_D_Write(0x1F800038, 0x00000014); delayMicroseconds(1); //'CFGCON3SET  'SPLL2 = (SYSPLL->200MHz/2) / 0x14 = 80MHz
    tmod12_A_D_Write(0x1F801208, 0x00000100); delayMicroseconds(1); //'OSCCONSET request switch to sys pll
    tmod12_A_D_Write(0x1F801208, 0x00000001); delayMicroseconds(1); //'OSCCONSET execute request
    // you can monitor the clock based on what pin used as set above
    tmod12_A_D_Write(0x1F8012C0, 0x00289007); delayMicroseconds(1); //'REFO2CON 'SPLL2 expect around 1 MHz  setting of REFO3CON divider set to 80 (80*1 MHz = 80 Mhz)
    //--------------- Rio0 initial setup, includes sys pll setting for 200 Mhz Core speed and 40Mhz SPI0

 
 */
    
}
/* *****************************************************************************
 End of File
 */
