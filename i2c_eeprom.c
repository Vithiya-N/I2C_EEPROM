
// PIC16F877A Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = EXTRC     // Oscillator Selection bits (RC oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#define _XTAL_FREQ 6000000
#define EEPROM_ADD_W 0xA0
#define EEPROM_ADD_R 0xA1


void my_I2C_Master_init(unsigned long baud);
void my_I2C_Master_wait();
void my_I2C_START();
void my_I2C_STOP();
void my_I2C_RESTART();
void my_I2C_ACK();
void my_I2C_NACK();
unsigned int my_I2C_WRITE(unsigned int data);
unsigned int my_I2C_READ();
void my_I2C_EEPROM_WRITE(unsigned int add,unsigned int data);
unsigned int my_I2C_EEPROM_READ(unsigned int add);
unsigned int z;


void main()
{   
 
    PORTD=0x00;
    TRISD=0x00;// Set portd as output
    my_I2C_Master_init(100000); // Initialize I2C Master with 100kHz clock
    my_I2C_EEPROM_WRITE(0x0028,'A');// Write 'A' to EEPROM address 0x0028
    z=my_I2C_EEPROM_READ(0x0028); // Read from EEPROM address 0x0028
   
   if(z=='A') // checks for the condition 
    {
        PORTD=1;// led blink
    }
    
    while(1)
    {
        
    }

}

void my_I2C_Master_init(unsigned long baud)
{
    SSPCON=0x28; // Configure MSSP module as I2C Master
    SSPCON2=0x00;
    SSPADD=((_XTAL_FREQ)/4*baud)-1;  // Set baud rate
    SSPSTAT=0;
    TRISC3=1;  // Set SCL (RC3) as input
    TRISC4=1; // Set SDA (RC4) as input
}
    

void my_I2C_Master_wait()
{
    while((SSPSTAT & 0x04)||(SSPCON2 & 0x1F)); // Wait for I2C module to be idle
}

void my_I2C_START()
{
  my_I2C_Master_wait();
  SEN=1;// Initiate Start condition
}

void my_I2C_STOP()
{
    my_I2C_Master_wait();
    PEN=1; // Initiate Stop condition
}

void my_I2C_RESTART()
{
    my_I2C_Master_wait();
    RSEN=1; // Initiate Restart condition
}

void my_I2C_ACK()
{
    ACKDT=0;// Acknowledge data
    my_I2C_Master_wait();
    ACKEN=1;// Enable Acknowledge
}

void my_I2C_NACK()
{
    ACKDT=1;// Not Acknowledge data
    my_I2C_Master_wait();
    ACKEN=1;// Enable Acknowledge
}
unsigned int my_I2C_WRITE(unsigned int data)
{
    my_I2C_Master_wait();
    SSPBUF=data; // Load data into buffer
    while(SSPIF==0);// Wait until transmission is complete
    SSPIF=0; // Clear SSPIF flag
    my_I2C_Master_wait(); 
    return ACKSTAT; // Return ACK status
  
}

unsigned int my_I2C_READ()
{
    
    my_I2C_Master_wait(); 
    RCEN=1; // Enable reception
    while(SSPIF==0);// Wait until reception is complete
    SSPIF=0;// Clear SSPIF flag
    my_I2C_Master_wait(); 
    return SSPBUF ;// Return received data
    
}

void my_I2C_EEPROM_WRITE(unsigned int add,unsigned  int data)
{
    my_I2C_Master_wait();
    my_I2C_START();// Start I2C communication
    while(my_I2C_WRITE(EEPROM_ADD_W))// Send EEPROM write address and check for ACK
    {
        my_I2C_START();
    }
    my_I2C_WRITE(add>>8); // Send upper address bits
    my_I2C_WRITE((unsigned char)add); // Send low address bits
    my_I2C_WRITE(data); // Send data
    my_I2C_STOP(); // Stop I2C communication
    __delay_ms(1000);
}

unsigned int my_I2C_EEPROM_READ(unsigned int add)
{
  unsigned int x;  
  my_I2C_Master_wait();
  my_I2C_START(); // Start I2C communication
  while(my_I2C_WRITE(EEPROM_ADD_W))// Send EEPROM write address and check for ACK
  {
      my_I2C_START();
  }
  my_I2C_WRITE(add>>8); // Send upper address bits
  my_I2C_WRITE((unsigned char)add); // Send low address bits
  my_I2C_RESTART(); // Restart I2C communication
  while(my_I2C_WRITE(EEPROM_ADD_R)) // Send EEPROM read address and check for ACK
  {
      my_I2C_RESTART();
  }
  x=my_I2C_READ(); // Read data from EEPROM
  my_I2C_NACK(); // Send NACK to indicate end of data reception
  my_I2C_STOP(); // Stop I2C communication
  __delay_ms(1000);
  return x; // Return received data
  
}





