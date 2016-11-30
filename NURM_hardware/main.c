/*
Code for the NURM hardware part.
Runs on an ATMEL ATmega328p micro which using PWM generates voltage on
an element under test and the measuring the current.

Author: Jakub Niemczuk
*/
/////////////////////////////////////////////
//Speed of the microcontroller
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
//
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
//UART BAUD RATE
#define USART_BAUDRATE 57600 //9600
#define BAUD_PRESCALE (((F_CPU/(USART_BAUDRATE*16UL)))-1)
/////////////////////////////////////////////
void uitstr(char *, uint16_t, char);
uint16_t strtui(char *, char);
void serial_write_int16_t(int16_t);
int16_t serial_read_int16_t(void);
void serial_write_uchar(char);
char serial_recieve_uchar(void);
void serial_init(void);
void pwm_init(void);
void adc_init(void);
/////////////////////////////////////////////
void main(void) {
  serial_init();
  pwm_init();
  adc_init();
  int16_t something;
  //OCR1A = 220;
  while (1) {
    OCR1A = serial_read_int16_t();
    ADCSRA |= (1<<ADSC);
    while( ADCSRA & (1<<ADSC) ); // wait until ADC conversion is complete
    serial_write_int16_t(ADC);
    //serial_write_uchar('\n');
    //serial_write_uchar('\r');
    //serial_write_int16_t(65323);
    //_delay_ms(100);
  }
  for(;;);
}
/////////////////////////////////////////////
//uint16_t to string
void uitstr(char *str, uint16_t ul, char length) {
  length--;
  str[length--] = '\0';
  do {
    str[length--] = (ul % 10) + '0';
    ul = ul / 10;
  } while (length + 1);
}
//string to uint16_t
uint16_t strtui(char *str, char length) {
  uint16_t number = 0;
  char counter;
  for (counter = 0; counter < length &&
      str[counter] >= '0' && str[counter] <= '9'; counter++) {
    number = number * 10 + str[counter] - '0';
  }
  return number;
}
/////////////////////////////////////////////
//send uint16_t
void serial_write_int16_t(int16_t number){
  char string[5];
  int counter;
  string[4] = '\0';
  //for (counter = 0; counter < sizeof(string); counter++) {
  //  string[counter] = '\0';
  //}
  //counter = 1;
  uitstr(string, number, sizeof(string));
  for (counter = 0; counter < sizeof(string); counter++) {
    serial_write_uchar(string[counter]);
  }
  //do {
  //  serial_write_uchar(string[counter]);
  //  counter++;
  //} while (counter <= sizeof(string));
}
//read uint16_t
int16_t serial_read_int16_t(void){
  char string[5];
  int counter;
  for (counter = 0; counter < sizeof(string); counter++) {
    string[counter] = '\0';
  }
  //counter = 0;
  for (counter = 0; counter < sizeof(string); counter++) {
    string[counter] = serial_recieve_uchar();
  }
  return strtui(string, sizeof(string));
}
//send char
void serial_write_uchar(char data) {
  while (!( UCSR0A & (1<<UDRE0))); // wait while register is free
  UDR0 = data; //load data in the register
}
//receive char
char serial_recieve_uchar(void) {
  while(!(UCSR0A & (1<<RXC0))); //wait while data is being received
  return UDR0; //return 8-bit data
}
/////////////////////////////////////////////
//set serial
void serial_init(void) {
  //asynchronous operation
  //8-bit character size
  //one stop bit
  //no parity bit
  UCSR0B |= (1<<RXEN0) | (1<<TXEN0); //enable tx and rx
  UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01); // 8bit data format
  UBRR0H  = (BAUD_PRESCALE >> 8); //setting the baudrate MSB
  UBRR0L  = BAUD_PRESCALE; //setting the baudrate LSB
}
//set pwm
void pwm_init(void) {
  TCCR1A |= (1<<COM1A1)|(1<<WGM11)|(1<<WGM10);
  TCCR1B |= (1<<WGM12)|(1<<CS10);
  DDRB |= (1<<PB1);
  OCR1A = 0;
}
//set adc
void adc_init(void) {
  ADMUX |= (1<<REFS1)|(1<<REFS0); //set internal 1.1V ref
  DIDR0 |= (1<<ADC0D); //disale IO function on adc0
  ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //128 prescaler for adc clock
  ADCSRA |= (1<<ADEN); //enable adc
}
