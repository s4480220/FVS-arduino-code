#include "Wire.h"
#include <MS5837.h>
extern "C" {
#include "utility/twi.h" // from Wire library, so we can do bus scanning
}
#define TCAADDR 0x70
MS5837 sensor[8];

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
      while (!sensor[t].init()) {
      }
      sensor[t].setModel(MS5837::MS5837_30BA);
      sensor[t].setFluidDensity(997); // kg/m^3 (freshwater, 1029 for seawater)
    }
  }
  Serial.println("\nSetup done");
  Serial.println("\nSTATIC    S_2       S_3       S_4       DYN       S_6       S_7       S_8");
}


void loop() {
  while (!Serial);
  //delay(10);
  Wire.begin();
  for (uint8_t t=0; t<8; t++) {
    tcaselect(t);
    sensor[t].start_conversionD1();
  }
  //delay(16);
  for (uint8_t t=0; t<8; t++) {
    tcaselect(t);
    sensor[t].read_conversionD1();
  }
  for (uint8_t t=0; t<8; t++) {
    tcaselect(t);
    sensor[t].start_conversionD2();
  }
  //delay(16);
  for (uint8_t t=0; t<8; t++) {
    tcaselect(t);
    sensor[t].read_conversionD2();
  }
  for (uint8_t t=0; t<8; t++) {
    tcaselect(t);
    sensor[t].calculate();
    Serial.print(sensor[t].pressure());
    Serial.print(" , ");
  }
  Serial.println("");
}
