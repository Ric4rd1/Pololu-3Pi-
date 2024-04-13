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

//Variables
int optionSelect = -1;
int nOptions = 4;
int left, right = 000;

//Funciones 

//Botones A, C para ciclar menu, boton B para seleccionar 
void buttonPress(){
  while(1){
    //Boton A
    if(buttonA.isPressed()){
      optionSelect = (optionSelect - 1 + nOptions) % nOptions;
      Serial.print(optionSelect);
      buttonA.waitForRelease();
      return;
    //Boton C
    }else if(buttonC.isPressed()){
      optionSelect = (optionSelect + 1) % nOptions;
      Serial.print(optionSelect);
      buttonC.waitForRelease();
      return;
    //Boton B
    }else if(buttonB.isPressed()){
      buttonB.waitForRelease();
      switch(optionSelect){
        case 0:
          forward();
          break;
        case 1:
          backward();
          break;
        case 2:
          spin();
          break;
        case 3:
          spinAnti();
          break;
      }
      return;
    }
  }
}

//Display al prender robot
void mainMenu(){
  display.clear();
  display.gotoXY(3,1);
  display.print("MENU");
  display.gotoXY(0,3);
  display.print("     B*    ");
  buttonB.waitForButton();
  optionSelect++;
}

//Menu 1 (FORWARD)
void menu1(){
  display.clear();
  display.gotoXY(1,1);
  display.print("FORWARD");
  display.gotoXY(0,3);
  display.print(" <-  B* -> ");
  buttonPress();
}

//Menu 2 (BACKWARD)
void menu2(){
  display.clear();
  display.gotoXY(1,1);
  display.print("BACKWARD");
  display.gotoXY(0,3);
  display.print(" <-  B* -> ");
  buttonPress();
}

//Menu 3 (SPIN CLOCKWISE)
void menu3(){
  display.clear();
  display.gotoXY(1, 1);
  display.print("SPIN");
  display.gotoXY(1, 2);
  display.print("CLOCKWISE");
  display.gotoXY(0,3);
  display.print(" <-  B* -> ");
  buttonPress();
}

//Menu 4 (SPIN ANIT-CLOCKWISE)
void menu4(){
  display.clear();
  display.gotoXY(1, 1);
  display.print("SPIN ANTI");
  display.gotoXY(1, 2);
  display.print("CLOCKWISE");
  display.gotoXY(0,3);
  display.print(" <-  B* -> ");
  buttonPress();
}

//Avanzar
void forward(){
  right = 000;
  display.clear();
  display.gotoXY(0,2);
  display.print(" +       - ");
  display.gotoXY(0,3);
  display.print(" A       C ");
  while(1){
    display.gotoXY(4,0);
    display.print(right);
    motors.setSpeeds(right, right);
    if(buttonA.isPressed()){
        if(right < 400)
        right = right + 20;
        buttonA.waitForRelease();
      }else if(buttonC.isPressed()){
        if(right > 0)
        right = right - 20;
        buttonC.waitForRelease();
      }else if(buttonB.isPressed()){
        motors.setSpeeds(0,0);
        break;
      }

  }
  buttonB.waitForRelease();
}

//Retroceder
void backward(){
  right = 000;
  int negRight;
  display.clear();
  display.gotoXY(0,2);
  display.print(" +       - ");
  display.gotoXY(0,3);
  display.print(" A       C ");
  while(1){
    display.gotoXY(4,0);
    display.print(right);
    negRight = (right)*(-1);
    motors.setSpeeds(negRight, negRight);
    if(buttonA.isPressed()){
        if(right < 400)
        right = right + 20;
        buttonA.waitForRelease();
      }else if(buttonC.isPressed()){
        if(right > 0)
        right = right - 20;
        buttonC.waitForRelease();
      }else if(buttonB.isPressed()){
        motors.setSpeeds(0,0);
        break;
      }

  }
  buttonB.waitForRelease();
}

//vuelta +
void spin(){
  right = 000;
  display.clear();
  display.gotoXY(0,2);
  display.print(" +       - ");
  display.gotoXY(0,3);
  display.print(" A       C ");
  while(1){
    display.gotoXY(4,0);
    display.print(right);
    left = (right)*(-1);
    motors.setSpeeds(right, left);
    if(buttonA.isPressed()){
        if(right < 400)
        right = right + 20;
        buttonA.waitForRelease();
      }else if(buttonC.isPressed()){
        if(right > 0)
        right = right - 20;
        buttonC.waitForRelease();
      }else if(buttonB.isPressed()){
        motors.setSpeeds(0,0);
        break;
      }

  }
  buttonB.waitForRelease();
}

//vuelta -
void spinAnti(){
  right = 000;
  int negRight;
  display.clear();
  display.gotoXY(0,2);
  display.print(" +       - ");
  display.gotoXY(0,3);
  display.print(" A       C ");
  while(1){
    display.gotoXY(4,0);
    display.print(right);
    left = right;
    motors.setSpeeds(right*-1, left);
    if(buttonA.isPressed()){
        if(right < 400)
        right = right + 20;
        buttonA.waitForRelease();
      }else if(buttonC.isPressed()){
        if(right > 0)
        right = right - 20;
        buttonC.waitForRelease();
      }else if(buttonB.isPressed()){
        motors.setSpeeds(0,0);
        break;
      }

  }
  buttonB.waitForRelease();
}

void setup() {
  //Definir tamaño del display OLED
  display.setLayout11x4();
  Serial.begin(9600);
}

void loop() {
  switch (optionSelect) {
    case 0:
      // Acción para la opción 1
      menu1();
      break;
    case 1:
      // Acción para la opción 2
      menu2();
      break;
    case 2:
      // Acción para la opción 3
      menu3();     
      break;
    case 3:
      // Acción para la opción 4
      menu4();
      break;
    default:
      mainMenu();
      break;
  }
    
}
