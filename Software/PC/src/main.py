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
            "right": ctk.CTkButton(self, text="→", font=fontbut, width=w_button, height=w_button)
        }

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
        self.toggle_button.place(relx = 0.15, rely = 0.2, anchor = "center")
        # État de la connexion
        self.is_connected = False

        # Label de statut
        self.status_label = ctk.CTkLabel(self, text="Connection Lost", font=("Consolas", 20), fg_color="red", width=260, height=60, corner_radius=5)
        self.status_label.place(relx = 0.15, rely = 0.3, anchor = "center")



        # Start updating buttons
        self.update_buttons()
        

    def load_and_display_image(self, image_path, lenght, height):
        """Charge et affiche une image dans la fenêtre."""
        image_pil = Image.open(image_path)

        # Convertir l'image pour CustomTkinter
        image_ctk = ctk.CTkImage(light_image=image_pil, dark_image=image_pil, size=(lenght, height))

        # Affichage dans un label
        self.image_label = ctk.CTkLabel(self, image=image_ctk, text="")  
        return(self.image_label)
    
    #UPDATE
    def update_buttons(self):
        key_has_changed = False
        """Updates button colors based on pressed keys."""
        for key, btn in self.buttons.items():
            btn.configure(fg_color="green" if keyboard.is_pressed(key) else "gray")
            if (dico_key_pressed[key] != keyboard.is_pressed(key)) :                  #We test is there is a change in the dictionnary of key states
                dico_key_pressed[key] = keyboard.is_pressed(key)
                key_has_changed = True                        

        if (key_has_changed == True) :         #if there is a change, we send the trame before the next update, so there is no conflicts when two buttons are pushed at the same time
            print(generate_trame())
            send_command(generate_trame())             
            
        # Schedule this function every 50ms
        self.after(50, self.update_buttons)

    def toggle_connection(self):
        """Active ou désactive la connexion et met à jour l'affichage"""
        self.is_connected = not self.is_connected

        if self.is_connected:
            initialize_connection(COM_PORT)
            self.status_label.configure(text="Connection Established", fg_color="green")
        else:
            close_connection()
            self.status_label.configure(text="Connection Interrupted", fg_color="red")

# Run the application
if __name__ == "__main__":
    app = DroneControlPanel()
    app.mainloop()


