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
/*
//pulsos a revMotor
Pos = pulsos * (1rev/12pulsos)

//revMotor a revRueda
Pos = revMotor * (1 revRueda/29.86 revMotor)

//revRueda a cm
Pos = revRueda * ((diametroRueda3.2 * pi)cm/1 revRueda)
*/

float pos_left = 0;//almacenamos la posición de los encoders derecha e izquierda
float pos_right = 0;
float vel_left = 0;//almacenamos la velocidad de las  ruedas derecha e izquierda

float vel_right = 0;
float period = 0.05; // define el periodo de muestreo
double prev_time = 0; //almacena el tiempo anterior de la ejecución del ciclo.

void setup() {
  display.setLayout11x4();//inicializamos la pantalla para que se muestre la información

}

void loop() {
  //Esperamos a que haya transcurrida el periodo de muestra
  if(millis()-prev_time >= period*1000){
    //Actualizamos el tiempo anterior del ciclo
    prev_time = millis();

    //Ejecutamos nuestra rutina cada 50ms
    pos_left = float(encoders.getCountsAndResetLeft());
    pos_left = pos_left * (1.0/12.0); // pulsos encoder a revMotor
    pos_left = pos_left * (1.0/29.86); //revMotor a revRueda
    pos_left = pos_left * ((3.2*3.1416)/1.0); //revRueda a cm

    vel_left = pos_left / period;

    pos_right = float(encoders.getCountsAndResetRight());
    pos_right = pos_right * (1.0/12.0); // pulsos encoder a revMotor
    pos_right = pos_right * (1.0/29.86); //revMotor a revRueda
    pos_right = pos_right * ((3.2*3.1416)/1.0); //revRueda a cm

    vel_right = pos_right / period;
  }

  display.noAutoDisplay(); //despues de imprimir el texto la pantalla no se actualizara 
                          //hasta que reciba una instrucción manualmente.
  display.clear(); //limpia el contenido de la pantalla para poder agregar nueva información
  display.gotoXY(0,0); //mueve el cursor de texto en la posición(0,0)
  display.print("LEFT: ");//este texto nos srive para identificar la información
                          //de la velocidad izquierda

  display.print(vel_left); //imprime el valor de la variable 
  display.gotoXY(0, 2); //mueve el cursor a la posición (0,2)
  display.print("RIGHT: ");//indica que la información se refiere a la velocidad derecha
  display.print(vel_right); //imprime valor de la variable
  display.display(); //muestra los cambios 
}
