
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
#include "io.h"

#include "stdlib.h"
#include "time.h"
#include "stdio.h"

#include <util/delay.h>
#include "ledmatrix7219d88.h"



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

unsigned char checker = 0;
unsigned char counting = 0;
unsigned char number = 0;



enum states{init, lights_wait, lit_up, user_input, wait_up, wait_down, wait_right, wait_left, correct, wrong} state; //might want a starter state?

void tick() { //transitions babyyyyyy

	int x, y, xy;
	//unsigned char center, up, down, left, right = 0;
	ADC_Init();

	x = ADC_Read(1);
	y = ADC_Read(2);
	xy = ADC_Read(3);

  switch (state) {

    case init: ;
			//unsigned temp = ~PINA&0x01; //meant to ignore joystick inputs and just care about button
			// time_t now;
			// time(&now);
			// ctime(&now);
			// struct tm *local = localtime(&now);
			// counting = localtime->tm_sec;
			// counting = counting % 4;
			if((x > 400 && x < 600) && (y > 400 && y < 600)) { //meaning button not pressed
				counting++;
				LCD_Cursor(1);
				LCD_WriteData(counting + '0');
				state = init;
			}
			else {
				number = x + y;
				state = lights_wait;
			}
      break;

    case lights_wait: //automatically moves to next state after finsihing for loop
			PORTB = 0;
      if(i < count){
				light_on = number % num_lights;
        //light_on = rand() % num_lights;
        array[i] = light_on;
				number = (number * light_on + 69) % 157; //https://www.quora.com/What-is-a-simple-formula-to-generate-a-random-number
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
				//int debug_wait = 9;
        if((x > 400 && x < 600) && (y > 400 && y < 600)) { //waiting for user input, currently in middle state
          state = user_input;
        }
        else if((array[j] == 0) && (y<400)) { //user got it right on top light //was 600
          count_2++;
          j++;
					LCD_Cursor(1);
					LCD_WriteData(debug_up+'0');
					state = wait_up;
          //state = user_input;
        }
        else if((array[j] == 1) && (x<400)) { //user got it right on left light //was 600
          count_2++;
          j++;
					LCD_Cursor(1);
					LCD_WriteData(debug_left+'0');
					
          state = wait_left;
        }
        else if((array[j] == 2) && (x>600)) { //user got it right on right light //wass 400
          count_2++;
          j++;
					LCD_Cursor(1);
					LCD_WriteData(debug_right+'0');
					
          state = wait_right;
        }
        else if((array[j] == 3) && (y>600)) { //user got it right on bottom light
          count_2++;
          j++;
					LCD_Cursor(1);
					LCD_WriteData(debug_down+'0');
					
          state = wait_down;
        }
        else{ //user got it wrong
          state = wrong; 
        }
      }
			break;

		case wait_up:
			if((x > 400 && x < 600) && (y > 400 && y < 600)) { //waiting for user input, currently in middle state
          state = user_input;
			}
			else {
				state = wait_up;
			}
			break;

		case wait_left:
			if((x > 400 && x < 600) && (y > 400 && y < 600)) { //waiting for user input, currently in middle state
          state = user_input;
        }
			else {
				state = wait_left;
			}
			break;

		case wait_right:
			if((x > 400 && x < 600) && (y > 400 && y < 600)) { //waiting for user input, currently in middle state
          state = user_input;
        }
			else {
				state = wait_right;
			}
			break;

		case wait_down:
			if((x > 400 && x < 600) && (y > 400 && y < 600)) { //waiting for user input, currently in middle state
          state = user_input;
        }
			else {
				state = wait_down;
			}
			break;

    case correct:
      //printf("smart boi") on LCD;
      
      PORTB = 0x1F;
			ledmatrix7219d88_init();


			uint8_t ledmatrix2 = 0;
			uint8_t p = 1;
		//for(;;) {
		int8_t led = 0;
		//loop every matrix
		for(ledmatrix2=0; ledmatrix2<2; ledmatrix2++){
			//loop every led
			for(led=0; led<64; led++) {
				if(p)
					ledmatrix7219d88_setledon(ledmatrix2, led);
				else
					ledmatrix7219d88_setledoff(ledmatrix2, led);
				_delay_ms(50);
			}
			ledmatrix7219d88_resetmatrix(ledmatrix2);
		}

		if(p)
			p = 0;
		else
			p = 1;

		_delay_ms(1000);
	//}


      i=0;
			j=0;
      count++;
      state = init;
      break;
    
    case wrong:
      //printf("you are stupid, a loser, and have no friends") on LCD;
			
			PORTB = 0x01;
			// LCD_Cursor(1);
			// LCD_WriteData("wrong"+'0');

			ledmatrix7219d88_init();


	uint8_t ledmatrix = 0;

	//display test rows
	uint8_t rows[8] = {
			0b10000001,
			0b01000010,
			0b00100100,
			0b00011000,
			0b00011000,
			0b00100100,
			0b01000010,
			0b10000001
	};
	ledmatrix = 0;
	ledmatrix7219d88_setmatrix(0, rows);
	ledmatrix = 1;
	ledmatrix7219d88_setmatrix(1, rows);
	_delay_ms(2000);

	//display test rows
	ledmatrix = 0;
	ledmatrix7219d88_setrow(ledmatrix, 0, 0b10000000);
	ledmatrix7219d88_setrow(ledmatrix, 1, 0b01000000);
	ledmatrix7219d88_setrow(ledmatrix, 2, 0b00100000);
	ledmatrix7219d88_setrow(ledmatrix, 3, 0b00010000);
	ledmatrix7219d88_setrow(ledmatrix, 4, 0b00001000);
	ledmatrix7219d88_setrow(ledmatrix, 5, 0b00000100);
	ledmatrix7219d88_setrow(ledmatrix, 6, 0b00000010);
	ledmatrix7219d88_setrow(ledmatrix, 7, 0b00000001);
	ledmatrix = 1;
	ledmatrix7219d88_setrow(ledmatrix, 0, 0b10101010);
	ledmatrix7219d88_setrow(ledmatrix, 1, 0b01010101);
	ledmatrix7219d88_setrow(ledmatrix, 2, 0b10101010);
	ledmatrix7219d88_setrow(ledmatrix, 3, 0b01010101);
	ledmatrix7219d88_setrow(ledmatrix, 4, 0b10101010);
	ledmatrix7219d88_setrow(ledmatrix, 5, 0b01010101);
	ledmatrix7219d88_setrow(ledmatrix, 6, 0b10101010);
	ledmatrix7219d88_setrow(ledmatrix, 7, 0b01010101);
	_delay_ms(2000);
	ledmatrix7219d88_resetmatrix(0);
	ledmatrix7219d88_resetmatrix(1);

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

		case wait_up:
			break;

		case wait_down:
			break;

		case wait_right:
			break;

		case wait_left:
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

	//timing();

	srand(counting); // randomize seed
	
	

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



/*


//CODE TO MAKE THE MATRIX WORK:
//ledmatrix7219d88 output example

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "ledmatrix7219d88.h"


int main(void) {
	//init ledmatrix
	ledmatrix7219d88_init();


	uint8_t ledmatrix = 0;

	//display test rows
	uint8_t rows[8] = {
			0b10000001,
			0b01000010,
			0b00100100,
			0b00011000,
			0b00011000,
			0b00100100,
			0b01000010,
			0b10000001
	};
	ledmatrix = 0;
	ledmatrix7219d88_setmatrix(0, rows);
	ledmatrix = 1;
	ledmatrix7219d88_setmatrix(1, rows);
	_delay_ms(2000);

	//display test rows
	ledmatrix = 0;
	ledmatrix7219d88_setrow(ledmatrix, 0, 0b10000000);
	ledmatrix7219d88_setrow(ledmatrix, 1, 0b01000000);
	ledmatrix7219d88_setrow(ledmatrix, 2, 0b00100000);
	ledmatrix7219d88_setrow(ledmatrix, 3, 0b00010000);
	ledmatrix7219d88_setrow(ledmatrix, 4, 0b00001000);
	ledmatrix7219d88_setrow(ledmatrix, 5, 0b00000100);
	ledmatrix7219d88_setrow(ledmatrix, 6, 0b00000010);
	ledmatrix7219d88_setrow(ledmatrix, 7, 0b00000001);
	ledmatrix = 1;
	ledmatrix7219d88_setrow(ledmatrix, 0, 0b10101010);
	ledmatrix7219d88_setrow(ledmatrix, 1, 0b01010101);
	ledmatrix7219d88_setrow(ledmatrix, 2, 0b10101010);
	ledmatrix7219d88_setrow(ledmatrix, 3, 0b01010101);
	ledmatrix7219d88_setrow(ledmatrix, 4, 0b10101010);
	ledmatrix7219d88_setrow(ledmatrix, 5, 0b01010101);
	ledmatrix7219d88_setrow(ledmatrix, 6, 0b10101010);
	ledmatrix7219d88_setrow(ledmatrix, 7, 0b01010101);
	_delay_ms(2000);
	ledmatrix7219d88_resetmatrix(0);
	ledmatrix7219d88_resetmatrix(1);

	//test loop
	uint8_t p = 1;
	for(;;) {

		int8_t led = 0;
		//loop every matrix
		for(ledmatrix=0; ledmatrix<LEDMATRIX7219D88_MAXLEDMATRIX; ledmatrix++){
			//loop every led
			for(led=0; led<64; led++) {
				if(p)
					ledmatrix7219d88_setledon(ledmatrix, led);
				else
					ledmatrix7219d88_setledoff(ledmatrix, led);
				_delay_ms(50);
			}
			ledmatrix7219d88_resetmatrix(ledmatrix);
		}

		if(p)
			p = 0;
		else
			p = 1;

		_delay_ms(1000);
	}
}

*/


















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









