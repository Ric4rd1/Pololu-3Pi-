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
PololuMenu<typeof(display)> menu;            // OLED Menu

unsigned int calibrationSpeed = 80; // motor speeds for calibration
unsigned int sensitivityBlack = 300;
unsigned int sensitivityWhite = 700;

// Flags
bool colorLine = 0; // 0 = black, 1 = white
bool mode = 0; // 0 = recognition, 1 = optimized solution
bool handAlg = 0; // 0 = leftHand algorithm, 1 = rightHand algorithm
bool finish = 0;
// Sensor values 
bool left = 0;
bool mid = 0;
bool right = 0;

bool leftLast = 0;
bool midLast = 0;
bool rightLast = 0;

// Array to store decitions
char decitions[100];
char simplifiedDecitions[100];
// Simplify array iterators
int i = 0, j = 0;
// Store decition iterator
int decitionIndex = 0;
int simplifiedDecitionIndex = 0;

void storeDecision(char decition) {
  if (decitionIndex < 100) {
    decitions[decitionIndex++] = decition;
  }
}

// Restart decision index
void cleardecitions() {
  decitionIndex = 0;
}

void replace(bool first){
    char before = decitions[i-1];
    char after = decitions[i+1];

    if (first == true) {
        before = simplifiedDecitions[j-1];
        j--;
    }

    // Case: LUL
      if (before == 'L' && after == 'L') {
          simplifiedDecitions[j++] = 'S';
      }
      // Case: LUS
      else if (before == 'L' && after == 'S') {
          simplifiedDecitions[j++] = 'R';
      }
      // Case: RUR
      else if (before == 'R' && after == 'R') {
          simplifiedDecitions[j++] = 'S';
      }
      // Case: RUS
      else if (before == 'R' && after == 'S') {
          simplifiedDecitions[j++] = 'L';
      }
      // Case: SUL
      else if (before == 'S' && after == 'L') {
          simplifiedDecitions[j++] = 'R'; 
      }
      // Case: SUR
      else if (before == 'S' && after == 'R') {
          simplifiedDecitions[j++] = 'L';
      }
      // Case: SUS
      else if (before == 'S' && after == 'S') {
          simplifiedDecitions[j++] = 'U';
      }
      // Case: LUR
      else if (before == 'L' && after == 'R') {
          simplifiedDecitions[j++] = 'U';
      }
      // Case: RUL
      else if (before == 'R' && after == 'L') {
          simplifiedDecitions[j++] = 'U';
      }
}

void replaceExeption(){
    j--;
    char before = simplifiedDecitions[j-1];
    char after = decitions[i];

    // Case: LUL
    if (before == 'L' && after == 'L') {
        simplifiedDecitions[j-1] = 'S';
    }
    // Case: LUS
    else if (before == 'L' && after == 'S') {
        simplifiedDecitions[j-1] = 'R';
    }
    // Case: RUR
    else if (before == 'R' && after == 'R') {
        simplifiedDecitions[j-1] = 'S';
    }
    // Case: RUS
    else if (before == 'R' && after == 'S') {
        simplifiedDecitions[j-1] = 'L';
    }
    // Case: SUL
    else if (before == 'S' && after == 'L') {
        simplifiedDecitions[j-1] = 'R';
    }
    // Case: SUR
    else if (before == 'S' && after == 'R') {
        simplifiedDecitions[j-1] = 'L';
    }
    // Case: SUS
    else if (before == 'S' && after == 'S') {
        simplifiedDecitions[j-1] = 'U';
    }
    // Case: LUR
    else if (before == 'L' && after == 'R') {
        simplifiedDecitions[j-1] = 'U';
    }
    // Case: RUL
    else if (before == 'R' && after == 'L') {
        simplifiedDecitions[j-1] = 'U';
    }
    simplifiedDecitions[j] = '0'; 
    i++;
}

