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

//#####Global Varialbes#######
//slected menu
enum Menus{startupMenu, movementRoutineMenu, modeMenu, setupMenu, multiInstructionMenu};
Menus menuSelect = startupMenu;
int menuOptions = 4;
//selected option
int optionSelect = 0;
int numOptions = 0; 

//flags
bool flag = false;
bool multimode = false;
int temp = 0;
float tempFloat = 0;

int routineInstructions = 2;
int routineNumber = 0;
//Encoder variables
const float rad = 1.6; //radius of the wheel
const float len = 9.0; //distance between wheels

float period = 0.05; //periodo de muestreo
double prev_time = 0;

float distance = 0;
float rot = 0;
float speed = 0;

float error = 0;
float pos_left = 0;
float pos_right = 0;


float dis_ref[10] = {100, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, };
float rot_ref[10] = {180, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, };
float min_speed[10] = {30, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, };
float max_speed[10] = {150, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, };
float k_dis[10] = {5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, };
float k_rot[10] = {1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, };

//Encoder Functions 
float pulse2CmLeft(){
  float pos;
  pos = float(encoders.getCountsAndResetLeft());
  pos = pos * (1.0/12.0); // pulsos encoder a revMotor
  pos = pos * (1.0/29.86); //revMotor a revRueda
  pos = pos * ((3.2*3.1416)/1.0); //revRueda a cm
  return pos;
}

float pulse2CmRight(){
  float pos;
  pos = float(encoders.getCountsAndResetRight());
  pos = pos * (1.0/12.0); // pulsos encoder a revMotor
  pos = pos * (1.0/29.86); //revMotor a revRueda
  pos = pos * ((3.2*3.1416)/1.0); //revRueda a cm
  return pos;
}

float pulse2DegLeft(){
  float pos;
  pos = float(encoders.getCountsAndResetLeft());
  pos = pos * (1.0/12.0); // pulsos encoder a revMotor
  pos = pos * (1.0/29.86); //revMotor a revRueda
  pos = pos*(360.0/1.0); //revRueda a grados
  return pos;
}

float pulse2DegRight(){
  float pos;
  pos = float(encoders.getCountsAndResetRight());
  pos = pos * (1.0/12.0); // pulsos encoder a revMotor
  pos = pos * (1.0/29.86); //revMotor a revRueda
  pos = pos*(360.0/1.0); //revRueda a grados
  return pos;
}

void forward(int i){
  bumpSensors.read();
  while(1){
    if(millis()-prev_time >= period*1000){
      
      bumpSensors.read();
      if (bumpSensors.rightChanged()||bumpSensors.leftChanged()){
        motors.setSpeeds(0,0);
        break;
      }
      //Obtenemos posicion en cm
      pos_left = pulse2CmLeft();
      pos_right = pulse2CmRight();
      //display.print("pos_left: ");
      //display.print(pos_left);
      //delay(5000);

      //######### FORWARD ##############
      //Calculamos la distancia recorrida
      distance = distance +((pos_left+pos_right)/2);
      //calculamos el error
      error = dis_ref[i] - distance;
      //calculamos la velocidad (Control)
      speed = k_dis[i]*error;
      if(speed>max_speed[i]){
        speed = max_speed[i];
      } else if(speed<min_speed[i]){
        speed = min_speed[i];
      }

      if (error > 1){
        motors.setSpeeds(speed, speed);
      }else{
        motors.setSpeeds(0,0);
        //delay(200);
        break;
      }

    } //if
  } //while
}

void rotate(int i){
  bumpSensors.read();
  while(1){
    if(millis()-prev_time >= period*1000){
      
      bumpSensors.read();
      if (bumpSensors.rightChanged()||bumpSensors.leftChanged()){
        motors.setSpeeds(0,0);
        break;
      }
        
      //Obtenemos posicion en cm
      pos_left = pulse2DegLeft();
      pos_right = pulse2DegRight();
      
      //######### ROTATE ##############
      //calcular el angulo de giro
      rot = rot + (rad * ((pos_right - pos_left)/len));
      //calculamos el error
      error = (rot_ref[i]-7) - rot;
      //calculamos la velocidad (Control)
      speed = k_rot[i]*error;
      if(speed>max_speed[i]){
        speed = max_speed[i];
      } else if(speed<min_speed[i]){
        speed = min_speed[i];
      }

      if (error > 1){
        motors.setSpeeds(-speed, speed);
      }else{
        motors.setSpeeds(0,0);
        Serial.print("llegue aqui");
        Serial.print("   error: ");
        Serial.print(error);
        break;
      }
    
    }//if
  }//while
}

