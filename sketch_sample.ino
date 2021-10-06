#include "Wire.h"
#include <MS5837.h>
extern "C" {
#include "utility/twi.h" // from Wire library, so we can do bus scanning
}
#define TCAADDR 0x70
MS5837 sensor;

void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(0);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void setup() {
  while (!Serial);
  //delay(1000);
  Wire.begin();
  // Serial.begin(115200);
  Serial.begin(9600);
  Serial.println("TCAScanner ready!");

  for (uint8_t t=0; t<8; t++) {
    tcaselect(t);
    Serial.print("TCA Port #"); Serial.println(t);
    for (uint8_t addr=0; addr<127; addr++) {
      if (addr == TCAADDR) continue;
      Wire.beginTransmission(addr);
      if (!Wire.endTransmission()) {
        Serial.print("Found I2C 0x"); Serial.println(addr,HEX);
      Wire.begin();
      }
    }
  }
  Serial.println("\nSetup done");
  Serial.println("\nSTATIC    S_2       S_3       S_4       DYN       S_6       S_7       S_8");
}


void loop() {
  while (!Serial);
  //delay(10);
  Wire.begin();
  for (uint8_t t=0; t<8; t++) { // Initialising sensor data
    tcaselect(t);
    for (uint8_t addr = 0; addr<=127; addr++) {
      if (addr == TCAADDR) continue;
      uint8_t data;
      if (! twi_writeTo(addr, &data, 0, 1, 1)) {
        Wire.begin();
        while (!sensor.init()) {
        }
        sensor.setModel(MS5837::MS5837_30BA);
        sensor.setFluidDensity(997); // kg/m^3 (freshwater, 1029 for seawater)
        
        //Timing section of code
        //unsigned long timeBegin = millis();
        // INSERT CODE HERE
        //unsigned long timeEnd = millis();
        //unsigned long duration = timeEnd - timeBegin;
        //Serial.print(duration);
      }
    }
  }
  for (uint8_t t=0; t<8; t++) { // Starting/Writing D1 data from sensors
    tcaselect(t);
    for (uint8_t addr = 0; addr<=127; addr++) {
      if (addr == TCAADDR) continue;
      uint8_t data;
      if (! twi_writeTo(addr, &data, 0, 1, 1)) {
        Wire.begin();
        sensor.start_conversionD1();
      }
    }
  }
  delay(16);
  for (uint8_t t=0; t<8; t++) { // Reading D1 data + Starting/Writing D2 data from sensors
    tcaselect(t);
    for (uint8_t addr = 0; addr<=127; addr++) {
      if (addr == TCAADDR) continue;
      uint8_t data;
      if (! twi_writeTo(addr, &data, 0, 1, 1)) {
        Wire.begin();
        sensor.read_conversionD1();
        sensor.start_conversionD2();
      }
    }
  }
  delay(16);
  for (uint8_t t=0; t<8; t++) { // Reading D2 data
    tcaselect(t);
    for (uint8_t addr = 0; addr<=127; addr++) {
      if (addr == TCAADDR) continue;
      uint8_t data;
      if (! twi_writeTo(addr, &data, 0, 1, 1)) {
        Wire.begin();
        sensor.read_conversionD2();
      }
    }
  }
  for (uint8_t t=0; t<8; t++) { // Calculating pressures
    tcaselect(t);
    for (uint8_t addr = 0; addr<=127; addr++) {
      if (addr == TCAADDR) continue;
      uint8_t data;
      if (! twi_writeTo(addr, &data, 0, 1, 1)) {
        Wire.begin();
        sensor.calculate();
        Serial.print(sensor.pressure());
        Serial.print(" , ");
      }
    }
  }
  Serial.println("");
  //if (sensor.pressure() == 1040) {
    //exit(0);
  }
