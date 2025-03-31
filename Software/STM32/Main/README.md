# Main code of the Drone

*Code assembled by Archenault Maël and Phil Rigling*

**PLEASE DO NOT EDIT THIS PROJECT. WE WILL TAKE CARE OF IMPLEMENTING EVERY CHANGE ON PERIPHERALS CODE IN THE MAIN CODE**


## Main Principle of the code

This code will run in the STM32 embedded in the drone. It has to assume the following functions:
- handle all sensors measurements
- manage the command sent by the controller (PC)
- compute motor control according to control theory (PIDs)
- control the motors

All of these functions are executed in a loop with this order. The loop has a period of **825 µs** (triggered by the EOC of TIM3)

Let's review each one of these functions


### Sensor measurements

We have 3 different inputs:
- an IMU (for angles)
- an ultrasound sensor (for height)
- a RF antenna (for command reception)

### Commands management

We agreed with the controller team to settle a protocol for command transmission.

Via the RF antenna, we receive a string of this format:
- **"${up}{down}{forward}{backward}{left}{right}{turn left}{turn right}"**

where {x} is either 0 or 1 to describe the key that has been pressed

For **emergency stop** the controller needs to send:
- **"stop"**


### Control of angles and altitude

For this part, we implemented a custom structure that serves as a PID. That way, we can create one object for each variable to control (pitch, roll, yaw, height).
These object are able to differentiate and integrate the errors set as input.

The output of the pid controllers is : $u = K_p.e +K_i.\int e .dt + K_d.\frac{de}{dt}$

For the next courses, we will need to fine tune the parameters (Kp, Kd, and Ki)

### Motor Control
For motor control, we have set each of the 4 motors to the **4 channels of TIM1**
We are using the *motor mixing* algorithm to transform all of the commands (u) into a percentage of power for each of the 4 motors.


## Table of wiring
CAUTION: these tables applies to the STM32l476rg
### Ultrasound

| Sensor Pin  | Nucleo Pin | Purpose |
|-------|-----|---------|
| +5V |  +5V| Alimentation of the sensor|
| GND  |  GND| Mass of the sensor  |
| Trig   |  CN10 pin 4 (male pins)| Send a pulse to the sensor to trigger the measurement |
|Echo|D10|High state on this pin is proportional to the distance measured|


### Motors

| Sensor Pin  | Nucleo Pin | Purpose |
|-------|-----|---------|
| PWM Motor Front Right | D7| Control motor power|
| PWM Motor Front Left  | D8| Control motor power  |
| PWM Motor Back Right   |  D2 | Control motor power|
|PWM Motor Back Left|CN10 pin 14 (male pins)|Control motor power|

### IMU

| Sensor Pin  | Nucleo Pin | Purpose |
|-------|-----|---------|
| VCC | +5V| Alimentation for the sensor|
| GND  | GND| Mass of the sensor  |
| SDA   |  CN10 pin 18 (male pins)| Data of I2C connection|
|SCL|D6|Clock of I2C connection|

### Nrf24l01

| Sensor Pin  | Nucleo Pin | Purpose |
|-------|-----|---------|
| VCC | +3.3V| Alimentation for the sensor|
| GND  | GND| Mass of the sensor  |
| MISO  |  CN7 pin 35 (male pins) | Master In Slave Out|
|MOSI|CN7 pin 37 (male pins)|Master Out Slave In|
|SCK|CN10 pin 30 (male pins)|Clock of the sensor|
|CSN |D9|Chip Select Not|
|CE|CN10 pin 2 (male pins)|Chip Enable|
|IRQ|CN10 pin 1 (male pins)|Activates the interrupt at reception|











