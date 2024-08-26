//CODIGO PARA LEER LA ENTRADA ADC Y MANDAR SEÑALES PWM A LOS LEDS

#include <Arduino.h> //Librería de Arduino
#include <driver/adc.h> //Librería para controlar el ADC
#include "driver/ledc.h" //Librería para controlar las señales PWM

//Pines para las entradas
#define Boton1 32
#define SensorTemp 33
#define PinReferencia 23

//Pinea para las salidas
#define PinVerde 15 
#define PinAmarillo 2
#define PinRojo 4

//Definir canal, frecuencia y resolución para las señales pwm de los LEDS
#define CanalLedVerde 1
#define CanalLedAmarillo 2
#define CanalLedRojo 3
#define freqLeds 50
#define resolucinLeds 10

//Variables para controlar el debounce del botón
unsigned long t_Boton = 0;
unsigned long ultimo_t_Boton = 0;

//Variables globales
int Temperatura = 0;
bool Encender = false;

//Función para la interrupción del botón
void IRAM_ATTR Prueba() {
  t_Boton = millis();
  if (t_Boton - ultimo_t_Boton > 250) {
    ultimo_t_Boton = t_Boton;
    Encender = true;
    Temperatura = analogReadMilliVolts(SensorTemp);
  }
}

//Definir función para encender los LEDs
void EncenderLeds (void);
//Definir función para iniciar las señales PWM
void initPWMLeds(void);

void setup() {
  // Configurar pines como entradas
  pinMode(Boton1, INPUT_PULLUP);

  //Configurar pines de salida
  pinMode(PinVerde, OUTPUT);
  pinMode(PinAmarillo, OUTPUT);
  pinMode(PinRojo, OUTPUT);

  //Comunicación serial
  Serial.begin(115200);

  // Interupcion para medir la temperatura
  attachInterrupt(Boton1, Prueba, FALLING);

  //Iniciar los pines para señales PWM
  initPWMLeds();

  //Configuración de la entrada analógica
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  //Configurar pines de ADC
  analogSetVRefPin(PinReferencia);
  pinMode(SensorTemp, INPUT);
}

void loop() {
  //Enciende los LEDs
  if (Encender) {
    EncenderLeds();
    Encender = false;
  }
  delay(10);
}

//Función para encender los LEDs
void EncenderLeds (void) {
  if (Temperatura <= 370) {
    ledcWrite(CanalLedVerde, 512);
    ledcWrite(CanalLedAmarillo, 0);
    ledcWrite(CanalLedRojo, 0);
  } else if (Temperatura > 370 && Temperatura <= 375) 
  {
    ledcWrite(CanalLedVerde, 0);
    ledcWrite(CanalLedAmarillo, 512);
    ledcWrite(CanalLedRojo, 0);
  } else if (Temperatura > 375) {
    ledcWrite(CanalLedVerde, 0);
    ledcWrite(CanalLedAmarillo, 0);
    ledcWrite(CanalLedRojo, 512);
  }
}

void initPWMLeds(void){
  // Paso 4.1:
  ledcSetup(CanalLedVerde, freqLeds, resolucinLeds);
  ledcSetup(CanalLedAmarillo, freqLeds, resolucinLeds);
  ledcSetup(CanalLedRojo, freqLeds, resolucinLeds);
  ledcSetup(CanalServo, freqLeds, resolucinLeds);
  // Paso 5: Especificar GPIO donde la señal saldrá
  ledcAttachPin(PinVerde, CanalLedVerde);
  ledcAttachPin(PinAmarillo, CanalLedAmarillo);
  ledcAttachPin(PinRojo, CanalLedRojo);
  ledcAttachPin(PinServo, CanalServo);
  // Paso 6: Establecer ciclo de trabajo
  ledcWrite(CanalLedVerde, 0);
  ledcWrite(CanalLedAmarillo, 0);
  ledcWrite(CanalLedRojo, 0);
  ledcWrite(CanalServo, 24);
}
