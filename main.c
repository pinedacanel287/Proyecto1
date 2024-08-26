//CODIGO PARA LEER LA ENTRADA ADC, MANDAR SEÑALES PWM A LOS LEDS, SERVO
//DISPLAY DE 7 SEGMENTOS Y ENVIAR LOS DATOS A ADAFRUIT
#include <Arduino.h> //Librería de Arduino
#include <driver/adc.h> //Librería para controlar el ADC
#include "driver/ledc.h" //Librería para controlar las señales PWM
#include "config.h" //Librería para Adafruit

//Configuración para el feed de Adafruit
AdafruitIO_Feed *tempCanal = io.feed("ValorTemperatura");

//Pines para las entradas
#define Boton1 32
#define SensorTemp 33
#define PinReferencia 23

//Pinea para las salidas
#define PinVerde 15 
#define PinAmarillo 2
#define PinRojo 4
#define PinServo 26
#define PinDigito1 27
#define PinDigito2 14
#define PinDigito3 12
#define PinSegmentoA 22
#define PinSegmentoB 21
#define PinSegmentoC 18
#define PinSegmentoD 17
#define PinSegmentoE 13
#define PinSegmentoF 19 
#define PinSegmentoG 25
#define PinSegmentoP 5

//Definir canal, frecuencia y resolución para las señales pwm de los LEDS
#define CanalLedVerde 1
#define CanalLedAmarillo 2
#define CanalLedRojo 3
#define CanalServo 4
#define freqLeds 50
#define resolucinLeds 10


//Variables para controlar el debounce del botón
unsigned long t_Boton = 0;
unsigned long ultimo_t_Boton = 0;

//Variables globales
int AnguloActualServo = 24, AnguloLlegadaServo = 24, Temperatura = 0, Centenas = 0, Decenas = 0, Unidades = 0, NumeroDisplay = 0;
float TemperaturaAdafruit = 0.0;
bool Encender = false;

//Función para la interrupción del botón
void IRAM_ATTR Prueba() {
  t_Boton = millis();
  if (t_Boton - ultimo_t_Boton > 250) {
    ultimo_t_Boton = t_Boton;
    Encender = true;
    Temperatura = analogReadMilliVolts(SensorTemp);
    TemperaturaAdafruit = Temperatura*0.1;
  }
}

//Definir función para encender los LEDs
void EncenderLeds (void);
//Definir función para iniciar las señales PWM
void initPWMLeds(void);
//Definir función para encender el display de 7 segmentos
void EncenderDisplay7(int NumeroDisplay);
//Definir función para enviar datos de Adafruit
void EnviarAdafruit (void);

void setup() {
  // Configurar pines como entradas
  pinMode(Boton1, INPUT_PULLUP);

  //Configurar pines de salida
  pinMode(PinVerde, OUTPUT);
  pinMode(PinAmarillo, OUTPUT);
  pinMode(PinRojo, OUTPUT);
  pinMode(PinServo, OUTPUT);
  pinMode(PinDigito1, OUTPUT);
  pinMode(PinDigito2, OUTPUT);
  pinMode(PinDigito3, OUTPUT);
  pinMode(PinSegmentoA, OUTPUT);
  pinMode(PinSegmentoB, OUTPUT);
  pinMode(PinSegmentoC, OUTPUT);
  pinMode(PinSegmentoD, OUTPUT);
  pinMode(PinSegmentoE, OUTPUT);
  pinMode(PinSegmentoF, OUTPUT);
  pinMode(PinSegmentoG, OUTPUT);
  pinMode(PinSegmentoP, OUTPUT);

  //Comunicación serial
  Serial.begin(115200);

  // Interupcion para medir la temperatura
  attachInterrupt(Boton1, Prueba, FALLING);

  //Configuración de la entrada analógica
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  //Iniciar los pines para señales PWM
  initPWMLeds();

  //Configurar pines de ADC
  analogSetVRefPin(PinReferencia);
  pinMode(SensorTemp, INPUT);

  //Espera para abrir el monitor serial
  while(! Serial);

  Serial.print("Connecting to Adafruit IO");

  //Conecxión a Adafruit
  io.connect();

  //Esperar para conectarse
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  //Mensaje para confirmar la conecxión
  Serial.println();
  Serial.println(io.statusText());

}

