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
void serial_write_double(double);
double serial_read_double(void);
void serial_write_char(char);
char serial_recieve_char(void);
void serial_init(void);
void pwm_init(void);
/////////////////////////////////////////////
void main(void) {
  serial_init();
  pwm_init();
  char something;
  double something2;
  something = serial_recieve_char();
  _delay_ms(10);
  serial_write_char(something);
  while (1) {
    something2 = serial_read_double();
    //_delay_ms(10);
    serial_write_double(something2/100);
  }
}
/////////////////////////////////////////////
//send double
void serial_write_double(double number){
    char string[10];
    int counter;
    snprintf(string, sizeof(string), "%f", number);
    for (counter = 0; (string[counter] != '\0')
        && (counter < sizeof(string)); counter++) {
      serial_write_char(string[counter]);
    }
}
//read double
double serial_read_double(void){
    char string[10];
    int counter = 0;
    do {
      string[counter] = serial_recieve_char();
      counter++;
    } while ((string[counter-1] != '\0') && (counter < sizeof(string)));
    return strtod(string, NULL);
}
//send char
void serial_write_char(char data) {
    while (!( UCSRA & (1<<UDRE))); // wait while register is free
    UDR = data; //load data in the register
}
//receive char
char serial_recieve_char(void) {
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
  UCSRC|= (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1); // 8bit data format
}
//set pwm
void pwm_init(void) {
  TCCR1A |= (1<<COM1A1)|(1<<WGM11)|(1<<WGM10);
  TCCR1B |= (1<<WGM12)|(1<<CS10);
  DDRD |= (1<<PD5);
  OCR1A = 0;
}
