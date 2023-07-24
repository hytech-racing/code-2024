/**
 * Basic ADC reading example.
 * - connects to ADC
 * - reads value from channel
 * - converts value to analog voltage
 */

#include <SPI.h>
#include <Mcp320x.h>

#define SPI_CS    	33 		   // SPI slave select
#define ADC_VREF    5000     // 5.0V Vref
#define ADC_CLK     1600000  // SPI clock 1.6MHz


MCP3208 adc(ADC_VREF, SPI_CS);

void setup() {

  // configure PIN mode
  pinMode(SPI_CS, OUTPUT);

  // set initial PIN state
  digitalWrite(SPI_CS, HIGH);

  // initialize serial
  Serial.begin(115200);

  // initialize SPI interface for MCP3208
  SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);
  SPI.begin();
  SPI.beginTransaction(settings);
}

void loop() {

  uint32_t t1;
  uint32_t t2;

  // start sampling
  Serial.println("Reading...");

  t1 = micros();
  uint16_t raw1 = adc.read(MCP3208::Channel::SINGLE_5);
  uint16_t raw2 = adc.read(MCP3208::Channel::SINGLE_4);
  uint16_t raw3 = adc.read(MCP3208::Channel::SINGLE_7);
  uint16_t raw4 = adc.read(MCP3208::Channel::SINGLE_6);
  t2 = micros();

  // get analog value
  uint16_t val1 = adc.toAnalog(raw1);
  uint16_t val2 = adc.toAnalog(raw2);
  uint16_t val3 = adc.toAnalog(raw3);
  uint16_t val4 = adc.toAnalog(raw4);

  // readed value
  Serial.print("value1: ");
  Serial.print(raw1);
  Serial.print(" (");
  Serial.print(val1);
  Serial.println(" mV)");
  Serial.print("value2: ");
  Serial.print(raw2);
  Serial.print(" (");
  Serial.print(val2);
  Serial.println(" mV)");
  Serial.print("value3: ");
  Serial.print(raw3);
  Serial.print(" (");
  Serial.print(val3);
  Serial.println(" mV)");
  Serial.print("value4: ");
  Serial.print(raw4);
  Serial.print(" (");
  Serial.print(val4);
  Serial.println(" mV)");

  // sampling time
  Serial.print("Sampling time: ");
  Serial.print(static_cast<double>(t2 - t1) / 1000, 4);
  Serial.println("ms");

  delay(1000);
}
