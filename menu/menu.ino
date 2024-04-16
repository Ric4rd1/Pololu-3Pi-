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
//opcion seleccionada
int optionSelect = -1;
//número de opciones
int nOptions = 4;
//valores velocidad y distancia
int temp, vel, dis = 0;
//bandera
bool flag = false;
//funciones de los motores
enum Function {encForward, encBackward, encClockWise, encAntiClockWise};

//Funciones 

//Botones A, C para ciclar menu, boton B para seleccionar función
void buttonPress(){
  while(1){
    //Boton A
    if(buttonA.isPressed()){
      //restar 1 a la opción seleccionada
      optionSelect = (optionSelect - 1 + nOptions) % nOptions;
      Serial.print(optionSelect);
      //esperar a soltar botón
      buttonA.waitForRelease();
      return;
    //Boton C
    }else if(buttonC.isPressed()){
      //sumar 1 a la opción selecionada
      optionSelect = (optionSelect + 1) % nOptions;
      Serial.print(optionSelect);
      //esperar a soltar botón
      buttonC.waitForRelease();
      return;
    //Boton B
    }else if(buttonB.isPressed()){
      buttonB.waitForRelease();
      //seleccionar función
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

//Seleccionar un valor de 0-400 y asignarlo a temp, regresa true al presionar B 
bool setVal(){
  if(buttonA.isPressed()){
        if(temp < 400)
        temp = temp + 20;
        display.gotoXY(4, 1);
        display.print(String(temp)+" ");
        buttonA.waitForRelease();
        return false;
      }else if(buttonC.isPressed()){
        if(temp > 0)
        temp = temp - 20;
        display.gotoXY(4, 1);
        display.print(String(temp)+" ");
        buttonC.waitForRelease();
        return false;
      }else if(buttonB.isPressed()){
        //motors.setSpeeds(0,0);
        buttonB.waitForRelease();
        return true;
      }
}

//cuenta regresiva 3,2,1
void countDown(){
  display.clear();
  display.gotoXY(5, 1);
  display.print("3");
  delay(1000);
  display.gotoXY(5, 1);
  display.print("2");
  delay(1000);
  display.gotoXY(5, 1);
  display.print("1");
  delay(1000);
}

//Inicia los motores dependiendo de function a la velocidad y distancia en los parametros
void encoderTrack(Function function, int distance, int velocity){
  //Encoder Variables
  float pos_left = 0;//almacenamos la posición de los encoders derecha e izquierda
  float pos_right = 0;
  float pos_avg = 0;//promedio
  float vel_left = 0;//almacenamos la posición de los encoders derecha e izquierda
  float vel_right = 0;
  float vel_avg = 0;
  float period = 0.05;//periodo de muestreo
  double prev_time = 0;//almacena el tiempo anterior de la ejecución del ciclo.
  
  //Selecionar funcion
  switch(function){
    case encForward:
      motors.setSpeeds(velocity, velocity);//Izq y Der positivos
      break;
    case encBackward:
      motors.setSpeeds(-1*velocity, -1*velocity);//Izq y Der negativos
      break;
    case encClockWise:
      motors.setSpeeds(velocity, -1*velocity);//Izq positivo, Der negativo
      break;
    case encAntiClockWise:
      motors.setSpeeds(-1*velocity, velocity);//Izq negativo, Der positivo
      break;
    default:
      break;
  }

  while(1){
    //Ejecutar nuestra rutina cada periodo
    if(millis()-prev_time >= period*1000){
    //Actualizamos el tiempo anterior del ciclo
    prev_time = millis();

    pos_left = float(encoders.getCountsAndResetLeft());//obtener pulsos encoder y reiniciar cuenta
    pos_left = pos_left * (1.0/12.0); // pulsos encoder a revMotor
    pos_left = pos_left * (1.0/29.86); //revMotor a revRueda
    pos_left = pos_left * ((3.2*3.1416)/1.0); //revRueda a cm

    vel_left = pos_left / period;
    vel_left = abs(vel_left);
    pos_left = abs(pos_left);

    pos_right = float(encoders.getCountsAndResetRight());//obtener pulsos encoder y reiniciar cuenta
    pos_right = pos_right * (1.0/12.0); // pulsos encoder a revMotor
    pos_right = pos_right * (1.0/29.86); //revMotor a revRueda
    pos_right = pos_right * ((3.2*3.1416)/1.0); //revRueda a cm

    vel_right = pos_right / period;
    vel_right = abs(vel_right);
    pos_right = abs(pos_right);

    //Calcular promedio velocidad dos llantas
    vel_avg = (vel_right+vel_left)/2.0;
    //Calcular promedio posición dos llantas
    pos_avg = pos_avg+((pos_left+pos_right)/2.0);
    
    }
    
    display.gotoXY(5,2);
    display.print(vel_avg);
    display.gotoXY(4,3);
    display.print(pos_avg);

    //esperar a que pos llegue a distance
    if(pos_avg >= distance || buttonB.isPressed()){
      motors.setSpeeds(0,0);
      buttonB.waitForRelease();
      break;
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

//Display seleccionar distancia
void menuDistance(){
  flag = false;
  display.clear();
  display.gotoXY(1,0);
  display.print("DISTANCE");
  display.gotoXY(0,2);
  display.print(" +       - ");
  display.gotoXY(0,3);
  display.print(" A       C ");
  while(flag == false){
    flag = setVal();
  }
  
}

//Display seleccionar velocidad
void menuVelocity(){
  flag = false;
  display.clear();
  display.gotoXY(1,0);
  display.print("VELOCITY");
  display.gotoXY(0,2);
  display.print(" +       - ");
  display.gotoXY(0,3);
  display.print(" A       C ");
  while(flag == false){
    flag = setVal();
  }
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
  flag = false;
  temp = 0;
  dis = 0;
  vel = 0;
  //preguntar distancia
  menuDistance();
  while(1){
    flag = setVal();
    if(flag == true)
      flag = false;
      break;
  }
  dis = temp;
  temp = 0;
  //preguntar velocidad
  menuVelocity();
  while(1){
    flag = setVal();
    if(flag == true)
      flag = false;
      break;
  }
  vel = temp;
  countDown();

  display.clear();
  display.gotoXY(1,0);
  display.print(" FORWARD ");
  display.gotoXY(0,2);
  display.print("Vel: ");
  display.gotoXY(0, 3);
  display.print("Cm: ");
  
  //Definir función
  Function function = encForward;
  //Activar Motores y empezar seguimiento con encoders   
  encoderTrack(encForward, dis, vel);
  delay(1000);
  
}

//Retroceder
void backward(){
  flag = false;
  temp = 0;
  dis = 0;
  vel = 0;
  //preguntar distancia
  menuDistance();
  while(1){
    flag = setVal();
    if(flag == true)
      flag = false;
      break;
  }
  dis = temp;
  temp = 0;
  //preguntar velocidad
  menuVelocity();
  while(1){
    flag = setVal();
    if(flag == true)
      flag = false;
      break;
  }
  vel = temp;
  countDown();

  display.clear();
  display.gotoXY(1,0);
  display.print("BACKWARD ");
  display.gotoXY(0,2);
  display.print("Vel: ");
  display.gotoXY(0, 3);
  display.print("Cm: ");

  //Definir función
  Function function = encBackward;
  //Activar Motores y empezar seguimiento con encoders
  encoderTrack(encBackward, dis, vel);
  delay(1000);
  
}

//vuelta clockwise
void spin(){
  flag = false;
  temp = 0;
  dis = 0;
  vel = 0;
  //preguntar distancia
  menuDistance();
  while(1){
    flag = setVal();
    if(flag == true)
      flag = false;
      break;
  }
  dis = temp;
  temp = 0;
  //preguntar velocidad
  menuVelocity();
  while(1){
    flag = setVal();
    if(flag == true)
      flag = false;
      break;
  }
  vel = temp;
  countDown();

  display.clear();
  display.gotoXY(1,0);
  display.print("SPIN CLOCK ");
  display.gotoXY(0,2);
  display.print("Vel: ");
  display.gotoXY(0, 3);
  display.print("Cm: ");

  //definir función
  Function function = encClockWise;
  //Activar Motores y empezar seguimiento con encoders
  encoderTrack(encClockWise, dis, vel);
  delay(1000);
}

//vuelta anticlockwise
void spinAnti(){
  flag = false;
  temp = 0;
  dis = 0;
  vel = 0;
  //preguntar distancia
  menuDistance();
  while(1){
    flag = setVal();
    if(flag == true)
      flag = false;
      break;
  }
  dis = temp;
  temp = 0;
  //preguntar velocidad
  menuVelocity();
  while(1){
    flag = setVal();
    if(flag == true)
      flag = false;
      break;
  }
  vel = temp;
  countDown();

  display.clear();
  display.gotoXY(1,0);
  display.print("SPIN ANTI-");
  display.gotoXY(1, 1);
  display.print("CLOCK");
  display.gotoXY(0,2);
  display.print("Vel: ");
  display.gotoXY(0, 3);
  display.print("Cm: ");

  Function function = encAntiClockWise;
  //Activar Motores y empezar seguimiento con encoders
  encoderTrack(encAntiClockWise, dis, vel);
  delay(1000);
}

void setup() {
  //Definir tamaño del display OLED
  display.setLayout11x4();
  //Serial.begin(9600);
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
