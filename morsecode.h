/*
* Copyright 2013 Daniel Hjort. All rights reserved.
* Author: Daniel Hjort
*/


#ifndef __MORSECODE_H__
#define __MORSECODE_H__

#include <inttypes.h>

#define STD_WORD_PARIS_LEN 50
#define STD_WORD_CODEX_LEN 60

/*
 * Text representation of morse code.
 *
 * dot .
 * dash -
 * gap between charachters c
 * gap between words w
 */

typedef char* morsecode_t;

typedef enum {
   DOT      = '.',
   DASH     = '-',
   CHAR_GAP = 'c',
   WORD_GAP = 'w'
} MorseCodeElement;

/*enum MorseProSign {

}*/

morsecode_t ham_morse_from_ascii(char character);

char ham_morse_to_ascii(morsecode_t code);

double ham_morse_element_time(MorseCodeElement element, uint8_t words_per_minute, uint8_t std_word_length);

#endif
