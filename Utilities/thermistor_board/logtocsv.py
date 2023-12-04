import serial
import csv

# GLOBAL CONSTANTS
fileNamePrefix = "thermistor-data"
batch_size = 10 # Number of readings that get buffered between file prints
number_of_multiplexers = 12
thermistors_per_multiplexer = 8
endless_mode = False # Program will never self-terminate if this is True
total_lines_of_data = 100 # How many batches of data should the program read before it quits? (This only applies when endlessMode == False)

# Sets up serial port
# arduino_port = "/dev/cu.usbmodem104812901" #serial port of arduino
arduino_port = "/dev/cu.usbmodem122442101"
baud = 9600
fileName = "thermistor-data.csv"
serial = serial.Serial(arduino_port, baud)
print("Connected to Arudino port: " + arduino_port)
file = open(fileName, "w")
print("Created file")
serial.readline() #flushes current data (incomplete lines)



# Write the file header
with open(fileName, 'w', encoding='UTF8', newline='') as f:
    f.write("Timestamp (sec),")
    for i in range(number_of_multiplexers):
        for j in range(thermistors_per_multiplexer):
            f.write("M" + str(i) + "-T" + str(j) + ',')
    f.write('\n')
file.close()


def write_to_file(sensor_data):
    with open(fileName, 'a', encoding='UTF8', newline='') as f:
        # Writing the file header
        for line in sensor_data:
            f.write(line)
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
        write_to_file(thermistor_data)
        thermistor_data = []
    

print("Data logging complete! Logged " + str(line) + " lines. Saved to " + fileName)
