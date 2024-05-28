#include <Pololu3piPlus32U4.h>
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

#define NUM_SENSORS 5                        // Number of infrarred sensors
unsigned int lineSensorValues[NUM_SENSORS];  // Array used to store line sensor data

unsigned int calibrationSpeed = 80;  // Motor speed for calibration
unsigned int sensitivityBlack = 300; // Threshold for detecting Black lines
unsigned int sensitivityWhite = 700; // Threshold for detecting White lines

bool colorLine = 0; //0 = black, 1 = white

// Sensor values 
bool left = 0;
bool mid = 0;
bool right = 0;

bool leftLast = 0;
bool midLast = 0;
bool rightLast = 0;

bool finish = 0; // Finish flag

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
  uint16_t prediction;
  if (colorLine == 0){
    prediction = lineSensors.readLineBlack(lineSensorValues);
  }else if(colorLine == 1){
    prediction = lineSensors.readLineWhite(lineSensorValues);
  }
  
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

void readSensors(){
  if(colorLine == 0){
    lineSensors.readLineBlack(lineSensorValues);
    left = (lineSensorValues[0] < sensitivityBlack) ? 0 : 1;
    mid = (lineSensorValues[2] < sensitivityBlack) ? 0 : 1;
    right = (lineSensorValues[4] < sensitivityBlack) ? 0 : 1;
  } else if (colorLine == 1){
    lineSensors.readLineWhite(lineSensorValues);
    left = (lineSensorValues[0] < sensitivityWhite) ? 1 : 0;
    mid = (lineSensorValues[2] < sensitivityWhite) ? 1 : 0;
    right = (lineSensorValues[4] < sensitivityWhite) ? 1 : 0;
  }
}

void showSensors(){
  display.clear();
  display.noAutoDisplay();
  display.gotoXY(0,0);
  display.print("left    mid    right");
  display.gotoXY(1,1);
  display.print(left);
  display.gotoXY(9,1);
  display.print(mid);
  display.gotoXY(17,1);
  display.print(right);
  display.display();
}

void followLine() {
  int16_t lastError = 0;
  uint16_t proportional = 10;  // coefficient proportional (error in the present) 256 = 1
  uint16_t derivative = 64;   // coefficient derivative (rate of change) 256 = 1
  uint16_t maxSpeed = 60;
  int16_t minSpeed = 40;
  uint16_t baseSpeed = 45;
  uint16_t position;
  int error = 0;

  readSensors(); // Read sensors before moving
  leftLast = left;
  midLast = mid;
  rightLast = right;

  // Stay in loop while readings in sensors don't change
  while (1) {
    // Asign last recorded values
    leftLast = left;
    midLast = mid;
    rightLast = right;

    readSensors(); // Read sensors

    // If readings have changed from previous, move forward 1cm and break
    if((left != leftLast) || (right != rightLast) || (mid != midLast)){
      motors.setSpeeds(30, 30);
      delay(70);
      motors.setSpeeds(0, 0);
      break;
    }
    
    // Read line sensors
    if (colorLine == 0){
      position = lineSensors.readLineBlack(lineSensorValues);
    } else if (colorLine == 1){
      position = lineSensors.readLineWhite(lineSensorValues);
    }
    
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

  }  // While
}

// Take Decition based on sensor recorded values
void takeDecition() {
  switch (left << 2 | mid << 1 | right) {
    case 0:  // Dead End
      //motors.setSpeeds(int16_t leftSpeed, int16_t rightSpeed)
      display.clear();
      display.print("Dead End");
      motors.setSpeeds(-60, 60);
      delay(685);
      motors.setSpeeds(0, 0);
      
      break;
    case 1:  // Right turn only
      display.clear();
      display.print("Right turn only");
      motors.setSpeeds(60, -60);
      delay(345);
      motors.setSpeeds(0, 0);
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
      motors.setSpeeds(-60, 60);
      delay(345);
      motors.setSpeeds(0, 0);
      break;
    case 5:  // Left or Right (T)
      display.clear();
      display.print("Left of Right (T)");
      motors.setSpeeds(-60, 60);
      delay(345);
      motors.setSpeeds(0, 0);
      break;
    case 6:  // Straight or Left
      display.clear();
      display.print("Straight or Left");
      motors.setSpeeds(-60, 60);
      delay(345);
      motors.setSpeeds(0, 0);
      break;
    case 7:  // Four Way
      display.clear();
      display.print("Four Way");
      
      // Check if robot is at the end of maze
      leftLast = left;
      midLast = mid;
      rightLast = right;
      motors.setSpeeds(-60, 60);
      delay(45);
      readSensors();
      if((left == leftLast) && (right == rightLast) && (mid == midLast)){
        motors.setSpeeds(0, 0);
        finish = 1;
        break;
      }
      delay(300);
      motors.setSpeeds(0, 0);
      break;
  }
}

void setup() {
  // Set display Layout
  display.setLayout21x8();

  display.clear();
  display.print("PRESS B TO CALIBRATE");

  // Wait for B button press
  while (!buttonB.getSingleDebouncedPress());
  display.clear();
  display.print("CALIBRATING...");
  calibrateSensors();
  display.clear();

  // 345 90grados
  // 685 180grados

  display.print("READY");
  while (!buttonB.getSingleDebouncedPress());

  
}

void loop() {
  followLine();
  motors.setSpeeds(40, 40);
  if (colorLine == 0){
    delay(25);
    readSensors();
    delay(150);
  }else if (colorLine == 1){
    delay(85);
    readSensors();
    delay(90);
  }
  // Total delay 175
  // white track 85, 90
  // Black track 25, 150
  motors.setSpeeds(0, 0);
  takeDecition();

  if (finish == 1){
    display.clear();
    display.print("     FINISH !!!");
    display.gotoXY(0, 3);
    display.print("  Press B to Restart");
    finish = 0;
    while (!buttonB.getSingleDebouncedPress());
    setup();
  }

}
