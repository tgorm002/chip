
//LIVE LOVE SHREKK
//SHREK IS LOVE SHREK IS LIFE

/*
Shrek: Ogres are like onions.
Donkey: They stink?
Shrek: Yes. No.
Donkey: Oh, they make you cry.
Shrek: No.
Donkey: Oh, you leave em out in the sun, they get all brown, start sproutin’ little white hairs.
Shrek: No. Layers. Onions have layers. Ogres have layers. Onions have layers. You get it? We both have layers.
Donkey: Oh, you both have layers. Oh. You know, not everybody like onions.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc.h"


volatile unsigned char TimerFlag = 0; 

unsigned long _avr_timer_M = 1; 
unsigned long _avr_timer_cntcurr = 0; 

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { 
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}


void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

unsigned char count = 4;
unsigned char count_2 = 0;
unsigned char num_lights = 4;

int j = 0; 

int array[100];

unsigned temp = ~PINA&0x0F;

enum states{init, lights, user_input, right, wrong} state; //might want a starter state?

void tick() { //transitions babyyyyyy
  switch (state) {

    case init:
      if(!temp) {
        state = init;
      }
      else { // case where user pressed the butt
        state = lights;
      }
      break;

    case lights: //automatically moves to next state after finsihing for loop
      unsigned char light_on;
      for(int i = 0; i < count; i++) { //move through whole for loop before moving to user_input state
        light_on = rand() % num_lights;
        array[i] = light_on;
        if(light_on == 0) { //up
          PORTB = 16;
        }
        if(light_on == 1) { //left
          PORTB = 4;
        }
        if(light_on == 2) { //rihgt
          PORTB = 8;
        }
        if(light_on == 3) { //down
          PORTB = 2;
        }
      }
      state = user_input;
      break;

    case user_input:
      if(count_2 = count) { //user inputted correct order
        count_2 = 0;
        j = 0;
        state = right;
      }
      else if(count_2 < count) { //case where user isnt done inputting joystick inputs
        if((x > 400 && x < 600) && (y > 400 && y < 600)) { //waiting for user input, currently in middle state
          state = user_input;
        }
        else if(array[j] == 0 && y>600) { //user got it right on top light
          count_2++;
          j++;
          state = user_input;
        }
        else if(array[j] == 1 && x>600) { //user got it right on left light
          count_2++;
          j++;
          state = user_input;
        }
        else if(array[j] == 2 && x<400) { //user got it right on right light
          count_2++;
          j++;
          state = user_input;
        }
        else if(array[j] == 3 && y>600) { //user got it right on bottom light
          count_2++;
          j++;
          state = user_input;
        }
        else { //user got it wrong
          state = wrong;
        }
        break;
      }

    case right:
      //printf("smart boi") on LCD;
      for(int x = 0; i < 10; i++) {
        PORTB = 0x1F;
      }
      //PORTB = 0x1F;
      count++;
      state = init;
      break;
    
    case wrong:
      //printf("you are stupid, a loser, and have no friends") on LCD;
      count = 4;
      state = init;
      break;

    default:
      break;
  }

  switch(state) { //actions babbyyyyyy
    case init:
      break;

    case lights:
      count_2 = 0; //get this variable ready for the user
      break;

    case user_input:
      break;

    case right:
      break; //dont need aything here cuz we count++ in the transitions

    case wrong:
      break; //dont need anything here cuz we count = 4 in the transition
  }
}



int main() {
  DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
  DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;


	TimerSet(100);
	TimerOn();

	
  int x, y, xy;
	unsigned char center, up, down, left, right = 0;
	ADC_Init();
	
	
	while(1) {
    x = ADC_Read(0);
		y = ADC_Read(1);
		xy = ADC_Read(2);
		tick();
		while(!TimerFlag){}
		TimerFlag = 0;
	}
}

