/* Very simple software for PLL ADF4351 and ATTiny13
  can be compiled with arduino and microcore
  https://github.com/MCUdude/MicroCore
  v 0.1 Ondra OK1CDJ 9/2018 ondra@ok1cdj.com

  Please check the ADF4351 datasheet or use ADF4351 software
  to get PLL register values.

  PIN layout
  PB4 ADF4531 clock (CLK)
  PB3 ADF4531 data (DATA)
  PB2 ADF4531 latch enable (LE)

*/
/****************************** 25M基准****************************************************8
  // TX  Fout= 1960.5 MHz +5dBm LO= 1690.5 MHz +439.55 MHz=2400.05MHZ
  r0 = 0X4E00A8;
  r1 = 0X80080C9;
  r2 = 0X4E42;
  r3 = 0X4B3;
  r4 = 0X9C803C;
  r5 = 0X580005;
  //RX Fout= 300 MHz +5dBm LO= 300MHz -739.55 MHz=439.55MHZ
  r0 = 0X300000;
  r1 = 0X8008011;
  r2 = 0X4E42;
  r3 = 0X4B3;
  r4 = 0XBC803C;
  r5 = 0X580005;

************************************** 10M 基准*****************************************************************
  // TX  Fout= 1960.5 MHz +5dBm LO= 1690.5 MHz +439.55 MHz=2400.05MHZ
  r0 = 0XC40008;
  r1 = 0X8008051;
  r2 = 0X4E42;
  r3 = 0X4B3;
  r4 = 0X95003C;
  r5 = 0X580005;
  //RX Fout= 300 MHz +5dBm LO= 300MHz -739.55 MHz=439.55MHZ
  r0 = 0x780000;
  r1 = 0X8008011;
  r2 = 0X4E42;
  r3 = 0X4B3;
  r4 = 0XB5003C;
  r5 = 0X580005;

  20190330 修改了外部基准由gpsdo输入。10M  增加了PTT输入 RX/TX切换输出   by BG0AUB
  
  20190920 修改了ADF4351控制数据，rx=51.584625 tx=1969.501 增加了PTT延时。rx/tx切换指示 by BG0AUB
*////////////////////////////////////////////////////////////////////////////////////////////////////
// PLL registers
//long int r0, r1, r2, r3, r4, r5;
//long int TX_R[6] = {0X4E00A8, 0X80080C9, 0X4E42, 0X4B3, 0X9C803C, 0X580005};
//long int RX_R[6] = {0X300000, 0X8008011, 0X4E42, 0X4B3, 0XBC803C, 0X580005};

int LE = 2;                                 //EL引脚
int CLK = 4;                                 //CLK引脚
int DAT = 3;                                //DAT引脚
int RX_TX = 0;                              //PTT
int LED = 1;                                //RX/TX_LED
int RX_P;
int TX_P;


void write2PLL(uint32_t PLLword)
{ // clocks 32 bits word  directly to the ADF4351
  // msb (b31) first, lsb (b0) last
  for (byte i = 32; i > 0; i--)
  { // PLL word 32 bits
    (PLLword & 0x80000000 ? PORTB |= 0b00001000 : PORTB &= 0b11110111);  // data on PB3
    PORTB |= 0b00010000;                   // clock in bit on rising edge of CLK (PB4 = 1)
    PORTB &= 0b11101111;                   // CLK (PB4 = 0)
    PLLword <<= 1;                         // rotate left for next bit
  }
  PORTB |= 0b00000100;                   // latch in PLL word on rising edge of LE (PB2 = 1)
  PORTB &= 0b11111011;  // LE (PB2 = 0)
  delayMicroseconds(1);
}

void setup ()
{
  //DDRB  = 0xff; // PB are all outputs
  //PORTB = 0x00; // make PB low
  pinMode(LE, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(DAT, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(RX_TX, INPUT);
  digitalWrite(LED, 1);
  delay(500);


//  //10M基准 1969.501M
//  write2PLL(0XBC803C & 0xffffffdf);  //关闭输出
//  write2PLL(0X580005);
//  write2PLL(0X928034);
//  write2PLL(0X4B3);
//  write2PLL(0X8E42);
//  write2PLL(0X800CE21);
//  write2PLL(0X189BE88);
//  delayMicroseconds(500);
  write2PLL(0XBC803C & 0xffffffdf);  //关闭输出
  write2PLL(0X580005);
  write2PLL(0XE5002C);
  write2PLL( 0X4B3);
  write2PLL(0X4E42);
  write2PLL(0X800A711);
  write2PLL(0XA50588);
  digitalWrite(LED, 0);
}

// main loop
void loop()
{ if (digitalRead(RX_TX) == 1)
  { if (RX_P == 0)
    {delay(700);
      //10M基准       51.584625 LNB REF
      write2PLL(0XBC803C & 0xffffffdf);  //关闭输出
      write2PLL(0X580005);
      write2PLL(0XE5002C);
      write2PLL( 0X4B3);
      write2PLL(0X4E42);
      write2PLL(0X800A711);
      write2PLL(0XA50588);
      RX_P = 1;
      TX_P = 0;
      digitalWrite(LED, 0);
    }
  }
  else
  { if (TX_P == 0)
    {
      //10M基准 1969.501M
      write2PLL(0XBC803C & 0xffffffdf);  //关闭输出
      write2PLL(0X580005);
      write2PLL(0X928034);
      write2PLL(0X4B3);
      write2PLL(0X8E42);
      write2PLL(0X800CE21);
      write2PLL(0X189BE88);
      TX_P = 1;
      RX_P = 0;
      digitalWrite(LED, 1);
    }
  }

}
//*****************************THE END*********************************************************************8
// delay(10);
//TX ON/OFF can be used for CW keying
//write2PLL(r4 & 0xffffffdf);

////void write_dat(long int *r)
//// write from r5 to r0
//{ write2PLL(r5);
//  write2PLL(r4);
//  write2PLL(r3);
//  write2PLL(r2);
//  write2PLL(r1);
//  write2PLL(r0);
//}
