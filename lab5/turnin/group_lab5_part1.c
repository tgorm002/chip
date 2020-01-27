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

/*

unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

int main(void) {
    
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs, initialize to 1s
	DDRC = 0xFF; PORTC = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s


 
while(1)
	{
		unsigned char fuelLevel = PINA & 0x0F;
		unsigned char light = 0;
		unsigned char lowFuel = 0;
		
		if (fuelLevel == 2 || fuelLevel == 1) {
			light = 0x20;
			lowFuel = 0x40;
		}
		if (fuelLevel == 3 || fuelLevel == 4) {
			light = 0x30;
			lowFuel = 0x40;
		}
		if (fuelLevel == 5 || fuelLevel == 6) {
			light = 0x38;
		
		}
		if (fuelLevel == 13 || fuelLevel == 14 || fuelLevel == 15) {
			light = 0x3F;
		}
		if (fuelLevel == 10 || fuelLevel == 11 || fuelLevel == 12) {
			light = 0x3E;
		}
		if (fuelLevel ==7 || fuelLevel == 8 || fuelLevel == 9) {
			light = 0x3C;
		}
		

		PORTC = light | lowFuel;
	}	
   return 0;
}
*/

/*
void main()
{
    
    unsigned char SetBit(unsigned char x,unsigned char k, unsigned char b){
	    return(b?x|(0x01<<k):x&~(0x01<<k));
    }

    unsigned char GetBit(unsigned char x, unsigned char k){
	    return ((x&(0x01<<k))!=0);
    }

    int main(void)
    {
	    DDRA = 0x00; PORTA = 0xFF; //configure port a's 8 pins as inputs
	    DDRB = 0xFF; PORTB= 0x00; //configure port c's 8 pins as outputs
	    
	    
	    while(1)
	    {
		    unsigned char gaslevel = 0x00; //final gas and seatbelt check
		    unsigned char tempgas = 0x00; //should save only the upper nibble to check for the belt
		    unsigned char checkbelt = 0x00;
		    
		    tempgas = ~PINA&0x0F;   //saves only the lower nibble for PINA
		    
		    if(tempgas == 0x00){
			    gaslevel = SetBit(gaslevel,6, 1);
		    }
		    else if ((tempgas == 0x01) || (tempgas == 0x02))
		    {
			    gaslevel = SetBit(gaslevel,5, 1);
			    gaslevel = SetBit(gaslevel,6, 1);
		    }
		    else if ((tempgas == 0x03) || (tempgas == 0x04))
		    {
			    gaslevel = SetBit(gaslevel,6, 1);
			    gaslevel = SetBit(gaslevel,5, 1);
			    gaslevel = SetBit(gaslevel,4, 1);
		    }
		    else if ((tempgas == 0x05) || (tempgas == 0x06))
		    {
			    gaslevel = SetBit(gaslevel,5, 1);
			    gaslevel = SetBit(gaslevel,4, 1);
			    gaslevel = SetBit(gaslevel,3,1);
		    }
		    else if ((tempgas == 0x07) || (tempgas == 0x08) || (tempgas == 0x09))
		    {
			    gaslevel = SetBit(gaslevel,5, 1);
			    gaslevel = SetBit(gaslevel,4, 1);
			    gaslevel = SetBit(gaslevel,3,1);
			    gaslevel = SetBit(gaslevel,2,1);
		    }
		    else if ((tempgas == 0x0A) || (tempgas == 0x0B) || (tempgas == 0x0C))
		    {
			    gaslevel = SetBit(gaslevel,5, 1);
			    gaslevel = SetBit(gaslevel,4, 1);
			    gaslevel = SetBit(gaslevel,3,1);
			    gaslevel = SetBit(gaslevel,2,1);
			    gaslevel = SetBit(gaslevel,1,1);
		    }
		    else if ((tempgas == 0x0D) || (tempgas == 0x0E) || (tempgas == 0x0F))
		    {
			    gaslevel = SetBit(gaslevel,5, 1);
			    gaslevel = SetBit(gaslevel,4, 1);
			    gaslevel = SetBit(gaslevel,3,1);
			    gaslevel = SetBit(gaslevel,2,1);
			    gaslevel = SetBit(gaslevel,1,1);
			    gaslevel = SetBit(gaslevel,0,1);
		    }
		    checkbelt = 0xF0 & PINA;
		    if (checkbelt == 0x30){
			    gaslevel = SetBit(gaslevel,7,1);
		    }
		    PORTB= gaslevel;
	    }
    }
    while (1) 
    {
    }
}
*/
