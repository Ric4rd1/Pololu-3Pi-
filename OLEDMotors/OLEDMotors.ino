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

void setup() {
  //Setup OLED display layout
  display.setLayout8x2();

}

void loop() {
  //Motors speeds -400 to 400
  if(buttonA.isPressed()){
    //set both motors speed forward
    motors.setSpeeds(200, 200);
  }else if (buttonC.isPressed()){
    //set both motors speed backward
    motors.setSpeeds(-200, -200);
  }else{
    //set both motors speed to stop 
    motors.setSpeeds(0, 0);
  }

  //Causes the clear(), write(), and print() functions to not perform any I/O or write any data to the OLED.
  display.noAutoDisplay();
  //Clears the text and resets the text cursor to the upper left
  display.clear();
  //Sets cursor to XY coordinates
  display.gotoXY(0, 0);
  //Prints text in cursor
  display.print("GOOD-BYE");
  display.gotoXY(0, 1);
  display.print("Ricard");
  display.display();
  }
