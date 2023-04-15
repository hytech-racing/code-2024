
#define RTCM_BUF_SZ 1024

typedef struct GPS_Data {
    int32_t latitude;
    int8_t latitudeHp;
    int32_t longitude;
    int8_t longitudeHp;
    int32_t ellipsoid;
    int8_t ellipsoidHp;
    int32_t msl;
    int8_t mslHp;
    uint32_t accuracy;
} GPS_Data;

static GPS_Data gps_data;

size_t rtcm_tail = 0;
uint8_t rtcm_buffer[RTCM_BUF_SZ];

typedef struct rtcm_data {
  uint8_t len;
  uint8_t data[128];
} rtcm_data;

rtcm_data rtcm_message;

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to u-blox module.

/*
 * Sets up the GPS
 */
void GPSSetup() {

  Wire.begin();

  //myGNSS.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  if (myGNSS.begin() == false) //Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.setNavigationFrequency(5, VAL_LAYER_RAM); //Set output to 5 times a second. Change the RAM layer only - not BBR

  byte rate;
  if(myGNSS.getNavigationFrequency(&rate)) //Get the update rate of this module
  {
    Serial.print("Current update rate: ");
    Serial.println(rate);
  }
  else
  {
    Serial.println("getNavigationFrequency failed!");
  }

  myGNSS.setI2CInput(COM_TYPE_UBX | COM_TYPE_NMEA | COM_TYPE_RTCM3); //Be sure RTCM3 input is enabled. UBX + RTCM3 is not a valid state.
}

/*
 * Loop to retreive data from the GPS and send correction data to the GPS
 */
void GPSLoop() {
  if(highPrecisionGPS()) {
    uint8_t encoded_gps_data[sizeof(GPS_Data) + 2*sizeof(uint8_t) + sizeof(uint16_t)];
    encode_ser_data(encoded_gps_data, (uint8_t*) &gps_data, sizeof(GPS_Data));
    TCU.write(encoded_gps_data, sizeof(GPS_Data) + 2*sizeof(uint8_t) + sizeof(uint16_t));
    counters.positions++;
    digitalWrite(GPS_OK, !digitalRead(GPS_OK));
  }
  //int tail = rtcm_tail;
  //myGNSS.pushRawData(rtcm_buffer, rtcm_tail);
  //rtcm -= tail
}

/*
 * Puts encoded data into a len + 4 long array
 * @param *cobs_buf     output array of len+4                [ fletcher (1) | data (len) | checksum (2) | delimiter (1) ]
 * @param *input        array that is the input data         [ data (len) ]
 * @param  len          amount of data to encode in bytes             
 */
void encode_ser_data(uint8_t *cobs_buf, const uint8_t *input, size_t len) {
    // Calculate checksum
    uint16_t checksum = fletcher16(input, len);
    //Create and populate an intermediate buffer with the checksum
    uint8_t checksum_buffer[len+sizeof(uint16_t)];
    memcpy(checksum_buffer, input, len);
    memcpy(checksum_buffer+len, (uint8_t*)&checksum, sizeof(uint16_t));
    //Encode buffer
    cobs_encode(checksum_buffer, len+sizeof(uint16_t), cobs_buf);
    cobs_buf[len+sizeof(uint16_t)+sizeof(uint8_t)] = 0x0;
}

void lowPrecisionGPS() {
  //Query module only every second. Doing it more often will just cause I2C traffic.
  //The module only responds when a new position is available
  if (millis() - lastTime > 1000)
  {
    lastTime = millis(); //Update the timer
    
    long latitude = myGNSS.getLatitude();
    Serial.print(F("Lat: "));
    Serial.print(latitude);

    long longitude = myGNSS.getLongitude();
    Serial.print(F(" Long: "));
    Serial.print(longitude);
    Serial.print(F(" (degrees * 10^-7)"));

    long altitude = myGNSS.getAltitude();
    Serial.print(F(" Alt: "));
    Serial.print(altitude);
    Serial.print(F(" (mm)"));

    byte SIV = myGNSS.getSIV();
    Serial.print(F(" SIV: "));
    Serial.print(SIV);

    Serial.println();

    long lat_long[2] = {latitude, longitude};

    uint8_t cobs_buf[2 + 2 * sizeof(long)];
    cobs_encode((uint8_t *)(lat_long), 2 * sizeof(long), cobs_buf);

    cobs_buf[2 * sizeof(long)+1] = 0x0;
    for (int i = 0; i < 2 * sizeof(long)+2; i++) {
        Serial.print(cobs_buf[i], HEX);
        Serial.print(" ");
    }
    Serial.println();

    //Choose TCU or Serial2 as required
    TCU.write(cobs_buf, (size_t) (2 * sizeof(long))+2);
  }
}

