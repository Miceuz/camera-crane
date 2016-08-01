#include "common.h"
#include <Bounce2.h>

#define baseForward()     digitalWrite(DIR_BASE, LOW)
#define baseReverse()     digitalWrite(DIR_BASE, HIGH)
#define panForward()      digitalWrite(DIR_PAN, HIGH)
#define panReverse()      digitalWrite(DIR_PAN, LOW)
#define tiltForward()      digitalWrite(DIR_TILT, HIGH)
#define tiltReverse()      digitalWrite(DIR_TILT, LOW)

#define LED1 13
#define LED2 20
#define LED3 21

#define SW1 2
#define SW2 3
#define SW3 12

#define END_SW1 A8
#define END_SW2 A9
#define END_SW3 A10

Bounce sw1 = Bounce(SW1, 10);
Bounce sw2 = Bounce(SW2, 10);
Bounce sw3 = Bounce(SW3, 10);

Bounce esw1 = Bounce(END_SW1, 10);
Bounce esw2 = Bounce(END_SW2, 10);
Bounce esw3 = Bounce(END_SW3, 10);

uint32_t stepTs;
uint16_t pulseUs = 4000;

uint32_t steps = 0;
uint8_t phase = HIGH;

void calibrateSetup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  delay(2000);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
}

void move(uint8_t stepPin, uint32_t stepsToDo, uint8_t waitForSwitch) {
  steps = 0;
  while(steps < stepsToDo) {
    if(isPulseTime(stepTs, pulseUs)) {
      digitalWrite(stepPin, phase);
      phase = !phase;
      steps++;
      stepTs = micros();

      esw1.update();
      esw2.update();
      esw3.update();

      if(esw1.read()) {
        if(waitForSwitch && STEP_BASE == stepPin) {
          break;
        }
      }

      if(esw2.read()) {
        if(waitForSwitch && STEP_PAN == stepPin) {
          break;
        }
      }

      if(esw3.read()) {
        if(waitForSwitch && STEP_TILT == stepPin) {
          break;
        }
      }
    }
  }
  Serial.println(String("Done ") + steps + String(" out of ") + stepsToDo);
}

uint8_t touchBase() {
  digitalWrite(LED3, LOW);
  digitalWrite(LED2, HIGH);

  pulseUs = 500;
  baseForward();
  move(STEP_BASE, 10000, 1);
  if(steps == 10000) {
    digitalWrite(LED3, HIGH);
    return 0;
  }
  delay(2000);

  pulseUs = 1000;
  baseReverse();
  move(STEP_BASE, 1000, 0);

  delay(2000);
  
  pulseUs = 8000;
  baseForward();
  move(STEP_BASE, 2000, 1);

  delay(2000);

  baseReverse();

  steps=0;
  while(esw1.read()) {
    if(isPulseTime(stepTs, pulseUs)) {
      digitalWrite(STEP_BASE, phase);
      phase = !phase;
      steps++;
      stepTs = micros();
    }
    esw1.update();
  }

  Serial.println(String("BASE backlash ") + steps);

  move(STEP_BASE, 4000, 0);
  digitalWrite(LED2, LOW);
  return 1;
}

uint8_t touchPan() {
  digitalWrite(LED3, LOW);
  digitalWrite(LED2, HIGH);

  pulseUs = 2000;
  panForward();
  move(STEP_PAN, 10000, 1);

  if(10000 == steps) {
    digitalWrite(LED3, HIGH);
    return 0;
  }
  
  delay(3000);

  panReverse();
  pulseUs = 750;
  move(STEP_PAN, 30000, 0);

  delay(5000);

  panForward();
  move(STEP_PAN, 32000, 1);
  delay(5000);

  pulseUs = 2000;
  panReverse();
  steps=0;
  while(esw2.read()) {
    if(isPulseTime(stepTs, pulseUs)) {
      digitalWrite(STEP_PAN, phase);
      phase = !phase;
      steps++;
      stepTs = micros();
    }
    esw2.update();
  }

  Serial.println(String("PAN backlash ") + steps);

  move(STEP_PAN, 1800, 0);

  digitalWrite(LED2, LOW);
  return 1;
}

uint8_t touchTilt() {
  digitalWrite(LED3, LOW);
  digitalWrite(LED2, HIGH);

  pulseUs = 2000;
  tiltForward();
  move(STEP_TILT, 10000, 1);

  if(10000 == steps) {
    digitalWrite(LED3, HIGH);
    return 0;
  }
  delay(2000);

  pulseUs = 8000;
  tiltReverse();
  move(STEP_TILT, 500, 0);
  delay(2000);

  tiltForward();

  move(STEP_TILT, 1000, 1);
  delay(2000);

  tiltReverse();
  steps=0;
  while(esw3.read()) {
    if(isPulseTime(stepTs, pulseUs)) {
      digitalWrite(STEP_TILT, phase);
      phase = !phase;
      steps++;
      stepTs = micros();
    }
    esw3.update();
  }

  Serial.println(String("TILT backlash ") + steps);

  move(STEP_TILT, 5000, 1);

  return 1;
}

void calibrate() {
  Serial.println("Starting calibration...");
  uint32_t startTs = millis();

  if(touchBase() && touchTilt() && touchPan()){ 
    baseReverse();
    pulseUs = 1000;
    move(STEP_BASE, 100000, 0);

    Serial.println(String("Calibration took ") + (millis() - startTs) + String("ms"));
    for(int i = 0; i < 5; i++) {
      digitalWrite(LED1, HIGH);
      delay(500);
      digitalWrite(LED1, LOW);
      delay(500);
    }
  }
}
