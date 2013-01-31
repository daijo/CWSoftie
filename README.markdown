CWSoftie
========

A Arduino-based CW sidetone generator and keyer.

The sidetone is generated in software and converted to a analog signal via PWM and a low-pass RC-filter.

The end goal is a custom board/kit with a ATtiny MCU. In it's simplest configuration it shouldn't be much more complex than the common 555-timer based circuit practise oscillator but still deliver a smooth, clickless sine wave signal.

Features
--------

* Sine wave tone using PWM.
* Adjustable sidetone frequency 400-911Hz.
* Adjustable keyer speed 5-37wpm.
* Keyboard keyer.

Backlog
-------

* Keyer output.
* Iambic keyer.
* Gaussian wave envelope.
* Simple record and replay.

Schematic
---------

![CWSoftie schematic](https://farm9.staticflickr.com/8236/8431658887_03357e70f3_z.jpg)

Assembly
--------

![CWSoftie prototype](https://farm9.staticflickr.com/8332/8375597913_f57e401473_z.jpg)

Pins assignment
---------------

| Arduino pin | Attached to |
| :---------: | :---------: |
| A0 | Tone pot slider |
| A1 | Speed pot slider |
| D2 | Paddle/key 3.5mm stereo plug tip |
| D2 | 10K ohm to GND |
| D3 | PS/2 Keyboard CLK |
| D4 | Keyer output circuit |
| D5 | Paddle/key 3.5mm stereo plug ring |
| D5 | 10K ohm to GND |
| D7 | PS/2 Keyboard DATA |
| D11 | Low-pass filter |

Potentiometer end terminals connected to GND and VCC.

VCC to paddle/key 3.5mm stereo plug sleeve.

Low pass filter (692 Hz):

* 5K resistor attached from D11 (PWM) to output
* 0.047uF cap from output to ground
* output to amplifier

Keyer output circuit:

* D4 to Logic level N-channel MOSFET gate pin
* MOSFET source pin to GND
* MOSFET drain pin to keyer output

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
