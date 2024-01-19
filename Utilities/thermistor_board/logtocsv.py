import serial
import math
import datetime;
import csv



##### GLOBAL CONSTANTS #####

ARDUINO_PORT = "/dev/cu.usbmodem122442101"

# Name of the "latest" log file. All data is saved two places-- the timestamped file, and to LATEST_LOG
LATEST_LOG = "data/latest.csv"

# File name prefix. Data will be saved to "fileNamePrefix" + timestamp
fileNamePrefix = "thermistor-data"

# Number of readings that get buffered between file prints
BATCH_SIZE = 4

number_of_multiplexers = 12
thermistors_per_multiplexer = 8

# Program will never self-terminate if this is True
ENDLESS_MODE = False

# How many batches of data should the program read before it quits? (This only applies when endlessMode == False)
TOTAL_LINES_OF_DATA = 100

# Unknown resistance data
#RESISTANCE_DATA = []
#for i in range(96):
#    f.append(0)



# Sets up serial port
baud = 9600
BETA = 3500
FILE_NAME = "data/" + fileNamePrefix + "_" + str(datetime.datetime.now()).replace(" ", "_") + ".csv"
serial = serial.Serial(ARDUINO_PORT, baud)
print("Connected to Arudino port: " + ARDUINO_PORT)
file = open(FILE_NAME, "w")
print("Created file")
serial.readline() #flushes current data (incomplete lines)



def convertDigitalToTemp(analogRead, index):
    #if analogRead >= 1023:
     #   return -1

    ##### INSERT DIGITAL TO CELSIUS HERE
    #resistance = 10000
    #kelvin = 1 / ((1/298.15) + (1/BETA) * math.log(resistance / 10000)  ) # 10kOhm at 25 celsius
    #celsius = kelvin - 273.15
    #return celsius

    #totalResistance = (analogRead * 3300) / (1023 - analogRead)

    #thermistorResistance = totalResistance - RESISTANCE_DATA[index]
    
    #kelvin = (298.15 * 3492) / (3492 + 298.15 * math.log(thermistorResistance / 10000))
    #celsius = kelvin - 273.15

    #return celsius
    #return totalResistance
    return analogRead

def convertDigitalLineToTemp(line):
    dataElements = line.split(",")
    outputLine = dataElements[0] + ","
    for i in range(1, len(dataElements) - 1):
        outputLine += str(convertDigitalToTemp(int(dataElements[i]), i - 1)) + ","
    return outputLine



# Write the file header
def openFile(fileName):
    with open(fileName, 'w', encoding='UTF8', newline='') as f:
        f.write("Timestamp (sec),")
        for i in range(number_of_multiplexers):
            for j in range(thermistors_per_multiplexer):
                f.write("M" + str(i) + "-T" + str(j) + ',')
        f.write('\n')
    file.close()

openFile(FILE_NAME)
openFile(LATEST_LOG)

def write_to_file(sensor_data, fileName):
    with open(fileName, 'a', encoding='UTF8', newline='') as f:
        # Writing the file header
        for line in sensor_data:
            #f.write(line)
            f.write(convertDigitalLineToTemp(line))
            f.write('\n')
        
    file.close()



# Write the actual data periodically
line = 0
thermistor_data = []
while ENDLESS_MODE or line < TOTAL_LINES_OF_DATA:
    getData = serial.readline()
    dataString = getData.decode('utf-8')
    print(dataString)
    thermistor_data.append(dataString.strip())

    line = line + 1
    
    if line % BATCH_SIZE == 0:
        write_to_file(thermistor_data, FILE_NAME)
        write_to_file(thermistor_data, LATEST_LOG)
        thermistor_data = []
    


print("Data logging complete! Logged " + str(line) + " lines. Saved to " + FILE_NAME)
print("If you want the data to be logged indefinitely, then enable ENDLESS_MODE to true")
