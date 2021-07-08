# Tauba Auerbach

## Membrane Sculpture

A Solar powered kinetic sculpture that creates a membrane by drawing apart soaped ropes. The ropes are drawn apart, then brought back together in a cycle repeated for the duration of its exhibition.

Originally created for its debut exhibition at the ARtists Institute, this code is now being updated for exhibit at SFMoMA. Previously, setup and control was done on-site via a button matrix UI for the arduino, which was running the motion control and timing software.

The SFMoMA installation will eventually move to a single Raspberry Pi. In the transistion period, a bi-directional serial communication protocol will be developed so that adjustments to the sculpture can be made via a webapp running on a raspberry pi connected to the arduino fitted with the motordrivers. This configuration will be simplified to a single raspberry pi zero w fitted with a motor control shield. The raspberry pi will host a control interface accessible via the local network and VPN, and run the motion control software.

## Process for compiling and uploading an arduino sketch from the command line using `arduino-mk` (Raspberry Pi Tested)

The previous example works well on MacOSX, but when attempted on a Raspberry Pi 3 B+, several errors were encountered without clear paths to resolve. `arduino-mk` seems to be a viable solution for getting this to work on the Raspberry Pi.

### Install `arduino-mk`

* Install all the necessary packages: `sudo apt-get install build-essential arduino arduino-core arduino-mk`
* Change directory to the location of the `.ino` sketch: `cd /path/to/sketch/`
* Create a Makefile using `nano Makefile` that contains:

```
ARDUINO_DIR=/usr/share/arduino
BOARD_TAG=mega2560
ARDUINO_PORT=/dev/ttyACM0
ARDUINO_LIBS=
include /usr/share/arduino/Arduino.mk
```
* compile using `sudo make`
* upload using `sudo make upload`

[--source](http://www.raspberryvi.org/stories/arduino-cli.html)
