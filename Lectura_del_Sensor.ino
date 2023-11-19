#include <NewPing.h>
#include <Arduino.h>
#include <BluetoothSerial.h>
#include <LiquidCrystal.h>


#define TRIGGER_PIN 4
#define ECHO_PIN 2
#define MAX_DISTANCE 400

NewPing ultrasonic(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

unsigned int valor_distancia;

const int buttonPin_Send = 21;
int lastButtonState = LOW;

const int buttonPin_MeasureType = 22;
int lastButtonState_MeasureType = LOW;
int MeasureType_Counter = 0;

const char* mediciones[] = {"Posterior", "Anterior", "Altura", "Largo", "Ancho"};

BluetoothSerial SerialBT;

// Define los pines para la conexión a la pantalla LCD
const int rs = 19;
const int en = 23;
const int d4 = 18;
const int d5 = 17;
const int d6 = 16;
const int d7 = 15;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int pinDeEncendidoDePantalla = 5; // Reemplaza con el pin al que está conectado el interruptor de la pantalla

bool ultimo_Estado_Pantalla = LOW;
int estado_Pantalla;


void setup() {
  Serial.begin(9600);
  pinMode(buttonPin_Send, INPUT);
  pinMode(buttonPin_MeasureType, INPUT);
  SerialBT.begin("ESP32_BT_Connection");
  
  
  
  

  estado_Pantalla = digitalRead(pinDeEncendidoDePantalla);
  if (estado_Pantalla == HIGH && ultimo_Estado_Pantalla == LOW) {
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    // Print a message to the LCD.
    lcd.setCursor(4, 0);
    lcd.print("MediCES");
    ultimo_Estado_Pantalla = estado_Pantalla;
  }
  delay(3000);
}

void loop() {
  int buttonState_MeasureType = digitalRead(buttonPin_MeasureType);

  estado_Pantalla = digitalRead(pinDeEncendidoDePantalla);

  if (estado_Pantalla == HIGH && ultimo_Estado_Pantalla == LOW) {
    lcd.begin(16, 2);
    ultimo_Estado_Pantalla = estado_Pantalla;
  } else {

  }

  // Calcula la posición para centrar el texto
  int xPos = (16 - strlen(mediciones[MeasureType_Counter])) / 2;
  
  lcd.setCursor(xPos, 0);
  lcd.print(mediciones[MeasureType_Counter]);

  if (buttonState_MeasureType == HIGH && lastButtonState_MeasureType == LOW) {
    lcd.clear();  // Limpia la pantalla
    MeasureType_Counter = (MeasureType_Counter + 1) % 5;

    // Calcula nuevamente la posición para centrar el texto
    xPos = (16 - strlen(mediciones[MeasureType_Counter])) / 2;
    
    lcd.clear();  // Limpia la pantalla
    lcd.setCursor(xPos, 0);
    lcd.print(mediciones[MeasureType_Counter]);
  }

  int buttonState = digitalRead(buttonPin_Send);

  if (buttonState == HIGH && lastButtonState == LOW) {
    valor_distancia = ultrasonic.ping_cm();
    Serial.print("Distancia: ");
    Serial.print(valor_distancia);
    Serial.println(" cm");
    lcd.clear();  // Limpia la pantalla
    lcd.setCursor(xPos, 0);
    lcd.print(mediciones[MeasureType_Counter]);
    lcd.setCursor(5, 1);
    lcd.print(valor_distancia);
    lcd.print(" cm");
    sendBluetoothData(mediciones[MeasureType_Counter]);
    sendBluetoothData(valor_distancia);
  }

  lastButtonState_MeasureType = buttonState_MeasureType;
  lastButtonState = buttonState;
}

void sendBluetoothData(unsigned int distancia) {
  SerialBT.print("Distancia: ");
  SerialBT.print(distancia);
  SerialBT.println(" cm");
}

void sendBluetoothData(const char* medida) {
  SerialBT.print("Medida: ");
  SerialBT.println(medida);
}

