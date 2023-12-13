## Directions for logging data with the thermistor board

### Set up arduino
* Connect arduino to laptop
* Flash the code to Ardunio (Teensy 4.1)

### Run python script
* Create a `logs` directory in the same directory as the `logtocsv.py` script
* Ensure that the ARDUINO_PORT constant is properly set in `logtocsv.py`
* Run `logtocsv.py`. The program will print the data it receives, as it receives it, but the data is only written to the timestamped file once every BATCH_SIZE batches of data.