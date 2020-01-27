/*	Author: trevorgorman
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum Compute_States { Start, Init, Wait, Inc, Dec, Zero} Compute_State;

void Com()
{
	switch(Compute_State) {   // Transitions
		case Start:  // Initial transition
		Compute_State = Init;
		unsigned temp = ~PINA&0x0F;
		break;

		case Init:
		temp = ~PINA&0x0F;
		if (temp == 0x01) {
			Compute_State = Inc;
		}
		else if (temp == 0x02) {
			Compute_State = Dec;
		}
		else if (temp == 0x03) {
			Compute_State = Zero;
		}
		break;

		case Wait:
		temp = ~PINA&0x0F;
		if ((temp == 0x01)||(temp == 0x02)){
			Compute_State = Wait;
		}
		else if((temp == 0x01)&&(temp == 0x02)){
			Compute_State = Zero;
		}
		else{
			Compute_State = Init;
		}
		break;
		
		case Inc:
		Compute_State = Wait;
		break;
		
		case Dec:
		Compute_State = Wait;
		break;
		
		case Zero:
		Compute_State = Wait;
		break;

		default:
		Compute_State = Start;
		break;
	} // Transitions

	switch(Compute_State) {   // State actions
		case Init:
		break;
		
		case Wait:
		break;
		
		case Inc:
		if (PORTB < 9) {
			PORTB = PORTB + 1;
		}
		break;

		case Dec:
		if (PORTB > 0) {
			PORTB = PORTB - 1;
		}
		break;
		
		case Zero:
		PORTB = 0;
		break;

		default:
		break;
	} // State actions
}

int main() {
	// Initialize outputs
	Compute_State = Start; // Indicates initial call
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	PORTB = 0x00;
	while(1) {
		
		Com();
	}
}



