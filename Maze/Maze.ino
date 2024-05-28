#include <Pololu3piPlus32U4.h>
#include <PololuMenu.h>
using namespace Pololu3piPlus32U4;

OLED display;
Buzzer buzzer;
ButtonA buttonA;
ButtonB buttonB;
ButtonC buttonC;
LineSensors lineSensors;
BumpSensors bumpSensors;
Motors motors;
Encoders encoders;

#define NUM_SENSORS 5                        // Number of infrarred sensores
unsigned int lineSensorValues[NUM_SENSORS];  // Array used to store line sensor data
PololuMenu<OLED> menu;                       //menu object for oled display

/*########## GLOBAL VARIABLES ##########*/

uint16_t calibrationSpeed = 80; // motor speeds for calibration
uint8_t sensitivity = 800; // Threshold for identifing line 
                           // (sensor < sensitivity = 1, sensor > sensitivity = 0)

// Robot constants
const float rad = 1.6;  // Wheel Radius
const float len = 9.0;  // Distance Between wheels

int16_t error = 0;
float angLeft, angRight;

// Sensor values 
bool left = 0;
bool mid = 0;
bool right = 0;

bool leftLast = 0;
bool midLast = 0;
bool rightLast = 0;

bool threeSensors[3] = {0,0,0};

void calibrateSensors() {
  display.clear();

  // Wait 2 seconds and then begin automatic sensor calibration
  // by rotating in place to sweep the sensors over the line
  delay(2000);
  for (uint16_t i = 0; i < 80; i++) {
    if (i > 20 && i <= 60) {
      motors.setSpeeds(-(int16_t)calibrationSpeed, calibrationSpeed);
    } else {
      motors.setSpeeds(calibrationSpeed, -(int16_t)calibrationSpeed);
    }
    // Calibrate sensors
    lineSensors.calibrate();
  }
  motors.setSpeeds(0, 0);
}

void showReadings(){
  uint16_t prediction = lineSensors.readLineBlack(lineSensorValues);
  display.noAutoDisplay();
  display.clear();
  display.print(lineSensorValues[0]); //Sensor DN1
  display.gotoXY(0,1);
  display.print(lineSensorValues[1]); //Sensor DN2
  display.gotoXY(0,2);
  display.print(lineSensorValues[2]); //Sensor DN3
  display.gotoXY(0,3);
  display.print(lineSensorValues[3]); //Sensor DN4
  display.gotoXY(0,4);
  display.print(lineSensorValues[4]); //Sensor DN5
  display.gotoXY(0,5);
  display.print("Prediction: "); //Sensor DN5
  display.gotoXY(0,6);
  display.print(prediction); //Sensor DN5
  display.display();
}

void showSensors(){
  
}

void readSensors(){
  lineSensors.readLineBlack(lineSensorValues);
  left = (lineSensorValues[0] < sensitivity) ? 0 : 1;
  mid = (lineSensorValues[2] < sensitivity) ? 0 : 1;
  right = (lineSensorValues[4] < sensitivity) ? 0 : 1;
  threeSensors[0] = left;
  threeSensors[1] = mid;
  threeSensors[2] = right;
}

void getAngles() {
  angLeft = float(encoders.getCountsAndResetLeft());
  angLeft = angLeft * (1.0 / 12.0);   // Encoder pulses to motor revolutions
  angLeft = angLeft * (1.0 / 29.86);  // Motor revolutions to wheel revolutions
  angLeft = angLeft * (360.0 / 1.0);  // Wheel revolutions to degrees

  angRight = float(encoders.getCountsAndResetRight());
  angRight = angRight * (1.0 / 12.0);   // Encoder pulses to motor revolutions
  angRight = angRight * (1.0 / 29.86);  // Motor revolutions to wheel revolutions
  angRight = angRight * (360.0 / 1.0);  // Wheel revolutions to degrees
}

