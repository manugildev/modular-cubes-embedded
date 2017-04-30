#include <components/MCAccelerometer/MCAccelerometer.h>
#include <MMA8451.h>
#include <Sensor.h>
#include <data/ModularCube.h>

#include <Wire.h>

MMA8451 mma = MMA8451();

void MCAccelerometer::setup(void) {
  if (!mma.begin()) {
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
}
// Returns the current orientation of the cube
int MCAccelerometer::getCurrentOrientation() { return currentOrientation; }

MCAccelerometer MC_Accelerometer;
