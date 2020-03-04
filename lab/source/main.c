
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


//RIGHT NOW MY LEFT AND RIGHT ARE INCORRECT


#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc.h"
#include "io.h"

#include "stdlib.h"
#include "time.h"
#include "stdio.h"


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
unsigned char light_on = 0;

int i = 0;

int j = 0; 

int array[100];

time_t t;


//unsigned temp = ~PINA&0x0F;

enum states{init, lights_wait, lit_up, user_input, correct, wrong} state; //might want a starter state?

void tick() { //transitions babyyyyyy

	//unsigned temp = ~PINA&0x0F;

	int x, y, xy;
	//unsigned char center, up, down, left, right = 0;
	ADC_Init();

	x = ADC_Read(0);
	y = ADC_Read(1);
	xy = ADC_Read(2);

  switch (state) {

    case init:
			state = lights_wait;
      break;

    case lights_wait: //automatically moves to next state after finsihing for loop
			PORTB = 0;
      if(i < count){
				
        light_on = rand() % num_lights;
        array[i] = light_on;
				LCD_Cursor(1);
				LCD_WriteData(light_on+'0');
				state = lit_up;
      }
			
			else {
      	state = user_input;
			}
      break;

		case lit_up:
			if(light_on == 0) { //up
						PORTB = 2;
        }
        else if(light_on == 1) { //left
          
						PORTB = 8;

        }
        else if(light_on == 2) { //rihgt
          
						PORTB = 4;
        }
        else if(light_on == 3) { //down
          
						PORTB = 16;

        }		
				i++;	
				state = lights_wait;
				break;




    case user_input:
      if(count_2 == count) { //user inputted correct order
        count_2 = 0;
        j = 0;
        state = correct;
      }
      else if(count_2 < count) { //case where user isnt done inputting joystick inputs
				int debug_up = 5;
				int debug_left = 6;
				int debug_right = 7;
				int debug_down = 8;
				int debug_wait = 9;
        if((x > 400 && x < 600) && (y > 400 && y < 600)) { //waiting for user input, currently in middle state
					//count_2++;
          state = user_input;
        }
        else if((array[j] == 0) && (y<600)) { //user got it right on top light
          count_2++;
          j++;
					LCD_Cursor(1);
					LCD_WriteData(debug_up+'0');
          state = user_input;
        }
        else if((array[j] == 1) && (x<600)) { //user got it right on left light
          count_2++;
          j++;
					LCD_Cursor(1);
					LCD_WriteData(debug_left+'0');
					
          state = user_input;
        }
        else if((array[j] == 2) && (x>400)) { //user got it right on right light
					while(x>600) {
						LCD_Cursor(1);
						LCD_WriteData(debug_wait+'0');
					}
          count_2++;
          j++;
					LCD_Cursor(1);
					LCD_WriteData(debug_right+'0');
					
          state = user_input;
        }
        else if((array[j] == 3) && (y>600)) { //user got it right on bottom light
          count_2++;
          j++;
					LCD_Cursor(1);
					LCD_WriteData(debug_down+'0');
					
          state = user_input;
        }
        else{ //user got it wrong
          state = wrong; 
        }
      }
			break;

    case correct:
      //printf("smart boi") on LCD;
      
      PORTB = 0x1F;
      i=0;
			j=0;
      count++;
      state = init;
      break;
    
    case wrong:
      //printf("you are stupid, a loser, and have no friends") on LCD;
			
			PORTB = 0x01;
			LCD_Cursor(1);
			LCD_WriteData("wrong"+'0');
			j = 0;
      i = 0;
      count = 4;
      state = init;
      break;

    default:
      break;
  }

  switch(state) { //actions babbyyyyyy
    case init:
      break;

    case lights_wait:
      count_2 = 0; //get this variable ready for the user
      break;

		case lit_up:
			break;

    case user_input:
      break;

    case correct:
      break; //dont need aything here cuz we count++ in the transitions

    case wrong:
      break; //dont need anything here cuz we count = 4 in the transition
		
		default:
			break;
	}
}



int main() {
  DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
  DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	
	//srand(time(&t)); // randomize seed
	

	TimerSet(500);
	TimerOn();

	state = init;
  
	LCD_init();
	
	while(1) {
    
		tick();
		while(!TimerFlag){}
		TimerFlag = 0;
	}
}
























//THE CODE USED TO MAKE THE JOUYSTICK WORK
/*
#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc.h"


int main() {
  DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
  DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	int x, y, xy;
	unsigned char center, up, down, left, right = 0;

	nokia_lcd_init();
	nokia_lcd_clear();
	nokia_lcd_write_string("IT'S WORKING!",1);
	nokia_lcd_set_cursor(0, 10);
	nokia_lcd_write_string("Nice!", 3);
	nokia_lcd_render();

	ADC_Init();

	while(1) {
		x = ADC_Read(0);
		y = ADC_Read(1);
		xy = ADC_Read(2);


	if(y>600) { //upward
			up = 1;
			PORTB = 16;
	}
	else if(y<400) {//down
		down = 1;
		PORTB = 2;
	}		
	else if(x<400) {//right
		right = 1;
		PORTB = 8;
	}	
	else if(x>600) {//left
		left = 1;
		PORTB = 4;
	}	
	else { //middle
		center = 1;
		PORTB = 1;
	}
	center = up = down = left = right = 0;

	}
	

}
*/