int highPrecisionGPS() {
  //Query module. The module only responds when a new position is available
  if (myGNSS.getHPPOSLLH())
  {
    // getHighResLatitude: returns the latitude from HPPOSLLH as an int32_t in degrees * 10^-7
    // getHighResLatitudeHp: returns the high resolution component of latitude from HPPOSLLH as an int8_t in degrees * 10^-9
    // getHighResLongitude: returns the longitude from HPPOSLLH as an int32_t in degrees * 10^-7
    // getHighResLongitudeHp: returns the high resolution component of longitude from HPPOSLLH as an int8_t in degrees * 10^-9
    // getElipsoid: returns the height above ellipsoid as an int32_t in mm
    // getElipsoidHp: returns the high resolution component of the height above ellipsoid as an int8_t in mm * 10^-1
    // getMeanSeaLevel: returns the height above mean sea level as an int32_t in mm
    // getMeanSeaLevelHp: returns the high resolution component of the height above mean sea level as an int8_t in mm * 10^-1
    // getHorizontalAccuracy: returns the horizontal accuracy estimate from HPPOSLLH as an uint32_t in mm * 10^-1

    // If you want to use the high precision latitude and longitude with the full 9 decimal places
    // you will need to use a 64-bit double - which is not supported on all platforms

    // To allow this example to run on standard platforms, we cheat by converting lat and lon to integer and fractional degrees

    // The high resolution altitudes can be converted into standard 32-bit float

    // First, let's collect the position data
    gps_data.latitude = myGNSS.getHighResLatitude();
    gps_data.latitudeHp = myGNSS.getHighResLatitudeHp();
    gps_data.longitude = myGNSS.getHighResLongitude();
    gps_data.longitudeHp = myGNSS.getHighResLongitudeHp();
    gps_data.ellipsoid = myGNSS.getElipsoid();
    gps_data.ellipsoidHp = myGNSS.getElipsoidHp();
    gps_data.msl = myGNSS.getMeanSeaLevel();
    gps_data.mslHp = myGNSS.getMeanSeaLevelHp();
    gps_data.accuracy = myGNSS.getHorizontalAccuracy();
    return 1;
  }
  return 0;
}

void printHighPrecisionGPS() {
  
    // Defines storage for the lat and lon units integer and fractional parts
    int32_t lat_int; // Integer part of the latitude in degrees
    int32_t lat_frac; // Fractional part of the latitude
    int32_t lon_int; // Integer part of the longitude in degrees
    int32_t lon_frac; // Fractional part of the longitude

    // Calculate the latitude and longitude integer and fractional parts
    lat_int = gps_data.latitude / 10000000; // Convert latitude from degrees * 10^-7 to Degrees
    lat_frac = gps_data.latitude - (lat_int * 10000000); // Calculate the fractional part of the latitude
    lat_frac = (lat_frac * 100) + gps_data.latitudeHp; // Now add the high resolution component
    if (lat_frac < 0) // If the fractional part is negative, remove the minus sign
    {
      lat_frac = 0 - lat_frac;
    }
    lon_int = gps_data.longitude / 10000000; // Convert latitude from degrees * 10^-7 to Degrees
    lon_frac = gps_data.longitude - (lon_int * 10000000); // Calculate the fractional part of the longitude
    lon_frac = (lon_frac * 100) + gps_data.longitudeHp; // Now add the high resolution component
    if (lon_frac < 0) // If the fractional part is negative, remove the minus sign
    {
      lon_frac = 0 - lon_frac;
    }

    // Print the lat and lon
    Serial.print("Lat (deg): ");
    Serial.print(lat_int); // Print the integer part of the latitude
    Serial.print(".");
    printFractional(lat_frac, 9); // Print the fractional part of the latitude with leading zeros
    Serial.print(", Lon (deg): ");
    Serial.print(lon_int); // Print the integer part of the latitude
    Serial.print(".");
    printFractional(lon_frac, 9); // Print the fractional part of the latitude with leading zeros
    Serial.println();

    // Now define float storage for the heights and accuracy
    float f_ellipsoid;
    float f_msl;
    float f_accuracy;

    // Calculate the height above ellipsoid in mm * 10^-1
    f_ellipsoid = (gps_data.ellipsoid * 10) + gps_data.ellipsoidHp;
    // Now convert to m
    f_ellipsoid = f_ellipsoid / 10000.0; // Convert from mm * 10^-1 to m

    // Calculate the height above mean sea level in mm * 10^-1
    f_msl = (gps_data.msl * 10) + gps_data.mslHp;
    // Now convert to m
    f_msl = f_msl / 10000.0; // Convert from mm * 10^-1 to m

    // Convert the horizontal accuracy (mm * 10^-1) to a float
    f_accuracy = gps_data.accuracy;
    // Now convert to m
    f_accuracy = f_accuracy / 10000.0; // Convert from mm * 10^-1 to m

    // Finally, do the printing
    Serial.print("Ellipsoid (m): ");
    Serial.print(f_ellipsoid, 4); // Print the ellipsoid with 4 decimal places

    Serial.print(", Mean Sea Level(m): ");
    Serial.print(f_msl, 4); // Print the mean sea level with 4 decimal places

    Serial.print(", Accuracy (m): ");
    Serial.println(f_accuracy, 4); // Print the accuracy with 4 decimal places
}

// Pretty-print the fractional part with leading zeros - without using printf
// (Only works with positive numbers)
void printFractional(int32_t fractional, uint8_t places)
{
  if (places > 1)
  {
    for (uint8_t place = places - 1; place > 0; place--)
    {
      if (fractional < pow(10, place))
      {
        Serial.print("0");
      }
    }
  }
  Serial.print(fractional);
}

void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {

  counters.recieved++;
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  digitalWrite(GPS_RTK_OK, !digitalRead(GPS_RTK_OK));
  memcpy(&rtcm_message, incomingData, len);
  myGNSS.pushRawData(rtcm_message.data, rtcm_message.len);
  //memcpy((rtcm_buffer + rtcm_tail), rtcm_message.data, rtcm_message.len);
  //rtcm_tail += rtcm_message.len;
}