void clearMemory(){
  memset(dis_ref, 0, sizeof(dis_ref));
  memset(rot_ref, 0, sizeof(rot_ref));
  memset(min_speed, 0, sizeof(min_speed));
  memset(max_speed, 0, sizeof(max_speed));
  memset(k_dis, 0, sizeof(k_dis));
  memset(k_rot, 0, sizeof(k_rot));
}

void resetEncoder(){
  prev_time = 0;
  distance = 0;
  rot = 0;
  speed = 0;
  error = 0;
  pos_left = 0;
  pos_right = 0;
}

//Buttons A, C to cycle menu, button B to select function
void buttonPress(){
  while(1){
    //Boton A
    if(buttonA.isPressed()){
      //restar 1 a la opción seleccionada
      optionSelect = (optionSelect - 1 + numOptions) % numOptions;
      Serial.print(optionSelect);
      //esperar a soltar botón
      buttonA.waitForRelease();
      return;
    //Boton C
    }else if(buttonC.isPressed()){
      //sumar 1 a la opción selecionada
      optionSelect = (optionSelect + 1) % numOptions;
      Serial.print(optionSelect);
      //esperar a soltar botón
      buttonC.waitForRelease();
      return;
    //Boton B
    }else if(buttonB.isPressed()){
      buttonB.waitForRelease();
      //seleccionar función

      //Menu Movement routine
      if(menuSelect == movementRoutineMenu){
        switch(optionSelect){
        case 0:
          
          countDown();
          if(multimode == false){
            resetEncoder();
            forward(0);
            delay(100);
            resetEncoder();
            rotate(0);
            delay(100);
          }else if(multimode == true){
            for(int i = 0; i <= routineInstructions; i++){
              resetEncoder();
              forward(i);
              delay(100);
              resetEncoder();
              rotate(i);
              delay(100);
            }
          }

          break;
        case 1:
          optionSelect = 0;
          menuSelect = modeMenu;
          break;
        case 2:
          optionSelect = 0;
          routineNumber = 0;
          menuSelect = setupMenu;
          break;
        }
        return;
      }
      
      //Menu Mode
      if(menuSelect == modeMenu){
        switch(optionSelect){
        case 0:
          optionSelect = 0;
          routineInstructions =  1;
          routineNumber = 0;
          multimode = false;
          menuSelect = movementRoutineMenu;
          break;
        case 1:
          optionSelect = 0;
          multimode = true;
          routineNumber = 0;
          menuSelect = multiInstructionMenu;
          break;
        }
        return;
      }

      //Menu Setup
      if(menuSelect == setupMenu){
        switch(optionSelect){
        case 0:
          //Distance
          tempFloat = 0;

          flag = false;
          temp = 2;
          display.gotoXY(15, 2);
          display.print(">0.00");
          while(flag == false){
            flag = setVal2(20, 2);
          }

          dis_ref[routineNumber] = tempFloat;
          Serial.print("  vlaor ingresado:  ");
          Serial.print(dis_ref[routineNumber]);

          break;
        //########################################
        case 1:
          //rotation

          tempFloat = 0;

          flag = false;
          temp = 2;
          display.gotoXY(15, 3);
          display.print(">0.00");
          while(flag == false){
            flag = setVal2(45, 3);
          }

          rot_ref[routineNumber] = tempFloat;
          Serial.print("  vlaor ingresado:  ");
          Serial.print(rot_ref[routineNumber]);
          
          break;
        //##########################################
        case 2:
          //max speed
          tempFloat = 0;

          flag = false;
          temp = 2;
          display.gotoXY(15, 4);
          display.print(">0.00");
          while(flag == false){
            flag = setVal2(20,4);
          }

          max_speed[routineNumber] = tempFloat;
          Serial.print("  vlaor ingresado:  ");
          Serial.print(max_speed[routineNumber]);  
          break;
        //##########################################
        case 3:
          //min speed
          tempFloat = 0;

          flag = false;
          temp = 2;
          display.gotoXY(15, 5);
          display.print(">0.00");
          while(flag == false){
            flag = setVal2(20, 5);
          }

          min_speed[routineNumber] = tempFloat;
          Serial.print("  vlaor ingresado:  ");
          Serial.print(min_speed[routineNumber]);
          break;
        //#############################################
        case 4:
          //k distance
          tempFloat = 0;

          flag = false;
          temp = 2;
          display.gotoXY(15, 6);
          display.print(">0.00");
          while(flag == false){
            flag = setVal2(1, 6);
          }

          k_dis[routineNumber] = tempFloat;
          Serial.print("  vlaor ingresado:  ");
          Serial.print(k_dis[routineNumber]); 
          break;
        //##############################################
        case 5: 
          //k rotation
          tempFloat = 0;

          flag = false;
          temp = 2;
          display.gotoXY(15, 7);
          display.print(">0.00");
          while(flag == false){
            flag = setVal2(1, 7);
          }

          k_rot[routineNumber] = tempFloat;
          Serial.print("  vlaor ingresado:  ");
          Serial.print(k_rot[routineNumber]);  
          break;
        //##############################################
        case 6: 
          //NEXT
          routineNumber++;
          if (routineNumber >=routineInstructions){
            menuSelect = movementRoutineMenu;
          } 
          optionSelect = 0;
        }
        return;
      }


    return;
    }
  
  }
  
  optionSelect = 0;
}

