#!/usr/bin/env python
# -*- coding: utf-8 -*-

'''
 Chaque seconde, un Raspberry Pi muni d'un module nRF24L01
 émet un nombre.
 Le message peut ensuite être capté par une carte Arduino,
 ou par un autre Raspberry Pi.
 http://electroniqueamateur.blogspot.ca/2017/02/communication-entre-raspberry-pi-et.html
'''

import time
from RF24 import *
import RPi.GPIO as GPIO

radio = RF24(22, 0);

adresse = 0x1111111111

compteur = 0

print('Emetteur Raspi vers Arduino')
radio.begin()

print('Pret a envoyer les données')
radio.openWritingPipe(adresse)

while 1:
    compteur = compteur + 1
    print('Envoi du message = {} '.format(compteur))
    buffer = str(compteur).encode('utf-8')
    radio.write(buffer)
    time.sleep(1)