void followLine() {
  int16_t lastError = 0;
  // proportional = 20 , derivative = 256
  uint16_t proportional = 20;  // coefficient proportional (error in the present) 256 = 1
  uint16_t derivative = 128;   // coefficient derivative (rate of change) 256 = 1
  uint16_t maxSpeed = 60;
  int16_t minSpeed = 10;
  uint16_t baseSpeed = 45;
  uint16_t position;
  error = 0;
  readSensors();
  leftLast = left;
  midLast = mid;
  rightLast = right;
  while (1) {
    // Read line sensors
    position = lineSensors.readLineBlack(lineSensorValues);

    // Calculate error
    // Perfect calibration
    error = position - 2000;

    // PD control
    int16_t speedDifference = error * (int32_t)proportional / 256 + (error - lastError) * (int32_t)derivative / 256;

    // Error is now last error
    lastError = error;

    // Set left and right speeds
    int16_t leftSpeed = (int16_t)baseSpeed + speedDifference;
    int16_t rightSpeed = (int16_t)baseSpeed - speedDifference;

    // Constrain speeds
    leftSpeed = constrain(leftSpeed, minSpeed, (int16_t)maxSpeed);
    rightSpeed = constrain(rightSpeed, minSpeed, (int16_t)maxSpeed);

    // Set speeds
    motors.setSpeeds(leftSpeed, rightSpeed);

    leftLast = left;
    midLast = mid;
    rightLast = right;

    readSensors();

    if((left != leftLast) || (right != rightLast) || (mid != midLast)){
      motors.setSpeeds(30, 30);
      delay(70);
      motors.setSpeeds(0, 0);
      break;
    }

  }  // While
}

void rotate(float angRef) {
  float ang = 0;
  int16_t speed = 60;

  error = 0;
  angLeft = 0;
  angRight = 0;

  while (1) {
    getAngles();  // Update angle readings from encoders

    // Calculate angle
    ang = ang + (rad * ((angRight - angLeft) / len));
    error = angRef - ang;
    /*
    display.clear();
    display.gotoXY(0,0);
    display.print("error: ");
    display.print(error);
    display.gotoXY(0, 1);
    display.print("ang: ");
    display.print(ang);
    */
    if (error > 5) {
      motors.setSpeeds(-speed, speed);
    } else if (error < -1) {
      motors.setSpeeds(speed, -speed);
    } else {
      motors.setSpeeds(0, 0);
      break;
    }

  }    //While
}

void takeDecition() {
  // Update sensor values
  readSensors();
  switch (left << 2 | mid << 1 | right) {
    case 0:  // Dead End
      //motors.setSpeeds(int16_t leftSpeed, int16_t rightSpeed)
      display.clear();
      display.print("Dead End");
      rotate(180);
      break;
    case 1:  // Right turn only
      display.clear();
      display.print("Right turn only");
      rotate(180);
      break;
    case 2:  // Straight
      display.clear();
      display.print("Straight");
      break;
    case 3:  // Straight or Right
      display.clear();
      display.print("Straight or Right");
      break;
    case 4:  // Left turn only
      display.clear();
      display.print("Left turn only");
      rotate(90);
      break;
    case 5:  // Left or Right (T)
      display.clear();
      display.print("Left of Right (T)");
      rotate(90);
      break;
    case 6:  // Straight or Left
      display.clear();
      display.print("Straight or Left");
      rotate(90);
      break;
    case 7:  // Four Way
      display.clear();
      display.print("Four Way");
      rotate(90);
      break;
  }
}

void setup() {
  // Set display Layout
  display.setLayout21x8();

  display.print("PRESS B TO CALIBRATE");

  // Wait for B button press
  while (!buttonB.getSingleDebouncedPress());
  display.clear();
  display.print("CALIBRATING...");
  calibrateSensors();
  display.clear();
  while (!buttonB.getSingleDebouncedPress()){
    showReadings();
  }
}

void loop() {
  followLine();
  delay(10);
  takeDecition();
  delay(10);

}