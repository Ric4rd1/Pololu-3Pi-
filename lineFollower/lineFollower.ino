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

#define NUM_SENSORS 5 // Number of infrarred sensores
unsigned int lineSensorValues[NUM_SENSORS]; // Array used to store line sensor data
PololuMenu<OLED> menu; //menu object for oled display

uint16_t maxSpeed;
int16_t minSpeed;
uint16_t position;

uint16_t baseSpeed;
uint16_t calibrationSpeed;

uint16_t proportional; // coefficient proportional (error in the present) 256 = 1
uint16_t derivative; // coefficient derivative (rate of change) 256 = 1

int16_t error;
int16_t lastError = 0;

bool lineColor; // balck = 0, white = 1

// Parameters for Black Line
void selectBlack(){
  lineColor = 0;
  maxSpeed = 340;
  minSpeed = 0;
  baseSpeed = 300;
  calibrationSpeed = 80;
  proportional = 50; // P coefficient = 1/4
  derivative = 256; // D coefficient = 1
}

// Parameters for White Line
void selectWhite(){
  lineColor = 1;
  maxSpeed = 340;
  minSpeed = 0;
  baseSpeed = 300;
  calibrationSpeed = 80;
  proportional = 50; // P coefficient = 1/4
  derivative = 256; // D coefficient = 1
}

// Select from black or white line mode
void selectMode(){
  //OLED print
  display.clear();
  display.print(F("Select"));
  display.gotoXY(0,1);
  display.print(F("Color "));
  delay(1000);

  // Items in menu
  static const PololuMenuItem items[] = {
    { F("Black"), selectBlack },
    { F("White"), selectWhite },
  };

  // Setup menu
  menu.setItems(items, 2);
  menu.setDisplay(display);
  menu.setBuzzer(buzzer);
  menu.setButtons(buttonA, buttonB, buttonC);

  // Wait for press
  while(!menu.select());

  display.clear();
  display.gotoXY(0,1);
  display.print("OK!");
  delay(1000);
}

void calibrateSensors(){
  display.clear();

  // Wait 2 seconds and then begin automatic sensor calibration
  // by rotating in place to sweep the sensors over the line
  delay(2000);
  for(uint16_t i = 0; i < 160; i++)
  {
    if (i > 40 && i <= 120)
    {
      motors.setSpeeds(-(int16_t)calibrationSpeed, calibrationSpeed);
    }
    else
    {
      motors.setSpeeds(calibrationSpeed, -(int16_t)calibrationSpeed);
    }
    // Calibrate sensors
    lineSensors.calibrate();
  }
  motors.setSpeeds(0, 0);
}

void showReadings(){
  // Show readings while b button has not been pressed
  while(!buttonB.getSingleDebouncedPress()){
    // Read lines acording to line color mode
    if (lineColor == 0){
      position = lineSensors.readLineBlack(lineSensorValues);
    } else{
      position = lineSensors.readLineWhite(lineSensorValues);
    }

  // Display reference
  display.noAutoDisplay();
  display.clear();
  display.print(F("Position"));
  display.gotoXY(0, 1);
  display.print(position);
  display.display();
  }
}

void setup() {
  // Play a little welcome song
  buzzer.play(">g32>>c32");

  //Select line color
  selectMode();

  display.clear();
  display.print(F("Press B"));
  display.gotoXY(0, 1);
  display.print(F("to calib"));
  // Wait for B button press
  while(!buttonB.getSingleDebouncedPress());

  calibrateSensors();
  showReadings();

  // Count down
  display.clear();
  display.print(F("3..."));
  buzzer.play("L4 V7 a.R");
  while(buzzer.isPlaying());
  display.clear();
  display.print(F("2..."));
  buzzer.play("L4 V7 a.R");
  while(buzzer.isPlaying());
  display.clear();
  display.print(F("1..."));
  buzzer.play("L4 V7 a.R");
  while(buzzer.isPlaying());
  display.clear();
  display.print(F("GO!!!"));
  buzzer.play("L4 V8 >a.");
  while(buzzer.isPlaying());
  display.clear();
}

void loop() {
  // Read lines acording to line color mode
  if (lineColor==0){
    position = lineSensors.readLineBlack(lineSensorValues);
  }else{
    position = lineSensors.readLineWhite(lineSensorValues);
  }

  // Calculate eror 
  error = position - 2000;

  // Modify parameters depending if its a straight or curved line
  // Straight
  if (abs(error) < 400){
    proportional = 20;
    baseSpeed = 260;
    derivative = 256; 
    maxSpeed = 380;
  // Curved
  }else{
    proportional = 32;
    baseSpeed = 200;
    derivative = 512;
    maxSpeed = 250;
    count = 0;
  }

  // PD control
  int16_t speedDifference = error * (int32_t)proportional / 256  + (error - lastError) * (int32_t)derivative / 256;

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
}