void countDown(){
  display.clear();
  display.gotoXY(9, 2);
  display.print("3");
  delay(1000);
  display.gotoXY(9, 2);
  display.print("2");
  delay(1000);
  display.gotoXY(9, 2);
  display.print("1");
  delay(1000);
  display.clear();
}

//Select a value from 2-10 and assign it to temp, returns true when pressing B
bool setVal() {
  if (buttonA.isPressed()) {
    if (temp < 10) {
      temp = temp + 1;
      display.gotoXY(9, 4);
      display.print(String(temp) + " ");
      buttonA.waitForRelease();
    }
    return false;
  } else if (buttonC.isPressed()) {
    if (temp > 2) {
      temp = temp - 1;
      display.gotoXY(9, 4);
      display.print(String(temp) + " ");
      buttonC.waitForRelease();
    }
    return false;
  } else if (buttonB.isPressed()) {
    // motors.setSpeeds(0,0);
    buttonB.waitForRelease();
    return true;
  }
  return false;
}

//Select a value from 2-10 and assign it to temp, returns true when pressing B
bool setVal2(int i, int posY) {
  switch (i) {
    case 20:
      if(buttonA.isPressed()){
            if(tempFloat < 400)
            tempFloat = tempFloat + 20;
            display.gotoXY(17, posY);
            display.print(String(tempFloat)+" ");
            buttonA.waitForRelease();
            return false;
          }else if(buttonC.isPressed()){
            if(tempFloat > 0)
            tempFloat = tempFloat - 20;
            display.gotoXY(17, posY);
            display.print(String(tempFloat)+" ");
            buttonC.waitForRelease();
            return false;
          }else if(buttonB.isPressed()){
            //motors.setSpeeds(0,0);
            buttonB.waitForRelease();
            return true;
          }
      break;
    //######################################
    case 45:
      if(buttonA.isPressed()){
          if(tempFloat < 360)
          tempFloat = tempFloat + 45;
          display.gotoXY(17, posY);
          display.print(String(tempFloat)+" ");
          buttonA.waitForRelease();
          return false;
        }else if(buttonC.isPressed()){
          if(tempFloat > 0)
          tempFloat = tempFloat - 45;
          display.gotoXY(17, posY);
          display.print(String(tempFloat)+" ");
          buttonC.waitForRelease();
          return false;
        }else if(buttonB.isPressed()){
          //motors.setSpeeds(0,0);
          buttonB.waitForRelease();
          return true;
        }
      break;
    //########################################
    case 1:
      if(buttonA.isPressed()){
          if(tempFloat < 20)
          tempFloat = tempFloat + 1;
          display.gotoXY(17, posY);
          display.print(String(temp)+" ");
          buttonA.waitForRelease();
          return false;
        }else if(buttonC.isPressed()){
          if(tempFloat > 0)
          tempFloat = tempFloat - 1;
          display.gotoXY(17, posY);
          display.print(String(tempFloat)+" ");
          buttonC.waitForRelease();
          return false;
        }else if(buttonB.isPressed()){
          //motors.setSpeeds(0,0);
          buttonB.waitForRelease();
          return true;
        }
      break;
  }
  display.gotoXY(16, posY);
  display.print(" ");
  return false;
}

