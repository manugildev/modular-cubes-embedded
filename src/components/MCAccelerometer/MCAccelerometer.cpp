#include <components/MCAccelerometer/MCAccelerometer.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
#include <data/ModularCube.h>

#include <Wire.h>

Adafruit_MMA8451 mma = Adafruit_MMA8451();

void MCAccelerometer::setup(void) {
  if (! mma.begin()) {
    Serial.println("Couldnt start");
    while (1);
  }
  Serial.println("MMA8451 found!");

  mma.setRange(MMA8451_RANGE_2_G);

  Serial.print("Range = "); Serial.print(2 << mma.getRange());
  Serial.println("G");
  currentOrientation = 0;
  previousOrientation = 0;
}

void MCAccelerometer::loop() {

  mma.read();
  int tempOrientation = 0;
  if (mma.x > -3500 && mma.x < 3500 && mma.y > -3500 && mma.y <3500){
    if(mma.z > 3500){
      tempOrientation = 1;
    }else if (mma.z < -3500){
      tempOrientation = 6;
    }
  }else if(mma.x > -3500 && mma.x < 3500 && mma.z < 3500 &&  mma.z > -3500){
    if(mma.y > 3500){
      tempOrientation = 3;
    }else if (mma.y < -3500){
      tempOrientation = 4;
    }
  }else if(mma.y < 3500 && mma.y > -3000 && mma.z > -3500 && mma.z < 3500){
   if(mma.x > 3500){
      tempOrientation = 2;
    }else if (mma.x < -3500){
      tempOrientation = 5;
    }
  }
  //currentOrientation = mma.getOrientation();
  currentOrientation = tempOrientation;
  if(previousOrientation != currentOrientation && currentOrientation != 0){
    currentOrientation = tempOrientation;
    previousOrientation = currentOrientation;
    Cube.updateOrientation();
  }
  // Read the 'raw' data in 14-bit counts
  // mma.read();
  // Serial.print("X:\t"); Serial.print(mma.x);
  // Serial.print("\tY:\t"); Serial.print(mma.y);
  // Serial.print("\tZ:\t"); Serial.print(mma.z);
  // Serial.println();
  //
  // /* Get a new sensor event */
  // sensors_event_t event;
  // mma.getEvent(&event);
  //
  // /* Display the results (acceleration is measured in m/s^2) */
  // Serial.print("X: \t"); Serial.print(event.acceleration.x); Serial.print("\t");
  // Serial.print("Y: \t"); Serial.print(event.acceleration.y); Serial.print("\t");
  // Serial.print("Z: \t"); Serial.print(event.acceleration.z); Serial.print("\t");
  // Serial.println("m/s^2 ");
  //
  // /* Get the orientation of the sensor */
  // uint8_t o = mma.getOrientation();
  //
  // switch (o) {
  //   case MMA8451_PL_PUF:
  //     Serial.println("Portrait Up Front");
  //     break;
  //   case MMA8451_PL_PUB:
  //     Serial.println("Portrait Up Back");
  //     break;
  //   case MMA8451_PL_PDF:
  //     Serial.println("Portrait Down Front");
  //     break;
  //   case MMA8451_PL_PDB:
  //     Serial.println("Portrait Down Back");
  //     break;
  //   case MMA8451_PL_LRF:
  //     Serial.println("Landscape Right Front");
  //     break;
  //   case MMA8451_PL_LRB:
  //     Serial.println("Landscape Right Back");
  //     break;
  //   case MMA8451_PL_LLF:
  //     Serial.println("Landscape Left Front");
  //     break;
  //   case MMA8451_PL_LLB:
  //     Serial.println("Landscape Left Back");
  //     break;
  //   }
  // Serial.println();
  // delay(500);
  //
  // Serial.println(o);
}
// Returns the current orientation of the cube
int MCAccelerometer::getCurrentOrientation() { return currentOrientation; }

MCAccelerometer MC_Accelerometer;
