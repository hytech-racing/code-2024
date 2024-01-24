#Raspberry Pi Shutdown
#TODO: make sure no data is still being transmitted before shutting down
#TODO: Determine BCM vs BOARD mode for numbering
#TODO: Decide on input pin
import RPi.GPIO as GPIO
from subprocess import call

GPIO.setmode(GPIO.BCM)           # Set's GPIO pins to BCM GPIO numbering
INPUT_PIN = 4                    # Sets our input pin 
GPIO.setup(INPUT_PIN, GPIO.IN)   # Set our input pin to be an input


while True: 
    if (GPIO.input(INPUT_PIN) == True): # Physically read the pin 
        call("sudo shutdown -h now", shell=True)

