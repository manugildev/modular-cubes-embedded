#include <components/Accelerometer/Accelerometer.h>

// Returns the current orientation of the cube
int Accelerometer::getCurrentOrientation() { return random(1, 7); }

Accelerometer accelerometer;
