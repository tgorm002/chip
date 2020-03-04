
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

#include <stdio.h>
#include "io.h"
//#include "io.c"
#include "bit.h"
#include "timer.h"

unsigned char x = 0x1F;
unsigned char get_value;
unsigned char temp = 0;
unsigned char temp2 = 0;

typedef struct _task {
	unsigned char state;
	unsigned long int period;
	unsigned long int Time_Taken;
	int (*TickFct)(int);
} task;

/* Keypad arrangement:
        PC4 PC5 PC6 PC7
    col  1   2   3   4
  row
PC0 1    1 | 2 | 3 | A
PC1 2    4 | 5 | 6 | B
PC2 3    7 | 8 | 9 | C
PC3 4    * | 0 | # | D
*/
unsigned char GetKeypadKey() {
	PORTA = 0xEF;
	asm("nop");
	if (GetBit(PINA,0)==0) { return('1'); }
	if (GetBit(PINA,1)==0) { return('4'); }
	if (GetBit(PINA,2)==0) { return('7'); }
	if (GetBit(PINA,3)==0) { return('*'); }

	PORTA = 0xDF;
	asm("nop");
	if (GetBit(PINA,0)==0) { return('2'); }
	if (GetBit(PINA,1)==0) { return('5'); }
	if (GetBit(PINA,2)==0) { return('8'); }
	if (GetBit(PINA,3)==0) { return('0'); }

	PORTA = 0xBF;
	asm("nop");
	if (GetBit(PINA,0)==0) { return('3'); }
	if (GetBit(PINA,1)==0) { return('6'); }
	if (GetBit(PINA,2)==0) { return('9'); }
	if (GetBit(PINA,3)==0) { return('#'); }

	PORTA = 0x7F;
	asm("nop");
	if (GetBit(PINA,0)==0) { return('A'); }
	if (GetBit(PINA,1)==0) { return('B'); }
	if (GetBit(PINA,2)==0) { return('C'); }
	if (GetBit(PINA,3)==0) { return('D'); }
	return('\0'); 
}

enum states {init, get, off} state;
int Tick(int state)
{
	switch (state)
	{
		case init:
			if (x == 0x1F)
			{
				state = init;
			}
			else if (x != 0x1F)
			{
				state = get;
			}
			break;
		
		case get:
			if (x != 0x1F)
			{
				state = get;
			}
			else if (x == 0x1F)
			{
				state = off;
			}
			break;
		
		case off:
			state = init;
			break;
		
		default:
			state = init;
			x = 0x1F;
			break;
	}
	switch (state)
	{
		case init:
			x = GetKeypadKey();
			break;
		
		case get:
			x = GetKeypadKey();
			temp = GetKeypadKey();
			break;
		
		case off:
			temp2 = temp;
			break;
		
		default:
		break;
	}

	
	return state;
}

