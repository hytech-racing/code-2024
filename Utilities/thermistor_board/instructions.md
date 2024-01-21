## Directions for logging data with the thermistor board

* Plug in SD card to Teensy
* Connect arduino to laptop
* Set the desired file name in `thermistor_board.ino` (line 31)
  * The file will be saved as "thermistorData_<FILE_NAME>.csv". For instance, if you set file name to "Trial01", then the file will be named "thermistorData_Trial01.csv"
* Flash the code to Ardunio (Teensy 4.1)
* Check the SerialMonitor. The header should get printed, which will look like "Time(sec),M0-T0,M0-T1,...,M11-T7", and data should start getting logged every 2 seconds.
* The data that is printed to the SerialMonitor will also get printed to the SD.

## Weird quirks
* The timestamp (left column) is in seconds since code restart. So, if the timestamp is 153, then it has been 2min and 33 sec since the code last restarted.
* If you do not set a new file name (or remove the last file from the SD), then the code will **append** a new header and the new data to the existing file.









### Run python script (OBSOLETE. DO NOT FOLLOW.)
* Create a `logs` directory in the same directory as the `logtocsv.py` script
* Ensure that the ARDUINO_PORT constant is properly set in `logtocsv.py`
* Run `logtocsv.py`. The program will print the data it receives, as it receives it, but the data is only written to the timestamped file once every BATCH_SIZE batches of data.