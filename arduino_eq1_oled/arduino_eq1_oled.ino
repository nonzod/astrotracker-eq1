#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <AccelStepper.h>
#include <ButtonDebounce.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define PIN_STEP 3
#define PIN_DIRECTION 4
#define PIN_ENABLE 5
#define BUTTON_AUTOMODE_PIN 9
#define BUTTON_CW_PIN 7
#define BUTTON_CCW_PIN 8

byte Play[] = {
  B00000,
  B01000,
  B01100,
  B01110,
  B01110,
  B01100,
  B01000,
  B00000
};
byte Stop[] = {
  B00000,
  B01010,
  B01010,
  B01010,
  B01010,
  B01010,
  B01010,
  B00000
};
int modState = 0, selectedSpeed = 0;
bool motorStopped = false, isRunning = false, initAuto = true, initMan = true;
unsigned long timeout;

float speedsValue[] = { // Velocità di inseguimento per oggetto
  15.23, // 15.23 Stelle
  17,    // Luna
};
float
autoFinalPosition = 54666.00, // Durata dell'inseguimento in ascensione retta (54666 == 1h)
manualSpeed = 6400.00;        // Velocità spostamenti in manuale

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

AccelStepper stepper(AccelStepper::DRIVER, PIN_STEP, PIN_DIRECTION);

ButtonDebounce BUTTON_AUTOMODE(BUTTON_AUTOMODE_PIN, 500);
ButtonDebounce BUTTON_CW(BUTTON_CW_PIN, 250);
ButtonDebounce BUTTON_CCW(BUTTON_CCW_PIN, 250);

void setup()
{
  BUTTON_AUTOMODE.setCallback(onAutomodeChanged);
  BUTTON_CW.setCallback(onCWChanged);
  BUTTON_CCW.setCallback(onCCWChanged);

  // Stato iniziale selettore modalità
  if (digitalRead(BUTTON_AUTOMODE_PIN) == LOW) {
    modState = LOW;
  } else {
    modState = HIGH;
  }

  initStepper();
  initLCD();
}

void loop() {
  BUTTON_AUTOMODE.update();
  BUTTON_CW.update();
  BUTTON_CCW.update();

  if (modState == LOW) { // Modalità manuale
    loopManual();
  } else {               // Modalita automatica
    loopAutomatic();
  }
}

/**
  Gestione loop in modalità Automatica

  @see: onCWChanged()
*/
void loopAutomatic() {
  if (initAuto == true) {
    stepper.setMaxSpeed(3500.00);
    stepper.setAcceleration(300000.00);
    stepper.setCurrentPosition(autoFinalPosition);
    initAuto = false;
    initMan = true;
    isRunning = false;
  }

  if (stepper.currentPosition() < autoFinalPosition) {
    stepper.runSpeed();
    isRunning = true;
  } else {
    isRunning = false;
  }
}

/**
  Gestione loop in modalità Manuale
*/
void loopManual() {
  if (initMan == true) {
    stepper.setMaxSpeed(10000.00);
    stepper.setAcceleration(300000.00);
    stepper.setCurrentPosition(0);
    initAuto = true;
    initMan = false;
    isRunning = false;
  }

  // Gira in senso orario
  if (BUTTON_CW.state() == HIGH) {
    manualRun(manualSpeed);
  }

  // Gira in senso antiorario
  if (BUTTON_CCW.state() == HIGH) {
    manualRun(-manualSpeed);
  }

  // Dopo 10 secondi di inattività disabilità i motori
  if (millis() > ( timeout + 10000)) {
    stepper.disableOutputs();
  }
}

/**
  Inizializzazione motore
*/
void initStepper() {
  stepper.setEnablePin(PIN_ENABLE);
  stepper.setPinsInverted(false, false, true);
  stepper.enableOutputs();
}

/**
  Inizializzazione LCD
*/
void initLCD() {

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  displayVersion();
  delay(2000);

  if (modState == LOW) {
    displayModeManual(false);
  } else {
    displayModeAuto();
  }
}

/**
  Rotazione manuale con i pulsanti
*/
void manualRun(int speed) {
  timeout = millis();
  if (stepper.runSpeed() == false) {
    stepper.enableOutputs();
    stepper.setSpeed(speed);
  }
}

/**
  Callback pulsante selezione modalità
*/
void onAutomodeChanged(const int state) {

  if (state == LOW) {
    modState = LOW;
    displayModeManual(false);
  } else {
    modState = HIGH;
    displayModeAuto();
  }
}

/**
  Gestione eventi al cambio di stato del pulsante CW
  Qui viene gestita l'avvio e lo stop dell'inseguimento automatico.
*/
void onCWChanged(const int state) {
  if (modState == HIGH) { // Se in modalità automatica
    if (state == HIGH) {
      if (isRunning == false) {
        stepper.enableOutputs();
        stepper.setCurrentPosition(0);
        stepper.setSpeed(speedsValue[selectedSpeed]);
      }  else {
        stepper.disableOutputs();
        stepper.setCurrentPosition(autoFinalPosition);
        stepper.setSpeed(0);
      }
    }

    displayModeAuto();
  } else {
    displayModeManual(state);
  }
}

/**
  Gestione eventi al cambio di stato del pulsante CCW
*/
void onCCWChanged(const int state) {
  if (modState == HIGH) { // Se in modalità automatica
    if (state == HIGH && isRunning == false) {
      ciclaAutoSpeeds();
    }
    displayModeAuto();
  } else {
    displayModeManual(state);
  }
}

/**
   Cicla tra le velocità disponibili
*/
void ciclaAutoSpeeds() {
  selectedSpeed++;
  if (selectedSpeed > 1) {
    selectedSpeed = 0;
  }
}
