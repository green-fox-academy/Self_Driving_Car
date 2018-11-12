# Robotcar

## Description

A self-driving robot car, witch can follow red, green, blue, or yellow tracks using RGB sensors along with multiplexers.

The colour of the track carries the information of the speed limit regarding to the specified section of the path. A servo motor controls the steering, and an H-bridge drives the motor (closed-loop control system). The throttle and steering control is based on the datas measured by the RGB sensors and the Lidar module.

The car is also able to avoid collisions and stop if necessary, and overtake other vehicle. Moreover, the car should be able to do parallel parking.

## Main features

- servo steering control (PID)
- following different coloured tracks using RGB sensors
- overtaking using Lidar
- changing lanes using Lidar
- collision avoidance using Lidar sensor
- motor control (PI) with RPM measurement
- live diagnostic via Bluetooth
- manual control mode (RC) for testing
- informative and user-friendly human machine interface with the STM32F7 microcontrollers's LCD screen

## Technology

C
Eclipse - System Workbench - Version: Neon.3 Release (4.6.3) - Build id: 20170314-1500
STM32 CubeF7 Software - Version 1.12.0
STM32 GUI Framework
Testing with Unity, CMock, Ceedling
PWM, I2C communication and multiplexing

## Hardware

1. [STM32F746G-DISCOVERY board](https://www.st.com/content/ccc/resource/technical/document/datasheet/96/ed/61/9b/e0/6c/45/0b/DM00166116.pdf/files/DM00166116.pdf/jcr:content/translations/en.DM00166116.pdf)
2. [APDS-9960 - Digital Proximity, Ambient Light, RGB and Gesture Sensor](http://www.farnell.com/datasheets/2343967.pdf)
3. [Multiplexer](http://www.ti.com/lit/ds/symlink/tca9548a.pdf)
4. MG-945/995 servo
5. [Automotive fully integrated H-bridge motor driver](https://www.st.com/resource/en/datasheet/cd00043711.pdf)
6. [MHC-06/6 - Bluetooth sensor](https://www.hestore.hu/prod_getfile.php?id=8279)
7. [LIDAR (Light Detection and Ranging) sensor](https://static.garmin.com/pumac/LIDAR_Lite_v3_Operation_Manual_and_Technical_Specifications.pdf)

## Installation

1. Clone the project to the desired location on your computer.
2. Make sure that the [STM32CubeF7 library package](https://www.st.com/content/st_com/en/products/embedded-software/mcus-embedded-software/stm32-embedded-software/stm32cube-mcu-packages/stm32cubef7.html) is installed on your machine
    2.1. In the "GET SOFTWARE" section click on the "Get Software" button to download the package.
    2.2. An ST registration will be required.
    2.3. A .zip file will be downloaded, which contains a lot's of source files.
    2.4. Unzip it to a folder.
3. Create a new C project in your preferred IDE. This was created in the Eclipsed based System Workbench, so to make sure that it runs correctly we advice you, to use this platform.
4. Pick a project name, choose your working directory and from the Executable directory pick Ac6 STM32 MCU Project.
5. Click on Next, then repeat this one more time.
6. From the Series click on STM32F7, and from the Board choose STM32F746G-DISCO. Click on Next.
7. Choose the Hardware Abstraction Layer (Cube HAL) and then click on Finish.
8. From the cloned project copy the main.c to the newly created main.c and then the program is ready to be runned.

## Contribute

- pull request convention:
    - the project has to have the following branches: main, development and feature branches
    - each feature branch must be created from the development branch
    - every user story needs to have its own feature branch, it must have the same name as in the sprint (in JIRA) plus some additional easily recognizable words like: FLU-8 Bluetooth diagnostic
    - after finishing the task the pull request has to be made from the feature branch to the development branch as base branch and at least two colleagues have to be asked to review the code

- commit message convention: 
	- short (50 chars or less) summary of changes
	- in some contexts, the first line is treated as the subject of an email and the rest of the 	text as the body
	- use key words like features, fix, add, change etc.
	- for more details please check the following link: https://gist.github.com/stephenparish/
    
- branch naming conventions:
	- each story should have its own branch
	- use grouping tokens (words) at the beginning of your branch names
	- define and use short lead tokens to differentiate branches in a way that is meaningful to 	your workflow
	- use slashes to separate parts of your branch names
	- do not use bare numbers as leading parts
	- avoid long descriptive names for long-lived branches

## Documentations

- [github repository](https://github.com/green-fox-academy/Self_Driving_Car ) 
- [developer.md](https://github.com/green-fox-academy/Self_Driving_Car/blob/master/sf_car/docs/developer.md)
-[user.md](https://github.com/green-fox-academy/Self_Driving_Car/blob/master/sf_car/docs/user.md)

### Green Fox Academy, ALOPEX Cohort, Embedded Robotcar project, Fluorite team, 2018.
