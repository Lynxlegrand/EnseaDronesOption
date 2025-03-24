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
- **"$11111111"**


### Control of angles and altitude

For this part, we implemented a custom structure that serves as a PID. That way, we can create one object for each variable to control (pitch, roll, yaw, height).
These object are able to differentiate and integrate the errors set as input.

The output of the pid controllers is : $u = K_p.e +K_i.\int e .dt + K_d.\frac{de}{dt}$

For the next courses, we will need to fine tune the parameters (Kp, Kd, and Ki)

### Motor Control
For motor control, we have set each of the 4 motors to the **4 channels of TIM1**
We are using the *motor mixing* algorithm to transform all of the commands (u) into a percentage of power for each of the 4 motors.





