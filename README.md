# 📡 Communication module NRF24L01 + Human Machine Interface 👨🏽   
Code written by:  
- [CARVALHO FONTES Hugo](https://github.com/HugouShare)
- [CORDI Hugo](https://github.com/Lynxlegrand)
  
## 📌 Project Description  
This part of the project is dedicated to the Human Machine Interface and the communication module: NRF24L01.  
It will be used in the project to transmit the user commands from the Human Machine Interface to the drone.  

## 💻 Project architecture  
The organisation of this part is the following one:  
- PC: contains the ***Human Machine Interface*** written on Python with Tkinter library
- STM32 -> Software_V3_Emitter: the ***C code*** allowing us to ***recover the user commands and send it through the NRF24L01 module***
- STM32 -> Software_V3_Receiver: the ***C code*** allowing the drone to ***receive the user commands through the NRF24L01 module***

## ... Next part ...  
The next part of the project consists in the integration of all the peripherals + communication module C codes.  
To have more information go to: [***Main code of the Drone***](https://github.com/Lynxlegrand/EnseaDronesOption/tree/main/Software/STM32/Main).
