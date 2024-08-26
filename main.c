//CODIGO PARA LEER LA ENTRADA ADC

#include <Arduino.h> //Librería de Arduino

//Pines para las entradas
#define Boton1 32
#define SensorTemp 33
#define PinReferencia 23

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
    TemperaturaAdafruit = Temperatura*0.1;
  }
}

void setup() {
  // Configurar pines como entradas
  pinMode(Boton1, INPUT_PULLUP);

  //Comunicación serial
  Serial.begin(115200);

  // Interupcion para medir la temperatura
  attachInterrupt(Boton1, Prueba, FALLING);

  //Configuración de la entrada analógica
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  //Configurar pines de ADC
  analogSetVRefPin(PinReferencia);
  pinMode(SensorTemp, INPUT);
}

void loop() {
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
