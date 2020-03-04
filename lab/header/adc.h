/*
 * ADC_H.h
 *
 * Created: 7/28/2016 4:50:16 PM
 *  Author: User
 */ 


#ifndef ADC_H_H_
#define ADC_H_H_

#define F_CPU 8000000UL					/* Define CPU Frequency e.g. here its 8 Mhz */
#include <avr/io.h>						/* Include AVR std. library file */
#include <util/delay.h>
#define ipin PINA
#define iport PORTA

void ADC_Init()
{
	DDRA = 0x00;		/* Make ADC port as input */
	ADCSRA = 0x87;		/* Enable ADC, fr/128  */
	ADMUX = 0x40;		/* Vref: Avcc, ADC channel: 0 */
}

int ADC_Read(char channel)
{
	int ADC_value;
	
	ADMUX = (0x40) | (channel & 0x07);/* set input channel to read */
	ADCSRA |= (1<<ADSC);	/* start conversion */
	while((ADCSRA &(1<<ADIF))== 0);	/* monitor end of conversion interrupt flag */
	
	ADCSRA |= (1<<ADIF);	/* clear interrupt flag */
	ADC_value = (int)ADCL;	/* read lower byte */
	ADC_value = ADC_value + (int)ADCH*256;/* read higher 2 bits, Multiply with weightage */

	return ADC_value;		/* return digital value */
}

#endif /* ADC_H_H_ */