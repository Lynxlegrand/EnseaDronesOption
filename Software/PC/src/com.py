import serial
import time
import serial.tools.list_ports
from const import *

ser = None
dico_key_pressed = {}
pid_values_to_change = []
emergency_stop = False

def initialize_connection():
    try:
        # Open the serial port

        port = get_available_ports()[0].split('(')[1][:-1]       #get the port in a chain of character
        global ser
        if ser != None:
            print("Connection already established")
            return 0
        ser = serial.Serial(port, BAUD_RATE, timeout=TIMEOUT)
        # Wait for the connection to initialize
        time.sleep(2)
        print("Connection initialized on port :"+ port)
        return 0

    except serial.SerialException as e:
        return (-1, str(e))     

def get_available_ports():
    ports = serial.tools.list_ports.comports()
    return [port.description for port in ports]

def close_connection():
    global ser
    if ser == None:
        return (-1, "No connection is established")
    if ser.is_open:
        try:
            ser.close()
            ser = None
            return (0,"Connection closed")
        except serial.SerialException as e:
            return (-1, str(e))
    return (-1, "No connection is established")


def send_command(command):
    global ser
    if ser == None:
        return (-1, "No connection is established")
    
    while len(command) <BUFF_SIZE-1:
       
        command += '\0'  # Add spaces to fill up to 32 characters

    encoded_string = (command + '\0').encode('utf-8')
    try:
        ser.write(encoded_string)
        #print("Sent "+ command)
        time.sleep(0.05) # Necessary time for the stm to gather all data before sending another command
        return (0,"")
    except serial.SerialException as e:
        return (-1, str(e))
   
def receive_data():
    global ser
    if ser == None:
        return None

    if ser.in_waiting >= BUFF_SIZE:  # Vérifier si assez de données sont disponibles
        data = ser.read(BUFF_SIZE)  # Lire exactement BUFF_SIZE octets

        try:
            # Décoder et nettoyer les données reçues
            return data.decode('utf-8').strip("\x00")
        except UnicodeDecodeError:
            print("Received non-UTF-8 data:", data)
            return None  # Ou retourner `data.hex()` pour le débogage

    return None


def generate_trame() : 
    trame = "$"
    L_buttons = ["up", "down", "z", "s", "q", "d", "left", "right"]         #left and right correspond to tleft and tright
    for key in L_buttons : 
        if dico_key_pressed[key] == True : 
            trame += "1"
        else : 
            trame +="0"
        trame += ""
    return(trame)

def generate_pid_trame(key, value) : 
    Lkey = key.split("_")
    assert(value<1000.0)
    rValue = f"{value:06.2f}"  # 6 caractères, 2 décimales
    trame = "*" + Lkey[0][0] + Lkey[1] + str(rValue)
    return(trame)


