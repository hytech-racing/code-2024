// SD Random I/O tests.
#include "SdFat.h"
const size_t MAX_FILES = 16;
const size_t MAX_SECTORS = 64;
// Space files by 1 GiB
const uint32_t FILE_ALLOC = 1UL << 29;
// File size 1 MiB
const uint32_t FILE_SIZE = 1UL << 20;

SdFs sd;
FsFile file[MAX_FILES];
uint8_t buf[512*MAX_SECTORS];

// Try to select the best SD card configuration.
#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#else  // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SS,DEDICATED_SPI, SD_SCK_MHZ(50))
#endif  // HAS_SDIO_CLASS
#define error(msg) {Serial.println(msg);while (true);}

char name[] = "?.bin";

void closeFiles() {
  for (uint8_t i = 0; i < MAX_FILES; i++) {
    file[i].close();
  }
}  

void createFiles() {
  uint32_t m;
  Serial.println("Create files");
  m = micros();
  for (uint8_t i = 0; i < MAX_FILES; i++) {
    // Spread files so files ping-pong.
    name[0] = 'A' + ( i < MAX_FILES/2 ? i*2 : 2*i - MAX_FILES + 1 );
    if (!file[i].open(name, O_RDWR | O_CREAT | O_TRUNC)) error("open create");
    if (!file[i].preAllocate(FILE_ALLOC)) error("preAllocate");
  }
  m = micros() - m;
  Serial.print("Create micros: ");
  Serial.println(m);
  memset(buf, 0XAA, sizeof(buf));
  uint32_t nw = FILE_SIZE/sizeof(buf);  
  m = micros();
  for (uint8_t i = 0; i < MAX_FILES; i++) {
    for (uint32_t iw = 0; iw < nw; iw++) {
      if (sizeof(buf) != file[i].write(buf, sizeof(buf))) error("create write");
    }
  }
  m = micros() - m;
  Serial.print("Sequential Write: ");
  Serial.print(MAX_FILES*FILE_SIZE/(0.001*m));
  Serial.println(" KB/sec"); 
  closeFiles();
}
void openFiles(uint8_t opt) {
  for (uint8_t i = 0; i < MAX_FILES; i++) {
    name[0] = i + 'A';
    if (!file[i].open(name, opt)) error("open");
  }  
}
void randomRead() {
  Serial.print("Begin Random Read Test\nns - read size sectors,");
  Serial.println(" nf - number of files, maxLat - latency micros");
  uint32_t m;
  uint32_t maxLat;
  uint32_t lat;
  for (uint8_t ns = MAX_SECTORS; ns > 0; ns /= 2) {
    for (uint8_t nf = 1; nf <= MAX_FILES; nf *= 2) {
      openFiles(O_RDONLY);
      maxLat = 0;      
      uint32_t nr = FILE_SIZE/(512*ns);
      m = micros();
      for (uint32_t ir = 0; ir < nr; ir++) {
        for (uint32_t i = 0; i < nf; i++) {
          lat = micros();
          if (ns*512 != file[i].read(buf, ns*512)) error("randomRead");
          lat = micros() - lat;
          if (lat > maxLat) {
            maxLat = lat;
          }
        }
      }
      m = micros() - m;
      Serial.print("ns: ");
      Serial.print(ns);     
      Serial.print(" nf: ");
      Serial.print(nf);
      Serial.print(" maxLat: ");
      Serial.print(maxLat);
      Serial.print(" total KB/sec: ");
      Serial.print(nf*FILE_SIZE/(0.001*m));
      Serial.print(" file KB/sec: ");
      Serial.println(FILE_SIZE/(0.001*m));
      closeFiles();      
    }
  }
  Serial.println("End Random Read Test");
}

void randomWrite() {
  Serial.print("Begin Random Write Test\nns - read size sectors,");
  Serial.println(" nf - number of files, maxLat - latency micros");
  uint32_t m;
  uint32_t maxLat;
  uint32_t lat;
  for (uint8_t ns = MAX_SECTORS; ns > 0; ns /= 2) {
    for (uint8_t nf = 1; nf <= MAX_FILES; nf *= 2) {
      openFiles(O_RDWR);
      maxLat = 0;          
      uint32_t nr = FILE_SIZE/(512*ns);
      m = micros();
      for (uint32_t ir = 0; ir < nr; ir++) {
        for (uint32_t i = 0; i < nf; i++) {
          lat = micros();          
          if (ns*512 != file[i].write(buf, ns*512)) error("randomWrite");
          lat = micros() - lat;          
          if (lat > maxLat) {
            maxLat = lat;
          }          
        }
      }
      m = micros() - m;
      Serial.print("ns: ");
      Serial.print(ns);     
      Serial.print(" nf: ");
      Serial.print(nf);
      Serial.print(" maxLat: ");
      Serial.print(maxLat);      
      Serial.print(" total KB/sec: ");
      Serial.print(nf*FILE_SIZE/(0.001*m));
      Serial.print(" file KB/sec: ");
      Serial.println(FILE_SIZE/(0.001*m));
      closeFiles();      
    }
  }
  Serial.println("End Random Write Test");
}

void removeFiles() {
  uint32_t m = micros();
  for (uint8_t i = 0; i < MAX_FILES; i++) {
    name[0] = i + 'A';
    if (!sd.remove(name)) error("removeFiles");;
  }
  m = micros() - m;
  Serial.print("Remove micros: ");
  Serial.println(m);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  Serial.println("Type any character to start");
  while (!Serial.available()) {}
  do {
    delay(10);
  } while (Serial.read() >= 0);
  if (!sd.begin(SD_CONFIG)) error("sd.begin()");
  createFiles();
  randomRead();
  randomWrite();
  Serial.println("Type any character to remove files");
  while (!Serial.available()) {}
  removeFiles();
  Serial.println("Done");
}

void loop() {
}