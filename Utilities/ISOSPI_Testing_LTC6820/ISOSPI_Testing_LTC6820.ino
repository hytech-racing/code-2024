#include <SPI.h>
#include <Mcp320x.h>

#define SPI_CS    	10 		   // SPI slave select
#define ADC_VREF    3300     // 3.3V Vref
#define ADC_CLK     500000  // SPI clock 1.6MHz

MCP3204 adc(ADC_VREF, SPI_CS);

void setup() {
  // configure PIN mode
  pinMode(SPI_CS, OUTPUT);

  // set initial PIN state
  digitalWrite(SPI_CS, HIGH);

  // initialize serial
  Serial.begin(115200);

  // initialize SPI interface for MCP3208
  // SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);
  SPI.begin();
  // SPI.beginTransaction(settings);
  
}

void loop() {
  // uint32_t t1;
  // uint32_t t2;

  // // start sampling
  // Serial.println("Reading...");

  // t1 = micros();
  // uint16_t raw = adc.read(MCP3204::Channel::SINGLE_0);
  // t2 = micros();

  // // get analog value
  // uint16_t val = adc.toAnalog(raw);

  // // readed value
  // Serial.print("value: ");
  // Serial.print(raw);
  // Serial.print(" (");
  // Serial.print(val);
  // Serial.println(" mV)");

  // // sampling time
  // Serial.print("Sampling time: ");
  // Serial.print(static_cast<double>(t2 - t1) / 1000, 4);
  // Serial.println("ms");

  // delay(2000);

  SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);

  SPI.beginTransaction(settings);

  // activate ADC with chip select
  digitalWrite(SPI_CS, LOW);

  delayMicroseconds(1);

  digitalWrite(SPI_CS, HIGH);

  delayMicroseconds(16);

  digitalWrite(SPI_CS, LOW);

  // // send first command byte
  // mSpi->transfer(cmd.hiByte);
  // // send second command byte and receive first(msb) 4 bits
  // adc.hiByte = mSpi->transfer(cmd.loByte) & 0x0F;
  // // receive last(lsb) 8 bits
  // adc.loByte = mSpi->transfer(0x00);

  delayMicroseconds(1);

  // deactivate ADC with slave select
  digitalWrite(SPI_CS, HIGH);

  delayMicroseconds(1);

  SPI.endTransaction();

}