int main(void)
{
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	unsigned long int SMTick1_calc = 50;
	unsigned long int SMTick1_period = 50;

	static task task_LCD;
	task *tasks[] = { &task_LCD };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	task_LCD.state = -1;
	task_LCD.period = SMTick1_period;
	task_LCD.Time_Taken = SMTick1_period;
	task_LCD.TickFct = &Tick;
	
	TimerSet(50);
	TimerOn();
	
	
	DDRD = 0xFF; PORTD = 0x00;
	DDRA = 0xF0; PORTA = 0x0F;
	
	LCD_init();
	LCD_ClearScreen();
	LCD_Cursor(1);
	
	unsigned char Display_Letters = 0;
	while(1)
	{ 	
		// Scheduler code
		for ( unsigned short counter = 0; counter < numTasks; counter++ ) {
			// Task is ready to tick
			if ( tasks[counter]->Time_Taken ==
			tasks[counter]->period ) {
				// Setting next state for task
				tasks[counter]->state =
				tasks[counter]->TickFct(tasks[counter]->state);
				// Reset elapsed time for next tick.
				tasks[counter]->Time_Taken = 0;
			}
			tasks[counter]->Time_Taken += 1;
		}
		
	
		switch (temp)
		{
			LCD_ClearScreen();
			// All 5 LEDs on
			case '\0': PORTC = 0x1F; break;
			// hex equivalent
			case '1': PORTC = 0x01; Display_Letters = 1; LCD_Cursor(1); LCD_WriteData(Display_Letters + '0'); break;
			case '2': PORTC = 0x02; Display_Letters = 2; LCD_Cursor(1); LCD_WriteData(Display_Letters + '0'); break;
			case '3': PORTC = 0x03; Display_Letters = 3; LCD_Cursor(1); LCD_WriteData(Display_Letters + '0'); break;
			case '4': PORTC = 0x04; Display_Letters = 4; LCD_Cursor(1); LCD_WriteData(Display_Letters + '0'); break;
			case '5': PORTC = 0x05; Display_Letters = 5; LCD_Cursor(1); LCD_WriteData(Display_Letters + '0'); break;
			case '6': PORTC = 0x06; Display_Letters = 6; LCD_Cursor(1); LCD_WriteData(Display_Letters + '0'); break;
			case '7': PORTC = 0x07; Display_Letters = 7; LCD_Cursor(1); LCD_WriteData(Display_Letters + '0'); break;
			case '8': PORTC = 0x08; Display_Letters = 8; LCD_Cursor(1); LCD_WriteData(Display_Letters + '0'); break;
			case '9': PORTC = 0x09; Display_Letters = 9; LCD_Cursor(1); LCD_WriteData(Display_Letters + '0'); break;
			case '0': PORTC = 0x00; Display_Letters = 0; LCD_Cursor(1); LCD_WriteData(Display_Letters + '0'); break;
			// . . . ***** FINISH *****
			case 'A': PORTC = 0x0A; Display_Letters = 65; LCD_Cursor(1); LCD_WriteData(Display_Letters); break;
			case 'B': PORTC = 0x0B; Display_Letters = 66; LCD_Cursor(1); LCD_WriteData(Display_Letters); break;
			case 'C': PORTC = 0x0C; Display_Letters = 67; LCD_Cursor(1); LCD_WriteData(Display_Letters); break;
			case 'D': PORTC = 0x0D; Display_Letters = 68; LCD_Cursor(1); LCD_WriteData(Display_Letters); break;
			case '*': PORTC = 0x0E; Display_Letters = 42; LCD_Cursor(1); LCD_WriteData(Display_Letters); break;
			case '#': PORTC = 0x0F; Display_Letters = 35; LCD_Cursor(1); LCD_WriteData(Display_Letters); break;
			// Should never occur. Middle LED off.
			default: PORTC = 0x1B; break;
		

			while (!TimerFlag);
			TimerFlag = 0;
		}
	}
}












/*

#include <avr/interrupt.h>
#include <stdio.h>
#include "io.h"
//#include "io.c"
#include "bit.h"
#include "timer.h"
//#include "lcd_8bit_task.h"
//#include "scheduler.h"

char * string = "CS120B is Legend... wait for it DARY!";
unsigned char count = 0;
unsigned char output[16]; //was 16


typedef struct _task {
	
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;
	int (*TickFct)(int);
} task;



enum states {init} state;
int tick_funct(state)
{
	for (int j = 0; j < 16; ++j)
	{
		output[j] = string[(count+j) % 38];
	}
	count = (count + 1) % 38;
	LCD_DisplayString(1, output);
	return state;
}

int main(void) {
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	
	
	unsigned long int GCD = 20;
	
	unsigned long int SMTick1_period = 20;
	
	static task task1;
	task *tasks[] = { &task1 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	task1.state = -1;
	task1.period = SMTick1_period;
	task1.elapsedTime = SMTick1_period;
	task1.TickFct = &tick_funct;
	

	
	TimerSet(25);
	TimerOn();
	LCD_init();
	
	
	unsigned short i;
	while(1)
	{
		
		for ( i = 0; i < numTasks; i++ )
		{
			
			if ( tasks[i]->elapsedTime == tasks[i]->period )
			{
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		
		while (!TimerFlag);
		TimerFlag = 0;
	}
}

*/