bool simplify() {
    bool firstComp = true; // Flag first comparison
    bool exeption = false; // Flag for exceptions (RUL, LUR)
    bool finished = false; // Flag for finished simplification

    if (decitions[i] == 'F') {
        finished = true;
        simplifiedDecitions[j] = 'F';
        return finished;
    }
    
    // Iterate over the decitions array
    while (decitions[i] != 'U' && decitions[i] != 'F') {

        if ((simplifiedDecitions[j-1] == 'U')&&((j-1) != 0)) {
            exeption = true;
            break;
        }
        
        if (decitions[i+1] != 'U' && decitions[i] != 'U')
            simplifiedDecitions[j++] = decitions[i];

        i++;
        firstComp = false;
    }

    if(exeption == true){
        replaceExeption();  
        return finished;
    }

    if ((decitions[i] == 'U')) {
        replace(firstComp);
        // Jump after comparison
        i += 2; 
    } 
    return finished;
}

// print the decitions array to the Serial Monitor
void printDecitions() {
  Serial.print("decitions: ");
  for (int i = 0; i < decitionIndex; i++) {
    Serial.print(decitions[i]);
    if (i < decitionIndex - 1) {
      Serial.print(", ");
    }
  }
  Serial.println();
}

/*
void printSimplifiedDecitions() {
  Serial.print("Simplified decitions: ");
  for (int i = 0; i < decitionIndex; i++) {
    Serial.print(simplifiedDecitions[i]);
    if (i < decitionIndex - 1) {
      Serial.print(", ");
    }
  }
  Serial.println();
}
*/

void printSimplifiedDecitions() {
  Serial.print("Simplified Decisions: ");
  for (int i = 0; simplifiedDecitions[i] != 'F'; i++) {
    Serial.print(simplifiedDecitions[i]);
    if (simplifiedDecitions[i + 1] != 'F') {
      Serial.print(", ");
    }
  }
  Serial.println();
}

void selectColor(){
  display.clear();
  display.print(F("MAZE"));
  display.gotoXY(0,1);
  display.print(F("SOLVER"));
  while (!buttonB.getSingleDebouncedPress());

  static const PololuMenuItem items[] = {
    { F("White"), whiteMode },
    { F("Black"), blackMode },
  };

  menu.setItems(items, 2);
  menu.setDisplay(display);
  menu.setButtons(buttonA, buttonB, buttonC);

  while(!menu.select());

  display.gotoXY(0,1);
  display.print("OK!  ...");
}

void selectAlg(){
  display.clear();
  display.print(F("SELECT"));
  display.gotoXY(0,1);
  display.print(F("ALG"));
  delay(1000);

  static const PololuMenuItem items[] = {
    { F("RIGHT"), rightHand },
    { F("LEFT"), leftHand },
  };

  menu.setItems(items, 2);
  menu.setDisplay(display);
  menu.setButtons(buttonA, buttonB, buttonC);

  while(!menu.select());

  display.gotoXY(0,1);
  display.print("OK!  ...");
}

void selectMode(){
  display.clear();
  display.print(F("SELECT"));
  display.gotoXY(0,1);
  display.print(F("MODE"));
  delay(1000);

  static const PololuMenuItem items[] = {
    { F("RECOGNITION"), recognitionMode },
    { F("OPTIMIZED"), optimizedMode },
  };

  menu.setItems(items, 2);
  menu.setDisplay(display);
  menu.setButtons(buttonA, buttonB, buttonC);

  while(!menu.select());

  display.gotoXY(0,1);
  display.print("OK!  ...");
}


void whiteMode(){
  colorLine = 1;
}

void blackMode(){
  colorLine = 0;
}

void leftHand(){
  handAlg = 0;
}

void rightHand(){
  handAlg = 1;
}

void recognitionMode(){
  mode = 0;
}

