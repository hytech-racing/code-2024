#pragma once
#ifndef noPush
#define noPush
#endif

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Callback: pushRXMPMP will be called when new PMP data arrives
// See u-blox_structs.h for the full definition of UBX_RXM_PMP_message_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setRXMPMPmessageCallbackPtr
//        /               _____  This _must_ be UBX_RXM_PMP_message_data_t
//        |              /              _____ You can use any name you like for the struct
//        |              |             /
//        |              |             |
void pushRXMPMP(UBX_RXM_PMP_message_data_t *pmpData)
{
  //Extract the raw message payload length
  uint16_t payloadLen = ((uint16_t)pmpData->lengthMSB << 8) | (uint16_t)pmpData->lengthLSB;
  Serial.print(F("New RXM-PMP data received. Message payload length is "));
  Serial.print(payloadLen);

#ifndef noPush

  Serial.println(F(" Bytes. Pushing it to the GNSS..."));
  
  //Push the PMP data to the GNSS
  //The payload length could be variable, so we need to push the header and payload, then checksum
  myGNSS.pushRawData(&pmpData->sync1, (size_t)payloadLen + 6); // Push the sync chars, class, ID, length and payload
  myGNSS.pushRawData(&pmpData->checksumA, (size_t)2); // Push the checksum bytes

#else
#ifdef printGPS
  Serial.println(F(" Bytes."));
#endif
#endif
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Callback: printPVTdata will be called when new NAV PVT data arrives
// See u-blox_structs.h for the full definition of UBX_NAV_PVT_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setAutoPVTcallbackPtr
//        /                  _____  This _must_ be UBX_NAV_PVT_data_t
//        |                 /               _____ You can use any name you like for the struct
//        |                 |              /
//        |                 |              |
void printPVTdata(UBX_NAV_PVT_data_t *ubxDataStruct)
{

  int32_t latitude = ubxDataStruct->lat; // Print the latitude
  int32_t longitude = ubxDataStruct->lon; // Print the longitude
  int32_t altitude = ubxDataStruct->hMSL; // Print the height above mean sea level
  uint8_t fixType = ubxDataStruct->fixType; // Print the fix type
  uint8_t carrSoln = ubxDataStruct->flags.bits.carrSoln; // Print the carrier solution
  uint32_t hAcc = ubxDataStruct->hAcc; // Print the horizontal accuracy estimate

  gps_lat_long.set_gps_latitude(latitude);
  gps_lat_long.set_gps_longitude(longitude);
  gps_other.set_gps_accuracy(hAcc);
  gps_other.set_gps_msl(altitude);

  CAN_message_t rx_msg2;
  gps_lat_long.write(rx_msg2.buf);
  rx_msg2.len = sizeof(GPS_lat_long);
  rx_msg2.id = ID_GPS_LAT_LONG;
  CAN_message_t rx_msg3;
  gps_other.write(rx_msg3.buf);
  rx_msg3.len = sizeof(GPS_other);
  rx_msg3.id = ID_GPS_OTHER;
  uint64_t time = getTime();
  CAN_msg_q.unshift((CAN_msg_time){ .msg = rx_msg2, .time = time });  //unclear what were passing
  CAN_msg_q.unshift((CAN_msg_time){ .msg = rx_msg3, .time = time });  //unclear what were passing
  counters.GPS_freq++;

#ifdef printGPS
  Serial.print(F("Lat: "));
  Serial.print((double) latitude / 10000000.0, 7);
  Serial.print(F("  Long: "));
  Serial.print((double) longitude / 10000000.0, 7);
  Serial.print(F("  Height: "));
  Serial.print((double) altitude / 1000.0, 3);
  Serial.print(F("  Fix: "));
  Serial.print(fixType);
  if (fixType == 0)
    Serial.print(F(" (None)"));
  else if (fixType == 1)
    Serial.print(F(" (Dead Reckoning)"));
  else if (fixType == 2)
    Serial.print(F(" (2D)"));
  else if (fixType == 3)
    Serial.print(F(" (3D)"));
  else if (fixType == 3)
    Serial.print(F(" (GNSS + Dead Reckoning)"));
  else if (fixType == 5)
    Serial.print(F(" (Time Only)"));
  else
    Serial.print(F(" (UNKNOWN)"));

  Serial.print(F("  Carrier Solution: "));
  Serial.print(carrSoln);
  if (carrSoln == 0)
    Serial.print(F(" (None)"));
  else if (carrSoln == 1)
    Serial.print(F(" (Floating)"));
  else if (carrSoln == 2)
    Serial.print(F(" (Fixed)"));
  else
    Serial.print(F(" (UNKNOWN)"));

  Serial.print(F("  Horizontal Accuracy Estimate: "));
  Serial.print(hAcc);
  Serial.print(F(" (mm)"));

  Serial.println();    
#endif
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Callback: printRXMCOR will be called when new RXM COR data arrives
// See u-blox_structs.h for the full definition of UBX_RXM_COR_data_t
//         _____  You can use any name you like for the callback. Use the same name when you call setRXMCORcallbackPtr
//        /                  _____  This _must_ be UBX_RXM_COR_data_t
//        |                 /               _____ You can use any name you like for the struct
//        |                 |              /
//        |                 |              |
void printRXMCOR(UBX_RXM_COR_data_t *ubxDataStruct)
{
#ifdef printGPS
  Serial.print(F("UBX-RXM-COR:  ebno: "));
  Serial.print(ubxDataStruct->ebno);

  Serial.print(F("  protocol: "));
  if (ubxDataStruct->statusInfo.bits.protocol == 1)
    Serial.print(F("RTCM3"));
  else if (ubxDataStruct->statusInfo.bits.protocol == 2)
    Serial.print(F("SPARTN"));
  else if (ubxDataStruct->statusInfo.bits.protocol == 29)
    Serial.print(F("PMP (SPARTN)"));
  else if (ubxDataStruct->statusInfo.bits.protocol == 30)
    Serial.print(F("QZSSL6"));
  else
    Serial.print(F("Unknown"));

  Serial.print(F("  errStatus: "));
  if (ubxDataStruct->statusInfo.bits.errStatus == 1)
    Serial.print(F("Error-free"));
  else if (ubxDataStruct->statusInfo.bits.errStatus == 2)
    Serial.print(F("Erroneous"));
  else
    Serial.print(F("Unknown"));

  Serial.print(F("  msgUsed: "));
  if (ubxDataStruct->statusInfo.bits.msgUsed == 1)
    Serial.print(F("Not used"));
  else if (ubxDataStruct->statusInfo.bits.msgUsed == 2)
    Serial.print(F("Used"));
  else
    Serial.print(F("Unknown"));

  Serial.print(F("  msgEncrypted: "));
  if (ubxDataStruct->statusInfo.bits.msgEncrypted == 1)
    Serial.print(F("Not encrypted"));
  else if (ubxDataStruct->statusInfo.bits.msgEncrypted == 2)
    Serial.print(F("Encrypted"));
  else
    Serial.print(F("Unknown"));

  Serial.print(F("  msgDecrypted: "));
  if (ubxDataStruct->statusInfo.bits.msgDecrypted == 1)
    Serial.print(F("Not decrypted"));
  else if (ubxDataStruct->statusInfo.bits.msgDecrypted == 2)
    Serial.print(F("Successfully decrypted"));
  else
    Serial.print(F("Unknown"));

  Serial.println();
#endif
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=