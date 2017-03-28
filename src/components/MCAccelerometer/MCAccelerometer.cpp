#include <components/MCAccelerometer/MCAccelerometer.h>

// Returns the current orientation of the cube
int MCAccelerometer::getCurrentOrientation() { return random(1, 7); }

MCAccelerometer MC_Accelerometer;