void optimizedMode(){
  mode = 1;
}

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
  // proportional = 20 , derivative = 256
  uint16_t proportional = 10;  // coefficient proportional (error in the present) 256 = 1
  uint16_t derivative = 64;   // coefficient derivative (rate of change) 256 = 1
  uint16_t maxSpeed = 60;
  int16_t minSpeed = 40;
  uint16_t baseSpeed = 45;
  uint16_t position;
  int error = 0;
  readSensors();
  leftLast = left;
  midLast = mid;
  rightLast = right;
  while (1) {
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

void takeDecitionLeft() {
  switch (left << 2 | mid << 1 | right) {
    case 0:  // Dead End
      //motors.setSpeeds(int16_t leftSpeed, int16_t rightSpeed)
      display.clear();
      display.print("DEAD END");
      motors.setSpeeds(-60, 60);
      delay(685);
      motors.setSpeeds(0, 0);
      storeDecision('U');
      break;
    case 1:  // Right turn only
      display.clear();
      display.print("RIGHT ONLY");
      motors.setSpeeds(60, -60);
      delay(345);
      motors.setSpeeds(0, 0);
      break;
    case 2:  // Straight
      display.clear();
      display.print("STRAIGHT");
      // storeDecision('S');
      break;
    case 3:  // Straight or Right
      display.clear();
      display.print("STRAIGHT OR");
      display.gotoXY(0, 1);
      display.print("RIGHT");
      storeDecision('S');
      break;
    case 4:  // Left turn only
      display.clear();
      display.print("LEFT TURN");
      display.gotoXY(0, 1);
      display.print("ONLY");
      motors.setSpeeds(-60, 60);
      delay(345);
      motors.setSpeeds(0, 0);
      break;
    case 5:  // Left or Right (T)
      display.clear();
      display.print("LEFT OR");
      display.gotoXY(0, 1);
      display.print("RIGHT (T)");
      motors.setSpeeds(-60, 60);
      delay(345);
      motors.setSpeeds(0, 0);
      storeDecision('L');
      break;
    case 6:  // Straight or Left
      display.clear();
      display.print("STRAIGHT OR");
      display.gotoXY(0, 1);
      display.print("LEFT");
      motors.setSpeeds(-60, 60);
      delay(345);
      motors.setSpeeds(0, 0);
      storeDecision('L');
      break;
    case 7:  // Four Way
      display.clear();
      display.print("FOUR WAY");
      
      leftLast = left;
      midLast = mid;
      rightLast = right;
      motors.setSpeeds(-60, 60);
      delay(45);
      readSensors();
      if((left == leftLast) && (right == rightLast) && (mid == midLast)){
        motors.setSpeeds(0, 0);
        finish = 1;
        storeDecision('F');
        break;
      }
      delay(300);
      motors.setSpeeds(0, 0);
      storeDecision('L');
      break;
  }
}

void takeDecitionRight() {
  switch (left << 2 | mid << 1 | right) {
    case 0:  // Dead End
      display.clear();
      display.print("DEAD END");
      motors.setSpeeds(-60, 60);
      delay(685);
      motors.setSpeeds(0, 0);
      storeDecision('U');
      break;
    case 1:  // Right turn only
      display.clear();
      display.print("RIGHT ONLY");
      motors.setSpeeds(60, -60);
      delay(345);
      motors.setSpeeds(0, 0);
      break;
    case 2:  // Straight
      display.clear();
      display.print("STRAIGHT");
      // storeDecision('S');
      break;
    case 3:  // Straight or Right
      display.clear();
      display.print("STRAIGHT OR");
      display.gotoXY(0, 1);
      display.print("RIGHT");
      motors.setSpeeds(60, -60);
      delay(345);
      motors.setSpeeds(0, 0);
      storeDecision('R');
      break;
    case 4:  // Left turn only
      display.clear();
      display.print("LEFT TURN");
      display.gotoXY(0, 1);
      display.print("ONLY");
      motors.setSpeeds(-60, 60);
      delay(345);
      motors.setSpeeds(0, 0);
      break;
    case 5:  // Left or Right (T)
      display.clear();
      display.print("LEFT OR");
      display.gotoXY(0, 1);
      display.print("RIGHT (T)");
      motors.setSpeeds(60, -60);
      delay(345);
      motors.setSpeeds(0, 0);
      storeDecision('R');
      break;
    case 6:  // Straight or Left
      display.clear();
      display.print("STRAIGHT OR");
      display.gotoXY(0, 1);
      display.print("LEFT");
      storeDecision('S');
      break;
    case 7:  // Four Way
      display.clear();
      display.print("FOUR WAY");
      
      leftLast = left;
      midLast = mid;
      rightLast = right;
      motors.setSpeeds(60, -60);
      delay(45);
      readSensors();
      if((left == leftLast) && (right == rightLast) && (mid == midLast)){
        motors.setSpeeds(0, 0);
        finish = 1;
        storeDecision('F');
        break;
      }
      delay(300);
      motors.setSpeeds(0, 0);
      storeDecision('R');
      break;
  }
}

void takeDecitionSimplified() {
  bool flag = false;
  switch (left << 2 | mid << 1 | right) {
    case 1:  // Right turn only
      display.clear();
      display.print("RIGHT ONLY");
      motors.setSpeeds(60, -60);
      delay(345);
      motors.setSpeeds(0, 0);
      flag = true;
      break;

    case 2:  // Straight
      display.clear();
      display.print("STRAIGHT");
      flag = true;
      break;

    case 4:  // Left turn only
      display.clear();
      display.print("LEFT TURN");
      display.gotoXY(0, 1);
      display.print("ONLY");
      motors.setSpeeds(-60, 60);
      delay(345);
      motors.setSpeeds(0, 0);
      flag = true;
      break;
    default:
      break;
  }

  if (flag == false){
    switch (simplifiedDecitions[simplifiedDecitionIndex]) {
      case 'R':
        display.clear();
        display.print("RIGHT ONLY");
        motors.setSpeeds(60, -60);
        delay(345);
        motors.setSpeeds(0, 0);
        break;

      case 'L':
        display.clear();
        display.print("LEFT TURN");
        display.gotoXY(0, 1);
        display.print("ONLY");
        motors.setSpeeds(-60, 60);
        delay(345);
        motors.setSpeeds(0, 0);
        break;

      case 'S':
        display.clear();
        display.print("STRAIGHT");
        motors.setSpeeds(40,40);
        delay(175);
        motors.setSpeeds(0,0);
        break;
      
      case 'F':
        finish = 1;
        break;
    }
  }

  if (flag == false)
    simplifiedDecitionIndex++;
}

void recognition(){
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
  // white track 85, update sensors, 90
  // Black track 25, update sensors, 150
  motors.setSpeeds(0, 0);
  if(handAlg == 0){
    takeDecitionLeft();
  }else if(handAlg == 1){
    takeDecitionRight();
  }

  if (finish == 1){
    display.clear();
    display.gotoXY(0, 0);
    display.print(" FINISH !!!");
    display.gotoXY(0, 2);
    display.print(" *B TO");
    display.gotoXY(0, 3);
    display.print(" RESTART");
    while (!buttonB.getSingleDebouncedPress());
    setup();
  }
}

void optimized(){
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
  motors.setSpeeds(0, 0);
  takeDecitionSimplified();

  if (finish == 1){
    display.clear();
    display.gotoXY(0, 0);
    display.print(" FINISH !!!");
    display.gotoXY(0, 2);
    display.print(" *B TO");
    display.gotoXY(0, 3);
    display.print(" RESTART");
    while (!buttonB.getSingleDebouncedPress());
    i = 0;
    j = 0;
    setup();
  }
}

void setup() {
  // Serial monitor baudrate
  Serial.begin(9600);
  // Select line color
  selectColor();

  // If robot has already finished the maze
  if(finish == 1){
    finish = 0;
    
    //  Simplify decitions
    simplifiedDecitionIndex = 0;
    bool finishSimplify = false;
    while (finishSimplify == false){
      finishSimplify = simplify();
    }
    printDecitions();
    printSimplifiedDecitions();

    selectMode();
  }

  // Recognition mode
  if(mode == 0){
    cleardecitions();
    selectAlg(); // Select right or left algorithm
  } 

  // Set display Layout
  display.setLayout11x4();
  display.clear();
  display.gotoXY(0, 0);
  display.print(" PRESS B ");
  display.gotoXY(0,1);
  display.print(" TO");
  display.gotoXY(0,2);
  display.print(" CALIBRATE");

  // Wait for B button press
  while (!buttonB.getSingleDebouncedPress());

  // Calibration
  display.clear();
  display.gotoXY(0, 1);
  display.print("CALIBRATING");
  calibrateSensors();
  display.clear();
  display.gotoXY(0, 1);
  display.print("   READY   ");
  display.gotoXY(0, 2);
  display.print("*B TO START");
  while (!buttonB.getSingleDebouncedPress());
  delay(2000);

  if ((mode == 1)&&(simplifiedDecitions[0] == 'U')){
      motors.setSpeeds(-60, 60);
      delay(685);
      motors.setSpeeds(0, 0);
      simplifiedDecitionIndex++;
    }
}

void loop() {

  if(mode == 0){
    recognition();
  }else if(mode == 1){
    optimized();
  }

}