//Startup menu
void mainMenu(){
  display.clear();
  display.gotoXY(9,2);
  display.print("MENU");
  display.gotoXY(2, 3);
  display.print("Movement Routines");
  display.gotoXY(10,6);
  display.print("B*");
  buttonB.waitForButton();
  menuSelect = movementRoutineMenu;
}
//Menu movement routine
void menu1(){
  //menu variables
  numOptions = 3;

  display.clear();
  display.gotoXY(2,0);
  display.print("Movement Rountine");
  display.gotoXY(8,1);
  display.print("MENU");

  display.gotoXY(0,3);
  if(optionSelect == 0){
    display.print("- Run <--");    
  }else{
    display.print("- Run");    
  }
  
  display.gotoXY(0,4);
  if(optionSelect == 1){
    display.print("- Mode <--");    
  }else{
    display.print("- Mode");    
  }
  
  display.gotoXY(0,5);
  if(optionSelect == 2){
    display.print("- Setup <--");    
  }else{
    display.print("- Setup");    
  }

  buttonPress();
}

//Mode menu, single instruction or multi instruction
void menu2(){
  //menu variables
  numOptions = 2;

  display.clear();
  display.gotoXY(8,0);
  display.print("Mode");
  display.gotoXY(0,2);
  display.print("- 1 Instruction");
  display.gotoXY(0,3);
  if(optionSelect == 0){
    display.print("  Routine <--");
  }else{
    display.print("  Routine");
  }
  

  display.gotoXY(0,5);
  display.print("- Multi-Instruction");
  display.gotoXY(0,6);
  if(optionSelect == 1){
    display.print("  Routine <--");
  }else{
    display.print("  Routine");
  }

  buttonPress();
}

//Multi-Instruction routine, select how many instructions
void menu3(){
  display.clear();
  display.gotoXY(2,0);
  display.print("Multi-Instruction");
  display.gotoXY(1,1);
  display.print("2 to 10 instructions");
  display.gotoXY(0,6);
  display.print(" +      Sel       -");
  display.gotoXY(0,7);
  display.print("A*      B*       C*");
  display.gotoXY(9, 4);
  display.print("2");
  
  flag = false;
  temp = 2;
  while(flag == false){
    flag = setVal();
  }
  routineInstructions = temp;
  menuSelect = movementRoutineMenu;

}

//Setup menu, give parameters
void menu4(){
  //menu variables
  numOptions = 7;

  display.clear();
  display.gotoXY(7,0);
  display.print("Setup     NEXT");
  display.gotoXY(0, 2);
  display.gotoXY(0, 1);
  display.print("Instruction: ");
  display.print(routineNumber+1);

  display.gotoXY(0, 2);
  if(optionSelect == 0){
    display.print("->Distance ");
  }else{
    display.print("- Distance ");
  }
  display.gotoXY(16, 2);
  display.print(dis_ref[routineNumber]);

  display.gotoXY(0, 3);
  if(optionSelect == 1){
    display.print("->Rotation ");
  }else{
    display.print("- Rotation ");
  }
  display.gotoXY(16, 3);
  display.print(rot_ref[routineNumber]);

  display.gotoXY(0, 4);
  if(optionSelect == 2){
    display.print("->Max Speed ");
  }else{
    display.print("- Max Speed ");
  }
  display.gotoXY(16, 4);
  display.print(max_speed[routineNumber]);

  display.gotoXY(0, 5);
  if(optionSelect == 3){
    display.print("->Min Speed ");
  }else{
    display.print("- Min Speed ");
  }
  display.gotoXY(16, 5);
  display.print(min_speed[routineNumber]);

  display.gotoXY(0, 6);
  if(optionSelect == 4){
    display.print("->K Distance ");
  }else{
    display.print("- K Distance ");
  }
  display.gotoXY(16, 6);
  display.print(k_dis[routineNumber]);

  display.gotoXY(0, 7);
  if(optionSelect == 5){
    display.print("->K Rotation ");
  }else{
    display.print("- K Rotation ");
  }
  display.gotoXY(16, 7);
  display.print(k_rot[routineNumber]);

  display.gotoXY(16, 0);
  if(optionSelect == 6){
    display.print(">NEXT");
  }else{
    display.print(" NEXT");
  }

  buttonPress();  
  

}


void setup() {
  //Definir tamaño del display OLED
  display.setLayout21x8();
  Serial.begin(9600);
  bumpSensors.calibrate();
}
void loop() {
  switch (menuSelect) {
    case movementRoutineMenu:
      menu1();
      break;
    case modeMenu:
      menu2();
      break;
    case multiInstructionMenu:
      menu3();
      break;
    case setupMenu:
        menu4();
      break;
    case startupMenu:
      mainMenu();
      break;
    
  } 
}
