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
  Serial.println("\nSetup done");
  Serial.println("\nSTATIC    S_2       S_3       S_4       DYN       S_6       S_7       S_8");
}


void loop() {
  while (!Serial);
  //delay(10);
  Wire.begin();
  for (uint8_t t=0; t<8; t++) {
    tcaselect(t);
    for (uint8_t addr = 0; addr<=127; addr++) {
      if (addr == TCAADDR) continue;
      uint8_t data;
      if (! twi_writeTo(addr, &data, 0, 1, 1)) {
        unsigned long timeBegin = millis();
        Wire.begin();
        while (!sensor.init()) {
          }
        sensor.setModel(MS5837::MS5837_30BA);
        sensor.setFluidDensity(997); // kg/m^3 (freshwater, 1029 for seawater)
        sensor.read();
        sensor.calculate();
        unsigned long timeEnd = millis();
        unsigned long duration = timeEnd - timeBegin;
        //Serial.print(duration);
        Serial.print(sensor.pressure());
        Serial.print(" , ");
    //sensor.calculate();
    //Serial.print(sensor.pressure());
    //Serial.print(" , ");
      }
    }
  }
  Serial.println("");
  //if (sensor.pressure() == 1040) {
    //exit(0);
  }
