#include <Wire.h>
#include <VL53L0X.h>

// Definir pines para los motores
#define PWM1A 0
#define PWM1B 0
#define PWM2A 0
#define PWM2B 0
// Definir valores de velocidad
#define velocidadConstante 100 // Velocidad mayor para la calibracion y el giro de motores
#define velocidadCorreccion 80 // Velocidad menor para la calibracion y el giro de motores
// Definir objetos para sensores de distancia
VL53L0X S1;
VL53L0X S2;
VL53L0X S3;

int distanciaIzq;
int distanciaDer;
int distanciaFrente;
int error;
bool flagDoblar = false;
bool flagCentrado = false;

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
  // Calcula el error
  error = distanciaIzq - distanciaDer;
  // Checkeo de la calibracion del auto con respecto al error (si está centrado, digamos)
  checkCalibration();
  // Checkeo de giros
  doblar();
  // Se realiza la calibración
  calibration();
  // Control de los motores
  controlMotors();
}

void checkCalibration() //Función para el checkeo de la calibración
{
  if (error < -50 || error > 50) //Está descentrado
  {
    flagCentrado = true;
  }
}

void doblar() //Función para doblar siempre que se pueda hacia la izquierda
{
  if (error >= 180)
  {
    flagDoblar = true;
  }
}

void calibration() // Función para calibrar los motores en caso de estar descentrado
{
  if (flagDoblar == false && flagCentrado == false)
  {
    do
    {
      analogWrite(PWM1A, velocidadConstante);
      analogWrite(PWM2A, velocidadCorreccion);
      distanciaIzq = S2.readRangeSingleMillimeters();
      distanciaDer = S3.readRangeSingleMillimeters();
      error = distanciaIzq - distanciaDer;
    }
    while (error < -50) // Está más cerca de la pared izquierda

    do
    {
      analogWrite(PWM1A, velocidadCorreccion);
      analogWrite(PWM2A, velocidadConstante);
      distanciaIzq = S2.readRangeSingleMillimeters();
      distanciaDer = S3.readRangeSingleMillimeters();
      error = distanciaIzq - distanciaDer;
    }
    while(error > 50) // Está más cerca de la pared derecha
    flagCentrado = true;
    analogWrite(PWM1A, velocidadConstante);
    analogWrite(PWM2A, velocidadConstante);
  }
}

void controlMotors() // Función para el control de los motores
{
  if (flagDoblar == true)
  {
    analogWrite(PWM1B, velocidadCorreccion);
    analogWrite(PWM2A, velocidadCorreccion);
    delay(1000);
    analogWrite(PWM1A, velocidadConstante);
    analogWrite(PWM2A, velocidadConstante);
    delay(500);
    analogWrite(PWM1B, velocidadCorreccion);
    analogWrite(PWM2A, velocidadCorreccion);
    delay(1000);
    flagDoblar = false;
  }
  else if (flagDoblar == false)
  {
    analogWrite(PWM1A, velocidadConstante);
    analogWrite(PWM2A, velocidadConstante);
  }
}