void loop() {
//Mantiene la conecxión con Adafruit
//Proceso de la señal ADC a valores de temperatura
  Centenas = Temperatura/100;
  Decenas = (Temperatura - (Centenas*100))/10;
  Unidades = (Temperatura - (Centenas*100) - (Decenas*10));
  AnguloLlegadaServo = map(Temperatura, 0, 1000, 24, 84);
//Enciende los LEDs
  if (Encender) {
    EncenderLeds();
    EnviarAdafruit();
    Encender = false;
  }
//Muestra los valores de temperatura en el Display de 7 segmentos
  NumeroDisplay = Centenas;
  digitalWrite(PinDigito1, HIGH);
  digitalWrite(PinDigito2, LOW);
  digitalWrite(PinDigito3, LOW);
  digitalWrite(PinSegmentoP, LOW);
  EncenderDisplay7(NumeroDisplay);
  delay(5);
  NumeroDisplay = Decenas;
  digitalWrite(PinDigito1, LOW);
  digitalWrite(PinDigito2, HIGH);
  digitalWrite(PinDigito3, LOW);
  digitalWrite(PinSegmentoP, HIGH);
  EncenderDisplay7(NumeroDisplay);
  delay(5);
  NumeroDisplay = Unidades;
  digitalWrite(PinDigito1, LOW);
  digitalWrite(PinDigito2, LOW);
  digitalWrite(PinDigito3, HIGH);
  digitalWrite(PinSegmentoP, LOW);
  EncenderDisplay7(NumeroDisplay);
  delay(5);
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
  if (AnguloActualServo < AnguloLlegadaServo) {
    for (int i = AnguloActualServo; i <= AnguloLlegadaServo; i++) {
      ledcWrite(CanalServo, i);
    }
    AnguloActualServo = AnguloLlegadaServo;
  }
  if (AnguloActualServo > AnguloLlegadaServo) {
    for (int i = AnguloActualServo; i >= AnguloLlegadaServo; i--) {
      ledcWrite(CanalServo, i);
    }
    AnguloActualServo = AnguloLlegadaServo;
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

void EncenderDisplay7 (int NumeroDisplay) {
  Serial.println(NumeroDisplay);
  switch(NumeroDisplay) {
    case 1:
      digitalWrite(PinSegmentoA, LOW);
      digitalWrite(PinSegmentoB, HIGH);
      digitalWrite(PinSegmentoC, HIGH);
      digitalWrite(PinSegmentoD, LOW);
      digitalWrite(PinSegmentoE, LOW);
      digitalWrite(PinSegmentoF, LOW);
      digitalWrite(PinSegmentoG, LOW);
      break;
    case 2:
      digitalWrite(PinSegmentoA, HIGH);
      digitalWrite(PinSegmentoB, HIGH);
      digitalWrite(PinSegmentoC, LOW);
      digitalWrite(PinSegmentoD, HIGH);
      digitalWrite(PinSegmentoE, HIGH);
      digitalWrite(PinSegmentoF, LOW);
      digitalWrite(PinSegmentoG, HIGH);
      break;
    case 3:
      digitalWrite(PinSegmentoA, HIGH);
      digitalWrite(PinSegmentoB, HIGH);
      digitalWrite(PinSegmentoC, HIGH);
      digitalWrite(PinSegmentoD, HIGH);
      digitalWrite(PinSegmentoE, LOW);
      digitalWrite(PinSegmentoF, LOW);
      digitalWrite(PinSegmentoG, HIGH);
      break;
    case 4:
      digitalWrite(PinSegmentoA, LOW);
      digitalWrite(PinSegmentoB, HIGH);
      digitalWrite(PinSegmentoC, HIGH);
      digitalWrite(PinSegmentoD, LOW);
      digitalWrite(PinSegmentoE, LOW);
      digitalWrite(PinSegmentoF, HIGH);
      digitalWrite(PinSegmentoG, HIGH);
      break;
    case 5:
      digitalWrite(PinSegmentoA, HIGH);
      digitalWrite(PinSegmentoB, LOW);
      digitalWrite(PinSegmentoC, HIGH);
      digitalWrite(PinSegmentoD, HIGH);
      digitalWrite(PinSegmentoE, LOW);
      digitalWrite(PinSegmentoF, HIGH);
      digitalWrite(PinSegmentoG, HIGH);
      break;
    case 6:
      digitalWrite(PinSegmentoA, HIGH);
      digitalWrite(PinSegmentoB, LOW);
      digitalWrite(PinSegmentoC, HIGH);
      digitalWrite(PinSegmentoD, HIGH);
      digitalWrite(PinSegmentoE, HIGH);
      digitalWrite(PinSegmentoF, HIGH);
      digitalWrite(PinSegmentoG, HIGH);
      break;
    case 7:
      digitalWrite(PinSegmentoA, HIGH);
      digitalWrite(PinSegmentoB, HIGH);
      digitalWrite(PinSegmentoC, HIGH);
      digitalWrite(PinSegmentoD, LOW);
      digitalWrite(PinSegmentoE, LOW);
      digitalWrite(PinSegmentoF, LOW);
      digitalWrite(PinSegmentoG, LOW);
      break;
    case 8:
      digitalWrite(PinSegmentoA, HIGH);
      digitalWrite(PinSegmentoB, HIGH);
      digitalWrite(PinSegmentoC, HIGH);
      digitalWrite(PinSegmentoD, HIGH);
      digitalWrite(PinSegmentoE, HIGH);
      digitalWrite(PinSegmentoF, HIGH);
      digitalWrite(PinSegmentoG, HIGH);
      break;
    case 9:
      digitalWrite(PinSegmentoA, HIGH);
      digitalWrite(PinSegmentoB, HIGH);
      digitalWrite(PinSegmentoC, HIGH);
      digitalWrite(PinSegmentoD, HIGH);
      digitalWrite(PinSegmentoE, LOW);
      digitalWrite(PinSegmentoF, HIGH);
      digitalWrite(PinSegmentoG, HIGH);
      break;
    case 0:
      digitalWrite(PinSegmentoA, HIGH);
      digitalWrite(PinSegmentoB, HIGH);
      digitalWrite(PinSegmentoC, HIGH);
      digitalWrite(PinSegmentoD, HIGH);
      digitalWrite(PinSegmentoE, HIGH);
      digitalWrite(PinSegmentoF, HIGH);
      digitalWrite(PinSegmentoG, LOW);
      break;
  }
}

void EnviarAdafruit (void) {
  io.run();
  Serial.print("sending -> ");
  Serial.println(TemperaturaAdafruit);
  tempCanal->save(TemperaturaAdafruit);
}
