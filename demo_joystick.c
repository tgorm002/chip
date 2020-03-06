#include <avr/io.h>
#include <avr/interrupt.h>


enum states{init, user_input} state;

void tick() {
  switch(state) { //transitions
    case init:
      state = user_input;
      break;
    case user_input:
      if(A0 == 0 && A1 == 0) { //in middle
        PORTB = 1;
      }
      else if(A0 > 0) { //went right
        PORTB = 8; 
      }
      else if(A0 < 0) { //went left
        PORTB = 16;
      }
      else if(A1 > 0) { //went up
          PORTB = 2;
      }
      else if(A1 < 0) { //went down
          PORTB = 4;
      }
  }
}


int main() {
  DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
  DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

  while(1) {
		tick();
	}

}



