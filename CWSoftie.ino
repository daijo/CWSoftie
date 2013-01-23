/*  
 * Copyright 2012 Daniel Hjort. All rights reserved.
 * Author: Daniel Hjort
 */

#include <stdint.h>
#include "avr/pgmspace.h"
#include <PS2Keyboard.h>

extern "C" {
#include "morsecode.h"
}

/* table of 256 sine values / one sine period / stored in flash memory */
const char PROGMEM sine256[]  = {
  127,130,133,136,139,143,146,149,152,155,158,161,164,167,170,173,176,178,181,184,187,190,192,195,198,200,203,205,208,210,212,215,217,219,221,223,225,227,229,231,233,234,236,238,239,240,
  242,243,244,245,247,248,249,249,250,251,252,252,253,253,253,254,254,254,254,254,254,254,253,253,253,252,252,251,250,249,249,248,247,245,244,243,242,240,239,238,236,234,233,231,229,227,225,223,
  221,219,217,215,212,210,208,205,203,200,198,195,192,190,187,184,181,178,176,173,170,167,164,161,158,155,152,149,146,143,139,136,133,130,127,124,121,118,115,111,108,105,102,99,96,93,90,87,84,81,78,
  76,73,70,67,64,62,59,56,54,51,49,46,44,42,39,37,35,33,31,29,27,25,23,21,20,18,16,15,14,12,11,10,9,7,6,5,5,4,3,2,2,1,1,1,0,0,0,0,0,0,0,1,1,1,2,2,3,4,5,5,6,7,9,10,11,12,14,15,16,18,20,21,23,25,27,29,31,
  33,35,37,39,42,44,46,49,51,54,56,59,62,64,67,70,73,76,78,81,84,87,90,93,96,99,102,105,108,111,115,118,121,124
};

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

#define TONE_POT_PIN 0
#define SPEED_POT_PIN 1
#define KEY_PIN 2
#define KEYBOARD_CLK_PIN 3
#define LED_PIN 4
#define KEYBOARD_DATA_PIN 7
#define SOUND_PIN 11

#define KEYBOARD_BUFFER_LENGTH 50

void Setup_timer2();

PS2Keyboard keyboard;

// Pins
const uint8_t tonePotPin = TONE_POT_PIN;
const uint8_t speedPotPin = SPEED_POT_PIN;
const uint8_t keyPin = KEY_PIN;
const uint8_t keyboardClkPin = KEYBOARD_CLK_PIN;
const uint8_t ledPin = LED_PIN;
const uint8_t keyboardDataPin = KEYBOARD_DATA_PIN;
const uint8_t soundPin = SOUND_PIN;

// Keyboard buffer
char keyboardBuffer[KEYBOARD_BUFFER_LENGTH];
uint16_t nextWrite = 0;
uint16_t nextRead = 0;

// Key
bool ledState = HIGH;  // the current state of the output pin
bool keyState;

// Keyer
typedef enum { KEYER_IDLE, KEYER_DOWN, KEYER_UP } KeyerState;
KeyerState keyerState = KEYER_IDLE;
morsecode_t keyerQueue;
uint8_t currentElement = 0;
uint8_t speed = 20;
uint8_t dotMs = (uint16_t)(1000 * ham_morse_element_time(DOT, speed, STD_WORD_PARIS_LEN));
volatile int16_t keyerMsLeft = 0;
uint8_t elementsLeft = 0;

double dfreq;
/* const double refclk=31372.549;  // =16MHz / 510 */
const double refclk=31376.6;       // measured

/*
 * variables used inside interrupt service declared as volatile
 */
volatile byte icnt;              // var inside interrupt
volatile byte icnt1;             // var inside interrupt
volatile byte icnt2;             // var inside interrupt
volatile byte c4ms;              // counter incremented all 4ms
volatile unsigned long phaccu;   // phase accumulator
volatile unsigned long tword_m;  // DDS tuning word m
volatile bool makeTone = false;

static void toneOn()
{
  makeTone = true;
  digitalWrite(ledPin, true);
}

static void toneOff()
{
  makeTone = false;
  digitalWrite(ledPin, false);
}

/*
 *  Keyer functions
 */
static void keyerDown(char element)
{
  keyerState = KEYER_DOWN;

  if(element == '.') {
    keyerMsLeft = dotMs;
  } else if(element == '-') {
    keyerMsLeft = dotMs * 3;
  }

  toneOn();
}

static void startKeyer(morsecode_t code)
{
  keyerQueue = code;

  currentElement = 0;
  keyerDown(keyerQueue[currentElement]);
  elementsLeft = strlen(keyerQueue);
}

static void clearKeyer()
{
  currentElement = 0;
  elementsLeft = 0;

  if (keyerState == KEYER_DOWN) {
    toneOff();
  }

  keyerState = KEYER_IDLE;
}

