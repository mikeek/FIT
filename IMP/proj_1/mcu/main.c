/*******************************************************************************
   main.c: Dekoder Morseovy abecedy
   Copyright (C) 2014 Brno University of Technology,
                      Faculty of Information Technology
   Author(s): Michal Kozubik <xkozub03 AT stud.fit.vutbr.cz>
   Original

   LICENSE TERMS

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
   3. All advertising materials mentioning features or use of this software
      or firmware must display the following acknowledgement:

        This product includes software developed by the University of
        Technology, Faculty of Information Technology, Brno and its
        contributors.

   4. Neither the name of the Company nor the names of its contributors
      may be used to endorse or promote products derived from this
      software without specific prior written permission.

   This software or firmware is provided ``as is'', and any express or implied
   warranties, including, but not limited to, the implied warranties of
   merchantability and fitness for a particular purpose are disclaimed.
   In no event shall the company or contributors be liable for any
   direct, indirect, incidental, special, exemplary, or consequential
   damages (including, but not limited to, procurement of substitute
   goods or services; loss of use, data, or profits; or business
   interruption) however caused and on any theory of liability, whether
   in contract, strict liability, or tort (including negligence or
   otherwise) arising in any way out of the use of this software, even
   if advised of the possibility of such damage.

   $Id$


*******************************************************************************/

#include <fitkitlib.h>
#include <keyboard/keyboard.h>
#include <lcd/display.h>
#include <string.h>

#define MAX_CODE_LEN  6
#define MORSE_CODES 26
#define INVALID_CODE 0

#define LONG_PRESS_INTERVAL 0x2000  // delay pro dlouhy stisk
#define END_CHAR_INTERVAL 	  0x6000  // delay pro pauzu mezi znaky 

#define TIMER_STOP TACTL =   TACLR // zastaveni casovace
#define TIMER_SET(_delay_)  \
	TACTL = TASSEL_1 + MC_2; \
	CCR0 = (_delay_)

	
char last_ch;    // naposledy precteny znak

char input[MAX_CODE_LEN] = {0}; // nactena sekvence tecek/carek
int input_valid = 1;
int input_pos = 0; // pozice v nacitanem znaku

int lcd_char_cnt = 0;
	
enum kb_events { // stavy dekoderu
	KB_PRESSED,
	KB_RELEASED,
	KB_NONE
};

enum timer_result_e {
	TR_LONG,
	TR_END,
	TR_NONE
};

int timer_result = TR_NONE; // vysledek preruseni casovace

int event = KB_NONE; // aktualni stav

static const char *morse[MORSE_CODES] = { // Kody Morseovy abecedy (bez CH)
	".-", "-...", "-.-.", "-..", ".", "..-.", "--.",
	"....", "..",    ".---", "-.-", ".-..", "--",
	"-.", "---", ".--.", "--.-", ".-.", "...", "-",
	"..-", "...-", ".--", "-..-", "-.--", "--.."
};

/*******************************************************************************
 * Dekodovani zadaneho znaku
*******************************************************************************/
unsigned char decode_morse_code()
{
	unsigned char i;
	for (i = 0; i < MORSE_CODES; ++i) {
		if (!strcmp(input, morse[i])) {
			return i + 65; // prvni znak v ascii (A) ma hodnotu 65
		}
	}
	
	return INVALID_CODE;
}

/*******************************************************************************
 * Vypis uzivatelske napovedy (funkce se vola pri vykonavani prikazu "help")
*******************************************************************************/
void print_user_help(void)
{
}

/*******************************************************************************
 * Reset vstupniho pole
*******************************************************************************/
void input_reset()
{
	int i;
	for (i = 0; i < MAX_CODE_LEN; ++i) {
		input[i] = '\0'; // vynulovani pole
	}
	
	input_valid = 1; // reset validity flagu
	input_pos = 0;  // nastaveni pozice na zacatek pole
}

/*******************************************************************************
 * Pridani znaku do vstupniho pole
*******************************************************************************/
void input_add(char c)
{
	if (!input_valid || input_pos >= MAX_CODE_LEN) {
		input_valid = 0; // vstup je delsi nez max. delka kodu, neni validni
		return;
	}
	
	input[input_pos++] = c;
}

/*******************************************************************************
 * Kratky stisk tlacitka (tecka)
*******************************************************************************/
void short_press()
{
	input_add('.');
}

/*******************************************************************************
 * Dlouhy stisk tlacitka
*******************************************************************************/
void long_press()
{
	input_add('-');
}

/*******************************************************************************
 * Konec zadavani kodu znaku
*******************************************************************************/
void end_char()
{
	unsigned char decoded;
	if (input_valid) { // pouze pokud je validni vstup
		decoded = decode_morse_code(); // dekoduj morseuv kod
		if (decoded != INVALID_CODE) {   // kod odpovida nejakemu znaku
			if (lcd_char_cnt == LCD_CHAR_COUNT - 1) { // displej je zaplnen
				LCD_send_cmd(LCD_CURSOR_DISPLAY_SHIFT | LCD_SHIFT_DISPLAY | LCD_SHIFT_DIR_LEFT, 0);
				// LCD_send_cmd(LCD_ENTRY_MODE_SET | LCD_CURSOR_MOVE_INC, 0);
			} else {				
				lcd_char_cnt++;
			}
			
			LCD_append_char(decoded);
		}
	}
	
	input_reset(); // reset vstupu
}

/*******************************************************************************
 * Obsluha klavesnice
*******************************************************************************/
int keyboard_idle()
{
	char ch;
	ch = key_decode(read_word_keyboard_4x4());
	if (ch != last_ch)  {
		last_ch = ch;
		TIMER_STOP;
		if (ch == 0) {
			if (event == KB_PRESSED) { // tlacitko stisknute kratce, casovac jeste nevyvolal preruseni pro long_press
				short_press();
			}
			
			event = KB_RELEASED;
			TIMER_SET(END_CHAR_INTERVAL);
		} else {
			event = KB_PRESSED;
			TIMER_SET(LONG_PRESS_INTERVAL);
		}
	}
	
	return 0;
}

/*******************************************************************************
 * Dekodovani a vykonani uzivatelskych prikazu
*******************************************************************************/
unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{
  return CMD_UNKNOWN;
}

/*******************************************************************************
 * Inicializace periferii/komponent po naprogramovani FPGA
*******************************************************************************/
void fpga_initialized()
{
	LCD_init();
	keyboard_init();
}


/*******************************************************************************
 * Hlavni funkce
*******************************************************************************/
int main(void)
{
	initialize_hardware();
	WDG_stop();  // zastav watchdog
	
	CCTL0 = CCIE; // povol preruseni pro casovac (rezim vystupni komparace)
	TACTL  = 0;     // pozastaveni casovace
	
	while (1) {    // hlavni cyklus
		switch (timer_result) { // bylo vyvolano preruseni casovace? jaky je vysledek?
		case TR_LONG:
			long_press();
			timer_result = TR_NONE;
			break;
		case TR_END:
			end_char();
			timer_result = TR_NONE;
			break;
		default:
			break;
		}
						
		keyboard_idle(); // obsluha klavesnice
		terminal_idle();   // obsluha terminalu
	}         
}

/*******************************************************************************
 * Preruseni casovacem
*******************************************************************************/
interrupt (TIMERA0_VECTOR) Timer_A (void)
{
	switch (event) {
	case KB_PRESSED:
		timer_result = TR_LONG;
		break;
	case KB_RELEASED:
		timer_result = TR_END;
		break;
	default:
		break;
	}
	
	event = KB_NONE;  // event je zpracovan
	TIMER_STOP;
}

