CWSoftie
========

A Arduino-based CW sidetone generator.

The sidetone is generated in software and converted to a analog signal via PWM and a low-pass RC-filter.

The end goal is a custom board/kit with a ATtiny MCU. In it's simplest configuration it shouldn't be much more complex than the common 555-timer based circuit practise oscillator but still deliver a smooth, clickless sine wave signal.

Features
--------

* Sine wave tone using PWM.
* Adjustable sidetone frequency 0-1023Hz.

Backlog
-------

* Gaussian wave envelope.
* Simple record and replay.
* Keyer output.

BOM
---

TBD

Circuit
-------

### Using an Arduino board:

Input and non-audio output:

 * LED attached from pin 4 to ground
 * Key attached from pin 2 to +5V
 * 10K resistor attached from pin 2 to ground
 * Center pin of potentiometer connected to analog pin 0.

Low pass filter (692 Hz):

* 5K resistor attached from pin 11 (PWM) to output
* 0.047uF cap from output to ground
* output to amplifier

### Custom board

TBD

Build
-----

Requires the Arduino IDE installed in ARDUINODIR.

    export ARDUINODIR=/Users/daijo/Code/Arduino/
    export SERIALDEV=/dev/tty.usbserial-A6007ZuZ
    export BOARD=atmega328
    make
    make upload

For the ATtiny custom board/kit you also need the arduino-tiny additions found at <http://code.google.com/p/arduino-tiny/>. 
You can also use my fork of Arduino with arduino-tiny <https://github.com/daijo/Arduino>.

    export BOARD=at2313at1
    export ARDUINOPORT=tiny

Reference
---------

* <http://www.atmel.com/Images/doc2542.pdf>
* <http://interface.khm.de/index.php/lab/experiments/arduino-dds-sinewave-generator/>
* <http://www.w8ji.com/cw_bandwidth_described.htm>
* <http://www.sm5bsz.com/txmod/rt0282eng.htm>
* <http://www.solorb.com/elect/hamcirc/sidetone/> 
