#include <Wire.h>
#include <VL53L0X.h>

// Definir pines para los motores
#define PWM1A 0
#define PWM1B 0
#define PWM2A 0
#define PWM2B 0
// Definir objetos para sensores de distancia
VL53L0X S1;
VL53L0X S2;
VL53L0X S3;

int distanciaIzq;
int distanciaDer;
int distanciaFrente;
int error;
bool flagDoblar = false;

void setup()
{
  Wire.begin();
  //Setup de Motores
  pinMode(PWM1A, OUTPUT);
  pinMode(PWM1B, OUTPUT);
  pinMode(PWM2A, OUTPUT);
  pinMode(PWM2B, OUTPUT);
  //Setup de sensores de distancia en direcciones I2C
  S1.init();
  S1.setAddress(0x30);
  S2.init();
  S2.setAddress(0x31);
  S3.init();
  S3.setAddress(0x32);
}

void loop()
{
  //Lee los valores de cada sensores y los guarda en su variable correspondiente
  distanciaFrente = S1.readRangeSingleMillimeters();
  distanciaIzq = S2.readRangeSingleMillimeters();
  distanciaDer = S3.readRangeSingleMillimeters();
  //calcula el error
  error = distanciaIzq - distanciaDer;
  //Checkeo de giros
  doblar();
  //Se realiza la calibración
  calibration();
  //Control de los motores
  controlMotors();
}

void doblar() //Funcion para doblar siempre que se pueda hacia la izquierda
{
  error = distanciaIzq - distanciaDer;
  if (error >= 18)
  {
    flagDoblar = true;
  }
}

void calibration() //Funcion para calibrar los motores en caso de estar descentrado
{
  if (flagDoblar == false)
  {
    do
    {
      analogWrite(PWM1A, 100);
      analogWrite(PWM2A, 80);
      distanciaIzq = S2.readRangeSingleMillimeters();
      distanciaDer = S3.readRangeSingleMillimeters();
      error = distanciaIzq - distanciaDer;
    }
    while (error < -50) //Está más cerca de la pared izquierda
    analogWrite(PWM1A, 100);
    analogWrite(PWM2A, 100);
    do
    {
      analogWrite(PWM1A, 80);
      analogWrite(PWM2A, 100);
      distanciaIzq = S2.readRangeSingleMillimeters();
      distanciaDer = S3.readRangeSingleMillimeters();
      error = distanciaIzq - distanciaDer;
    }
    while(error > 50) //Está más cerca de la pared derecha
    analogWrite(PWM1A, 100);
    analogWrite(PWM2A, 100);
  }
}

void controlMotors() //funcion para el control de los motores
{
  if (flagDoblar == true)
  {
    analogWrite(PWM1B, 80);
    analogWrite(PWM2A, 80);
    delay(1000);
    analogWrite(PWM1A, 100);
    analogWrite(PWM2A, 100);
    delay(500);
    analogWrite(PWM1B, 80);
    analogWrite(PWM2A, 80);
    delay(1000);
    flagDoblar = false;
  }
  else if (flagDoblar == false)
  {
    analogWrite(PWM1A, 100);
    analogWrite(PWM2A, 100);
  }
}
