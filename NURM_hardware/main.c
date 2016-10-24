/*
Code for the NURM hardware part.
Runs on an ATMEL ATmega32 micro which using PWM generates voltage on
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
#define BAUD 9600
#define BAUDRATE F_CPU/16/BAUD-1
/////////////////////////////////////////////
void uitstr(char *, uint16_t, char);
uint16_t strtui(char *, char);
void serial_write_int16_t(int16_t);
int16_t serial_read_int16_t(void);
void serial_write_uchar(char);
char serial_recieve_uchar(void);
void serial_init(void);
void pwm_init(void);
/////////////////////////////////////////////
void main(void) {
  serial_init();
  pwm_init();
  int i;
  char something;
  int16_t something2 = 12345;
  while (1) {
    something2 = serial_read_int16_t();
    serial_write_int16_t(something2);
    //serial_write_uchar('\n');
    //serial_write_uchar('\r');
  }
  for(;;);
}
/////////////////////////////////////////////
//uint16_t to string
void uitstr(char *str, uint16_t ul, char length) {
  do {
    str[length--]= (ul % 10) + '0';
    ul = ul / 10;
  } while (length);
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
  for (counter = 0; counter < sizeof(string); counter++) {
    string[counter] = '\0';
  }
  counter = 0;
  uitstr(string, number, sizeof(string));
  do {
    serial_write_uchar(string[counter]);
    counter++;
  } while (counter <= sizeof(string));
}
//read uint16_t
int16_t serial_read_int16_t(void){
  char string[5];
  int counter;
  for (counter = 0; counter < sizeof(string); counter++) {
    string[counter] = '\0';
  }
  counter = 0;
  for (counter = 0; counter < sizeof(string); counter++) {
    string[counter] = serial_recieve_uchar();
  }
  return strtui(string, sizeof(string));
}
//send char
void serial_write_uchar(char data) {
  while (!( UCSRA & (1<<UDRE))); // wait while register is free
  UDR = data; //load data in the register
}
//receive char
char serial_recieve_uchar(void) {
  while(!(UCSRA & (1<<RXC))); //wait while data is being received
  return UDR; //return 8-bit data
}
/////////////////////////////////////////////
//set serial
void serial_init(void) {
  //asynchronous operation
  //8-bit character size
  //one stop bit
  //no parity bit
  UBRRH = (BAUDRATE>>8); //setting the baudrate MSB
  UBRRL = BAUDRATE; //setting the baudrate LSB
  UCSRB |= (1<<TXEN)|(1<<RXEN); //enable tx and rx
  UCSRC |= (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1); // 8bit data format
}
//set pwm
void pwm_init(void) {
  TCCR1A |= (1<<COM1A1)|(1<<WGM11)|(1<<WGM10);
  TCCR1B |= (1<<WGM12)|(1<<CS10);
  DDRD |= (1<<PD5);
  OCR1A = 0;
}
