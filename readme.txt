Robotcar

Description

A self-driving robot car, witch can follow red, green, blue, or yellow tracks using RGB sensors along with multiplexers.

The colour of the track carries the information of the speed limit regarding to the specified section of the path. A servo motor controls the steering, and an H-bridge drives the motor (closed-loop control system). The throttle and steering control is based on the datas measured by the RGB sensors and the Lidar module.

The car is also able to avoid collisions and stop if necessary, and overtake other vehicle. Moreover, the car should be able to do parallel parking.

Main features

- servo steering control (PID)
- following different coloured tracks using RGB sensors
- overtaking using Lidar
- changing lanes using Lidar
- collision avoidance using Lidar sensor
- motor control (PI) with RPM measurement
- live diagnostic via Bluetooth
- manual control mode (RC) for testing
- informative and user-friendly human machine interface with the STM32F7 microcontrollers's LCD screen

Technology

C
Eclipse - System Workbench
STM32F7 HAL Library
STM32 GUI Framework
Testing with Unity, CMock, Ceedling
PWM, I2C communication and multiplexing

Hardware

STM32F746G-DISCOVERY board
APDS-9960 - Digital Proximity, Ambient Light, RGB and Gesture Sensor
TCA9548A Low-Voltage 8-Channel I2C Switch with Reset
Automotive fully integrated H-bridge motor driver
Servo motor control
Bluetooth sensor
LIDAR (Light Detection and Ranging) sensor

Installation

Contribute

- pull request????
- commit message convention: 
	- short (50 chars or less) summary of changes
	- in some contexts, the first line is treated as the subject of an email and the rest of the 	text as the body
	- use key words like features, fix, add, change etc.
	- for more details please check the following link: https://gist.github.com/stephenparish/	9941e89d80e2bc58a153
- branch naming conventions:
	- each story should have its own branch
	- use grouping tokens (words) at the beginning of your branch names
	- define and use short lead tokens to differentiate branches in a way that is meaningful to 	your workflow
	- use slashes to separate parts of your branch names
	- do not use bare numbers as leading parts
	- avoid long descriptive names for long-lived branches

Docs
- 2 links???
- github repository: https://github.com/green-fox-academy/Self_Driving_Car 

Green Fox Academy, ALOPEX Cohort, Embedded Robotcar project, Fluorite team, 2018.


