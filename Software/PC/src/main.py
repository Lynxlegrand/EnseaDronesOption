import customtkinter as ctk
import keyboard  
from com import *
from const import * 
from PIL import Image  

# Initialize the application
ctk.set_appearance_mode("dark") 
ctk.set_default_color_theme("blue") 


class DroneControlPanel(ctk.CTk):
    def __init__(self):
        super().__init__()

        self.keys_pressed = {}

        self.title("Drone Control Panel")
        self.geometry("1080x600")  
        self.after(100, lambda: self.state("zoomed"))

        # Title label
        self.label = ctk.CTkLabel(self, text="Drone Remote Control", font=("Consolas", 50, "bold"))
        self.label.place(relx = 0.5, rely = 0.1, anchor = "center")

        # Load and display images
        logo_ensea = self.load_and_display_image("img\logo_ensea.jpg", 200, 200)
        logo_ensea.place(relx=1, rely=0, anchor="ne")

        drone_schematic = self.load_and_display_image("img\Drone_roll_pitch_yaw-01.png", 550, 400)
        drone_schematic.place(relx=0.5, rely=0.4, anchor="center")

        # Control buttons
        w_button = 75
        fontbut = ("Consolas", 30)
        self.buttons = {
            "z": ctk.CTkButton(self, text="Z", font=fontbut, width=w_button, height=w_button), ##S'en inspirer faire une fonction send keys pour récuperer les valeurs du dico et sned command (à appeler à chaque appui et chaque relèvement)
            "q": ctk.CTkButton(self, text="Q", font=fontbut, width=w_button, height=w_button),
            "s": ctk.CTkButton(self, text="S", font=fontbut, width=w_button, height=w_button),
            "d": ctk.CTkButton(self, text="D", font=fontbut, width=w_button, height=w_button),
            "up": ctk.CTkButton(self, text="↑", font=fontbut, width=w_button, height=w_button),
            "left": ctk.CTkButton(self, text="←", font=fontbut, width=w_button, height=w_button),
            "down": ctk.CTkButton(self, text="↓", font=fontbut, width=w_button, height=w_button),
            "right": ctk.CTkButton(self, text="→", font=fontbut, width=w_button, height=w_button),
        }

        # PID Adjustment Section
        self.pid_entries = {}
        pid_params = ["Height", "Pitch", "Roll", "Yaw"]
        pid_types = ["P", "I", "D"]

        frame = ctk.CTkFrame(self)
        frame.place(relx=0.85, rely=0.45, anchor="center")

        # Ajout des labels "P", "I", "D" en haut
        for j, p_type in enumerate(pid_types):
            ctk.CTkLabel(frame, text=p_type, font=("Consolas", 20, "bold")).grid(row=0, column=j+1, padx=10, pady=5)

        # Ajout des labels pour chaque paramètre PID et des champs de saisie
        for i, param in enumerate(pid_params):
            ctk.CTkLabel(frame, text=param, font=("Consolas", 20, "bold")).grid(row=i+1, column=0, padx=10, pady=5)

            for j, p_type in enumerate(pid_types):
                entry = ctk.CTkEntry(frame, width=100, font=("Consolas", 15), justify = "center")
                entry.insert(0, "Default")
                entry.grid(row=i+1, column=j+1, padx=10, pady=5)
                self.pid_entries[f"{param}_{p_type}"] = entry

        # Send Button PID
        self.send_pid_button = ctk.CTkButton(frame, text="Send", font=("Consolas", 20),command=self.fill_pid_values_to_change)
        self.send_pid_button.grid(row=len(pid_params) + 1, columnspan=4, pady=10)


        # Emergency Stop Button
        self.emergency_button = ctk.CTkButton(self, text="Space Bar", font=fontbut, width=350, height=75, fg_color="red", command=self.switch_emergency_stop)
        self.emergency_button.place(relx=0.5, rely=0.88, anchor="center")

        # Assign spacebar to emergency stop
        keyboard.add_hotkey("space", self.switch_emergency_stop)

        # Emergency Stop Label
        self.emergencylabel = ctk.CTkLabel(self, text = "Emergency stop button", font=("Consolas", 25))
        self.emergencylabel.place(relx=0.5, rely=0.79, anchor="center")

        #initialisation of the dictionnary
        for btn in self.buttons : 
            dico_key_pressed[btn] = False
        

        # Position buttons to match keyboard layout
        center_x = 750
        center_y = 750
        spacing = 20 + w_button
        dec_x = 450

        # ZQSD buttons (left)
        self.buttons["z"].place(x=center_x - dec_x, y= center_y - spacing , anchor = "center")
        self.buttons["q"].place(x=center_x - dec_x - spacing, y=center_y, anchor = "center")
        self.buttons["s"].place(x=center_x - dec_x, y=center_y, anchor = "center")
        self.buttons["d"].place(x=center_x -dec_x + spacing, y=center_y, anchor = "center")
        # Button label
        self.label1 = ctk.CTkLabel(self, text="Translation according to x and y", font=("Consolas", 25))
        self.label1.place(x=center_x - dec_x, y= center_y - spacing -75, anchor = "center")

        # Arrow buttons (right)
        self.buttons["up"].place(x=center_x + dec_x, y= center_y - spacing , anchor = "center")
        self.buttons["left"].place(x=center_x + dec_x - spacing, y=center_y, anchor = "center")
        self.buttons["down"].place(x=center_x + dec_x, y=center_y, anchor = "center")
        self.buttons["right"].place(x=center_x + dec_x + spacing, y=center_y, anchor = "center")

        # Button label
        self.label1 = ctk.CTkLabel(self, text="Translation according to z and yaw", font=("Consolas", 25))
        self.label1.place(x=center_x + dec_x, y= center_y - spacing -75, anchor = "center")

         # Bouton switch (toggle)
        self.toggle_button = ctk.CTkSwitch(self, text="Connection",font=("Consolas", 20), width=100, height=50, switch_width=75, switch_height=35, command=self.toggle_connection)
        self.toggle_button.place(relx = 0.15, rely = 0.15, anchor = "center")
        # État de la connexion
        self.is_connected = False

        # Label de statut
        self.status_label = ctk.CTkLabel(self, text="Connection Lost", font=("Consolas", 20), fg_color="red", width=260, height=60, corner_radius=5)
        self.status_label.place(relx = 0.15, rely = 0.25, anchor = "center") 

        #Start button
        self.start_button = ctk.CTkButton(self, text = "Start engine",font=("Consolas", 20), fg_color="green", width=260, height=60, corner_radius=50, command= self.toggle_start_button)    
        self.start_button.place(relx = 0.15, rely = 0.5, anchor = "center")
        self.send_start = False
        # Start updating buttons
        self.update()
        

    def load_and_display_image(self, image_path, lenght, height):
        """Charge et affiche une image dans la fenêtre."""
        image_pil = Image.open(image_path)

        # Convertir l'image pour CustomTkinter
        image_ctk = ctk.CTkImage(light_image=image_pil, dark_image=image_pil, size=(lenght, height))

        # Affichage dans un label
        self.image_label = ctk.CTkLabel(self, image=image_ctk, text="")  
        return(self.image_label)
    
    #UPDATE
    def update(self):
        data = receive_data()
        if data != None:
            print(data)
        if emergency_stop : 
            send_command("$stop")
        if self.send_start : 
            send_command("$start")
            self.send_start = False
        elif len(pid_values_to_change)!=0:
            n = len(pid_values_to_change)
            key = pid_values_to_change[n-1][0]
            value = pid_values_to_change[n-1][1]
            pid_values_to_change.pop()
            send_command(generate_pid_trame(key, value))
        else : 
            key_has_changed = False
            """Updates button colors based on pressed keys."""
            for key, btn in self.buttons.items():
                btn.configure(fg_color="green" if keyboard.is_pressed(key) else "gray")
                if (dico_key_pressed[key] != keyboard.is_pressed(key)) :                  #We test is there is a change in the dictionnary of key states
                    dico_key_pressed[key] = keyboard.is_pressed(key)
                    if not key_has_changed:
                        key_has_changed = True                        
            if key_has_changed :         #if there is a change, we send the trame before the next update, so there is no conflicts when two buttons are pushed at the same time
                send_command(generate_trame())   
            
        # Schedule this function every UPDATE_PERIOD
        self.after(UPDATE_PERIOD, self.update)

    def toggle_connection(self):
        """Active ou désactive la connexion et met à jour l'affichage"""
        self.is_connected = not self.is_connected

        if self.is_connected:
            initialize_connection()
            self.status_label.configure(text="Connection Established", fg_color="green")
        else:
            close_connection()
            self.status_label.configure(text="Connection Interrupted", fg_color="red")
    
    def switch_emergency_stop(self): 
        global emergency_stop
        emergency_stop = not emergency_stop
        print("Emergency stop activated" if emergency_stop else "Emergency stop diactivated")
        if emergency_stop : 
            self.emergency_button.configure(fg_color="grey", text = "Stop signal scent")
        else : 
            self.emergency_button.configure(fg_color="red", text = "Space bar")
    
    
    def fill_pid_values_to_change(self) : 
        self.focus_set()
        for key, entry in self.pid_entries.items():
            value = entry.get()
            if value != "Default" : 
                try :
                    value = float(value)  
                    pid_values_to_change.append([key,value])
                except(Exception) : 
                    entry.delete(0, "end")  # Effacer le champ
                    entry.insert(0, "Default")  # Remettre "Default"
    
    def toggle_start_button (self) : 
        if self.send_start == False : 
            self.send_start = True
            self.start_button.configure(text = "Start sended")


        

# Run the application
if __name__ == "__main__":
    app = DroneControlPanel()
    app.mainloop()