/*
#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include "io.h"
#include "io.c"
#include "bit.h"
#include "timer.h"
#include "usart.h"
#include "scheduler.h"
#include "keypad.h"
#include "queue.h"
#include "seven_seg.h"

unsigned char x = 0x1F;
unsigned char get_value;


typedef struct _task {
	
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;
	//Task tick function
	int (*TickFct)(int);
} task;

unsigned char GetKeypadKey() {
	
	PORTC = 0xEF;
	asm("nop");
	if (GetBit(PINC,0)==0) { return('1'); }
	if (GetBit(PINC,1)==0) { return('4'); }
	if (GetBit(PINC,2)==0) { return('7'); }
	if (GetBit(PINC,3)==0) { return('*'); }
	
	
	PORTC = 0xDF;
	asm("nop");
	if (GetBit(PINC,0)==0) { return('2'); }
	if (GetBit(PINC,1)==0) { return('5'); }
	if (GetBit(PINC,2)==0) { return('8'); }
	if (GetBit(PINC,3)==0) { return('0'); }
	
	
	PORTC = 0xBF;
	asm("nop");
	if (GetBit(PINC,0)==0) { return('3'); }
	if (GetBit(PINC,1)==0) { return('6'); }
	if (GetBit(PINC,2)==0) { return('9'); }
	if (GetBit(PINC,3)==0) { return('#'); }
	
	
	PORTC = 0x7F;
	asm("nop");
	if (GetBit(PINC,0)==0) { return('A'); }
	if (GetBit(PINC,1)==0) { return('B'); }
	if (GetBit(PINC,2)==0) { return('C'); }
	if (GetBit(PINC,3)==0) { return('D'); }
	return('\0'); // default value
}

enum states {init, get} state;
int tick_funct(int state)
{
	switch (state)
	{
		case init:
		if (x == 0x1F)
		{
			state = init;
		}
		else if (x != 0x1F)
		{
			state = get;
		}
		break;
		
		case get:
		state = init;
		x = 0x1F;
		break;
		
		default:
		state = init;
		x = 0x1F;
		break;
	}
	switch (state)
	{
		case init:
		x = GetKeypadKey();
		break;
		
		case get:
		get_value = x;
		break;
		
		default:
		break;
	}
	
	return state;
}

int main(void)
{
	// PORTB set to output, outputs init 0s
	DDRB = 0xFF; PORTB = 0x00;
	// PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRC = 0xF0; PORTC = 0x0F;
	
	//Period for the tasks
	unsigned long int SMTick1_calc = 50;
	
	//Calculating GCD
	//DONT NEED TO DO
	
	//GCD of all tasks
	unsigned long int GCD = 50;
	
	//GCD periods for scheduler
	//unsigned long int SMTick1_period = SMTick1_calc/GCD;
	unsigned long int SMTick1_period = 50;
	
	//Declare an array of tasks
	static task task1;
	task *tasks[] = { &task1 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	//Task 1 -- ONLY TASK
	task1.state = -1;
	task1.period = SMTick1_period;
	task1.elapsedTime = SMTick1_period;
	task1.TickFct = &tick_funct;
	
	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();
	
	
	unsigned short i;
	while(1)
	{ 	
		// Scheduler code
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime ==
			tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state =
				tasks[i]->TickFct(tasks[i]->state);
				// Reset elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		
		//tick_funct();
		switch (get_value)
		{
			// All 5 LEDs on
			case '\0': PORTB = 0x1F; break;
			// hex equivalent
			case '1': PORTB = 0x01; break;
			case '2': PORTB = 0x02; break;
			case '3': PORTB = 0x03; break;
			case '4': PORTB = 0x04; break;
			case '5': PORTB = 0x05; break;
			case '6': PORTB = 0x06; break;
			case '7': PORTB = 0x07; break;
			case '8': PORTB = 0x08; break;
			case '9': PORTB = 0x09; break;
			case '0': PORTB = 0x00; break;
			// . . . ***** FINISH *****
			case 'A': PORTB = 0x0A; break;
			case 'B': PORTB = 0x0B; break;
			case 'C': PORTB = 0x0C; break;
			case 'D': PORTB = 0x0D; break;
			case '*': PORTB = 0x0E; break;
			case '#': PORTB = 0x0F; break;
			// Should never occur. Middle LED off.
			default: PORTB = 0x1B; break;
		

			while (!TimerFlag);
			TimerFlag = 0;
		}
	}
}
*/