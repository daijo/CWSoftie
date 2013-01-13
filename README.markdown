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

Assembly
--------

![CWSoftie prototype](https://farm9.staticflickr.com/8332/8375597913_f57e401473_z.jpg)

Pins assignment
---------------

| Arduino pin | Attached to |
| :---------: | :---------: |
| A0 | Pot slider |
| D2 | Key to VCC |
| D2 | 10K ohm to GND |
| D4 | LED |
| D11 | Low-pass filter |

Potentiometer end terminals connected to GND and VCC.

Low pass filter (692 Hz):

* 5K resistor attached from D11 (PWM) to output
* 0.047uF cap from output to ground
* output to amplifier

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
