import serial
import math
import datetime;
import csv

# GLOBAL CONSTANTS
fileNamePrefix = "thermistor-data"
batch_size = 4 # Number of readings that get buffered between file prints
number_of_multiplexers = 12
thermistors_per_multiplexer = 8
endless_mode = False # Program will never self-terminate if this is True
total_lines_of_data = 100 # How many batches of data should the program read before it quits? (This only applies when endlessMode == False)

# Sets up serial port
# arduino_port = "/dev/cu.usbmodem104812901" #serial port of arduino
arduino_port = "/dev/cu.usbmodem122442101"
baud = 9600
BETA = 3500
FILE_NAME = "data/" + fileNamePrefix + str(datetime.datetime.now()) + ".csv"
LATEST_LOG = "data/latest.csv"
serial = serial.Serial(arduino_port, baud)
print("Connected to Arudino port: " + arduino_port)
file = open(FILE_NAME, "w")
print("Created file")
serial.readline() #flushes current data (incomplete lines)

def convertDigitalToTemp(digital):
    if digital == 1023:
        return -1

    #resistance = 10000
    #kelvin = 1 / ((1/298.15) + (1/BETA) * math.log(resistance / 10000)  ) # 10kOhm at 25 celsius
    #celsius = kelvin - 273.15
    #return celsius

    return -1

def convertDigitalLineToTemp(line):
    dataElements = line.split(",")
    outputLine = dataElements[0] + ","
#    for i in range(len(dataElements)):
#        print(dataElements[i] + ",")
    for i in range(1, len(dataElements) - 1):
        outputLine += str(convertDigitalToTemp(int(dataElements[i]))) + ","
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
while endless_mode or line < total_lines_of_data:
    getData = serial.readline()
    dataString = getData.decode('utf-8')
    print(dataString)
    thermistor_data.append(dataString.strip())

    line = line + 1
    
    if line % batch_size == 0:
        write_to_file(thermistor_data, FILE_NAME)
        write_to_file(thermistor_data, LATEST_LOG)
        thermistor_data = []
    

print("Data logging complete! Logged " + str(line) + " lines. Saved to " + FILE_NAME)
