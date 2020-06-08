#include <LiquidCrystal_I2C.h>
#include <AccelStepper.h>
#include <ButtonDebounce.h>

#define PIN_STEP 3
#define PIN_DIRECTION 4
#define PIN_ENABLE 5
#define BUTTON_AUTOMODE_PIN 9
#define BUTTON_CW_PIN 7
#define BUTTON_CCW_PIN 8

int modState = 0, selectedSpeed = 0;
bool motorStopped = false, isRunning = false, initAuto = true, initMan = true;
unsigned long timeout;

float speedsValue[] = { // Velocità di inseguimento per oggetto
  2000.00, // 15.23 Stelle
  4000.00,    // Luna
};
float
autoFinalPosition = 54666.00, // Durata dell'inseguimento in ascensione retta (54666 == 1h)
manualSpeed = 6400.00;        // Velocità spostamenti in manuale

AccelStepper stepper(AccelStepper::DRIVER, PIN_STEP, PIN_DIRECTION);
LiquidCrystal_I2C lcd(0x27, 16, 2);

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
  lcd.init(); // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();
  lcd.print("Astroinseguitore");
  lcd.setCursor(0, 1);
  lcd.print("Versione 0.7");
  delay(2000);
  lcd.clear();
  if (modState == LOW) {
    lcd.setCursor(0, 0);
    lcd.print("Manuale");
    lcd.setCursor(0, 1);
    lcd.print("Vel: " + String(manualSpeed));
  } else {
    printTarget();
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
  lcd.clear();
  if (state == LOW) {
    modState = LOW;
    lcd.setCursor(0, 0);
    lcd.print("Manuale");
    lcd.setCursor(0, 1);
    lcd.print("Vel: " + String(manualSpeed));
  } else {
    modState = HIGH;
    printTarget();
  }
}

/**
  Gestione eventi al cambio di stato del pulsante CW
  Qui viene gestita l'avvio e lo stop dell'inseguimento automatico.
*/
void onCWChanged(const int state) {
  if (modState == HIGH && state == HIGH) { // Se in modalità automatica
    if (isRunning == false) {
      stepper.enableOutputs();
      stepper.setCurrentPosition(0);
      stepper.setSpeed(speedsValue[selectedSpeed]);

      lcd.setCursor(15, 1);
      lcd.print(">");
    }  else {
      stepper.disableOutputs();
      stepper.setCurrentPosition(autoFinalPosition);
      stepper.setSpeed(0);

      lcd.setCursor(15, 1);
      lcd.print("|");
    }
  }
}

/**
  Gestione eventi al cambio di stato del pulsante CCW
*/
void onCCWChanged(const int state) {
  if (modState == HIGH && state == HIGH && isRunning == false) { // Se in modalità automatica
    ciclaAutoSpeeds();
    printTarget();
  }
}

/**
   Visualizza l'oggetto inseguito
*/
void printTarget() {
  lcd.setCursor(0, 0);
  lcd.print("Seguo:");
  lcd.setCursor(0, 1);
  switch (selectedSpeed) {
    case 0:
      lcd.print("le Stelle       ");
      break;
    case 1:
      lcd.print("la Luna         ");
      break;
    default:
      lcd.print("stocazzo        ");
      break;
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