static void keyerLoop()
{

  if (keyerMsLeft < 0) {

    if (keyerState == KEYER_DOWN) {
      toneOff();
      keyerState = KEYER_UP;
      keyerMsLeft = dotMs;
    } else if (keyerState == KEYER_UP
                 && elementsLeft > 0) {
      currentElement++;
      elementsLeft--;
      keyerDown(keyerQueue[currentElement]);
    } else {
      clearKeyer();
    }
  }
}

/*
 *  Buffer functions
 */
static void clearBuffer()
{
  nextWrite = 0;
  nextRead = 0;
}

static char readFromBuffer()
{

  char result = 0;

  if(nextRead < nextWrite) {
    result = keyboardBuffer[nextRead++];
  }

  return result;
}

static void writeToBuffer(char c)
{

  if (nextWrite == KEYBOARD_BUFFER_LENGTH) {
    nextWrite = 0;
    nextRead = 0;
  }

  keyboardBuffer[nextWrite] = c;
  nextWrite += 1;
}

void setup() {

  pinMode(keyPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(soundPin, OUTPUT);     // PWM  output

  Setup_timer2();

  /* disable interrupts to avoid timing distortion */
  cbi (TIMSK0,TOIE0);                    // disable Timer0, delay() and millis() is now not available
  sbi (TIMSK2,TOIE2);                    // enable Timer2 Interrupt

  dfreq = 440.0;                         // initial output frequency = 1000.o Hz
  tword_m = pow(2,32) * dfreq / refclk;  // calulate DDS new tuning word
  
  keyboard.begin(keyboardDataPin, keyboardClkPin);
  
  Serial.begin(9600);
  Serial.println("CWSoftie starting...");
}

void loop() {

  bool keyNewState = digitalRead(keyPin);

  if (c4ms > 100) {                         // timer / wait
    c4ms = 0;
    
    double newFreq = (analogRead(tonePotPin) / 2) + 400; // on analog to adjust output frequency from 400..911 Hz

    if (fabs(newFreq - dfreq) > 5) {
      dfreq = newFreq;
      toneOff();                    // disable Timer2 Interrupt
      tword_m = pow(2,32) * dfreq / refclk;  // calulate DDS new tuning word
      toneOn();                    // enable Timer2 Interrupt
    }
  }

  if (keyNewState != keyState) {
    keyState = keyNewState;
    clearBuffer();
    clearKeyer();
    if (keyState) {
      toneOn();
    } else {
      toneOff();
    }
  }
  
  if (keyboard.available()) {

    // read the next key
    char c = keyboard.read();
    if (c != ' ') {
      char* code = ham_morse_from_ascii(c);
      writeToBuffer(c);
    }
  }

  if (!keyState
        && nextWrite > nextRead
        && keyerState == KEYER_IDLE) {
      speed = (analogRead(speedPotPin) / 32) + 5; // 1023 / 32
      dotMs = (uint16_t)(1000*ham_morse_element_time(DOT, speed, STD_WORD_PARIS_LEN));
      char nextChar = readFromBuffer();
      if(nextChar != 0) {
        startKeyer(ham_morse_from_ascii(nextChar));
      }
  }

  if(keyerState != KEYER_IDLE) {
    keyerLoop();
  }
}

/*
 * timer2 setup
 * set prscaler to 1, PWM mode to phase correct PWM,  16000000/510 = 31372.55 Hz clock
 */
void Setup_timer2() {

  // Timer2 Clock Prescaler to : 1
  sbi(TCCR2B, CS20);
  cbi(TCCR2B, CS21);
  cbi(TCCR2B, CS22);

  // Timer2 PWM Mode set to Phase Correct PWM
  cbi(TCCR2A, COM2A0);  // clear Compare Match
  sbi(TCCR2A, COM2A1);

  sbi(TCCR2A, WGM20);   // Mode 1  / Phase Correct PWM
  cbi(TCCR2A, WGM21);
  cbi(TCCR2B, WGM22);
}

/*
 * Timer2 Interrupt Service at 31372,550 KHz = 32uSec
 * this is the timebase REFCLOCK for the DDS generator
 * FOUT = (M (REFCLK)) / (2 exp 32)
 * runtime : 8 microseconds ( inclusive push and pop)
 */
ISR(TIMER2_OVF_vect) {

  if (makeTone) {

    phaccu = phaccu + tword_m; // soft DDS, phase accu with 32 bits
    icnt = phaccu >> 24;       // use upper 8 bits for phase accu as frequency information
                             // read value fron ROM sine table and send to PWM DAC

    OCR2A = pgm_read_byte_near(sine256 + icnt);
  }

  if (icnt2++ == 31) {
    keyerMsLeft--;
    icnt2=0;
  }

  if(icnt1++ == 125) {  // increment variable c4ms all 4 milliseconds
    c4ms++;
    icnt1=0;
  }
}
