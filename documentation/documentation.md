# Tauba Auerbach

## Membrane Sculpture

![](images/membrane_diagram.jpg)

A Solar powered kinetic sculpture that creates a membrane by drawing apart soaped ropes. The ropes are drawn apart, then brought back together in a cycle repeated for the duration of its exhibition.

## Plan

An Arduino will be running code to control the speed of a peristaltic pump, and two gear head motors with encoders for feedback.

### Connections

* Arduino > via Arduino Shield > Gear Head Motors
	* Gear Head Motor Encoders > Arduino
* Arduino > via UART Serial > Peristaltic pump

## Parts

1. [Arduino UNO v3](https://www.pololu.com/product/2191)
2. [Gear Head Motors](https://www.pololu.com/product/2827)
3. [Dual G2 Arduino Shield](https://www.pololu.com/product/2517)
4. [Peristaltic pump](https://www.amazon.com/gp/product/B071F46P44/ref=ppx_yo_dt_b_asin_title_o05_s00?ie=UTF8&psc=1)


## Motor & Control Resources

### Dual G2 Controller

* [Product Page](https://www.pololu.com/product/2517)
* [Arduino Libraries](https://github.com/pololu/dual-g2-high-power-motor-shield)
* [Product Guide](https://www.pololu.com/docs/0J72/all) - READ FIRST!

### Pololu #2827 Gear Head

* [Product Page](https://www.pololu.com/product/2827)
* [Issues with MEGA Interrupts](https://forum.arduino.cc/index.php?topic=158434.0)

### EZO PMP

* [Product Page](https://www.atlas-scientific.com/product_pages/peristaltic/ezo-pmp-kit.html)
* [Code for Arduino MEGA](www.atlas-scientific.com/_files/code/ino_files/arduino_mega_PMP_sample_code.zip)
* [Code for Arduino UNO](https://www.atlas-scientific.com/_files/code/ino_files/arduino_UNO_PMP_sample_code.zip)

## Hours, Tasks, Notes:

### April 2nd 2019 (5 hrs)

* 1-6pm
	* Test EZO-PMP with Arduino MEGA - check
	* Test DUAL G2 with Arduino MEGA - Works
	* Test buttons - Wrote code to use a 4x4 button matrix as input
	* Test speed calibration - Buttons increment speed and display to monitor

**To Do:** write easing code, write code for and test encoder readability using interrupts.

### March 31st 2019 (2 hrs)

* 4-6pm Downloading and reviewing pump code for UNO and MEGA, Researching potential issues with reading the encoder input from the two gear head motors.

### March 29th 2019

#### Hours and Tasks (2 hrs)

* 10-11am Writing calibration code
* 4:30-5:30 Writing calibration code 

### March 28th 2019 

#### Hours and Tasks (4.5 hrs)

* 1-2pm Restructuring project documentation, publishing code on [github](https://github.com/phillipdavidstearns/auerbach-membrane)
* 4-5pm Setting up calibration code
* 11:30pm-1am Writing calibration code

### March 27th 2019

#### Hours and Tasks (5.5 hrs)

* 12-3pm Gathering and reviewing material on the Dual G2 board
* 5:30-8pm Soldering Dual G2 board, Flashing Arduino, Testing Dual G2 with 1 Motor

#### Notes

My Arduino UNO needed some fixing up. I damaged the original 328 chip in a previous project and had replaced with a 328 that was flashed for use in a different Arduino board. Used the USBtinyISP to 
flash the chip for use with the UNO board:

1. Start Arduino IDE
2. Connect USBtinyISP to Arduino UNO using 6 contact ribbon cable
3. Connect USBtinyISP to laptop via USB
4. Select `Tools > Programmer > USBtinyISP`
5. Select `Tools > Board > Arduino/Genuino Uno`
6. Click `Tools > Burn Bootloader`
7. Disconnect USBtinyISP from USB and from Arduino UNO board
8. Connect Arduino UNO to laptop via USB
9. Select `File > Examples > 01.Basics > Blink`
10. Click `upload` or press `command+u` to upload program

Dual G2 board came with some assembly required. Soldered it up.  No problem.

The Pololu 2827 motors (2827 from here on out) have pretty straight forward connections:

* RED: +V
* BLACK: -V
* GREEN: Encoder Ground
* BLUE: Encoder +V
* WHITE: Encoder A Output
* YELLOW: Encoder B Output

Testing the basic components and system:

1. Connected Dual G2 to Arduino UNO
2. Connected Dual G2 to bench supply: TENMA 72-8690-A
3. Connected 2827 motor to Dual G2
4. Installed Dual G2 libraries:
	1. Select `Sketch > Include Library > Manage Libraries...`
	2. Enter `DualG2HighPowerMotorShield` into search field
	3. Select the library
	4. Click `install`
5. Loaded Dual G2 demo code onto Arduino:
	1. Select `Files > Examples > DualG2HighPowerMotorShield > Demo`
	2. Click `upload` or press `command+u` to upload program
6. Turned on power supply

Motor spins up and reverses. Code seems pretty straight forward. It's possible to speed up and slow down the motor incrementally for smoothed acceleration and deceleration. Seems the the motor draws at most, 1 A in hard turn around situations where it is reversing direction.

#### To Do:

* Detect the encoder pulses and determine the appropriate resolution to use for control
* Write a sketch that allows for a specific selection of speed
* Test the peristaltic pump (over serial) and Dual G2 compatibility

### March 22nd 2019

#### Hours and Tasks (2 hrs)

* 1-2pm (1hr) Call
* 2pm-3pm (1hr) Research pump and compatibility with Arduino shield.