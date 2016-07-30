#include "common.h"
#include <Bounce2.h>

#define JOY_BUTTON_DEADMAN A3
#define JOY_BUTTON_TOP A5
#define JOY_BUTTON_BOT A4
#define JOY_REF A0
#define JOY_Y A1
#define JOY_X A2

#define JOY_READ_PERIOD_MS 100

#define PIN_NONE 0

#define JOY_SLOW_THRESHOLD 350

Bounce joystickDeadman = Bounce(JOY_BUTTON_DEADMAN, 10);

int16_t joystickRef;
int16_t joystickY;
int16_t joystickX;

uint16_t pulseUsY;
uint16_t pulseUsX;

uint16_t curPulseUsY;
uint16_t curPulseUsX;

uint32_t lastJoyRead;

uint32_t stepYts;
uint32_t stepXts;


int stepYPin = PIN_NONE;
int stepXPin = PIN_NONE;

uint16_t maxPulseLengthY = 4000;
uint16_t maxPulseLengthX = 4000;

uint8_t phaseY = 0;
uint8_t phaseX = 0;

uint32_t accelerationTs = 0;


inline static uint8_t isAccelerationTime(uint32_t accTs) {
  return micros() - accTs > 200;
}

void joystickSetup() {
}

inline static void setYDirection() {
	if(joystickY > 0) {
		digitalWrite(DIR_TILT, HIGH);
		digitalWrite(DIR_BASE, LOW);      
		digitalWrite(DIR_HEAD_TILT, HIGH);
	} else {
		digitalWrite(DIR_TILT, LOW);
		digitalWrite(DIR_BASE, HIGH);
		digitalWrite(DIR_HEAD_TILT, LOW);
	}
}

inline static void setXDirection() {
	if(joystickX > 0) {
		digitalWrite(DIR_PAN, HIGH);
		digitalWrite(DIR_HEAD_PAN, LOW);
	} else {
		digitalWrite(DIR_PAN, LOW);
		digitalWrite(DIR_HEAD_PAN, HIGH);
	}
}

void setMaxPulseLength(int16_t joystickVal, uint16_t *maxPulseLength, uint16_t slow, uint16_t fast) {
	if(abs(joystickVal) < JOY_SLOW_THRESHOLD) {
		*maxPulseLength = slow;
	} else {
		*maxPulseLength = fast;
	}
}

inline static void setupYMovement() {
	if(LOW == digitalRead(JOY_BUTTON_TOP)) {
		stepYPin = STEP_TILT;
		setMaxPulseLength(joystickY, &maxPulseLengthY, 400, 100);

		if(stepYPin == PIN_NONE) {
			curPulseUsY = 400;
		}
	} else if(LOW == digitalRead(JOY_BUTTON_BOT)) {
		stepYPin = STEP_BASE;
		setMaxPulseLength(joystickY, &maxPulseLengthY, 4000, 300);
	} else {
		stepYPin = STEP_HEAD_TILT;
		setMaxPulseLength(joystickY, &maxPulseLengthY, 4000, 1000);
	}

	pulseUsY = maxPulseLengthY - map(abs(joystickY), 0, 1023 - joystickRef, 0, maxPulseLengthY);

	if(pulseUsY > 65000) {
		pulseUsY = 20;
	}
}

inline static void setupXMovement() {
	if(LOW == digitalRead(JOY_BUTTON_TOP)) {
		stepXPin = STEP_PAN;
		setMaxPulseLength(joystickX, &maxPulseLengthX, 1500, 400);
	} else {
		stepXPin = STEP_HEAD_PAN;
		setMaxPulseLength(joystickX, &maxPulseLengthX, 4000, 1000);
	}
	pulseUsX = maxPulseLengthX - map(abs(joystickX), 0, 1023 - joystickRef, 0, maxPulseLengthX); 
}

inline static void joystickRead() {
	joystickRef = analogRead(JOY_REF);
	joystickY = analogRead(JOY_Y) - joystickRef;
	joystickX = analogRead(JOY_X) - joystickRef;

	Serial.print(joystickX);
	Serial.print(" ");
	Serial.print(joystickY);

	if(abs(joystickY) > 20) {
		setYDirection();
		setupYMovement();
	} else {
		stepYPin = PIN_NONE;
		curPulseUsY = 4000;
	}

	if(abs(joystickX) > 20) {
		setXDirection();
		setupXMovement();
	} else {
		stepXPin = PIN_NONE;
		curPulseUsX = 4000;
	}

	Serial.print(" ");
	Serial.print(curPulseUsX);
	Serial.print(" ");
	Serial.print(curPulseUsY);
	Serial.print(" ");
	Serial.print(maxPulseLengthX);
	Serial.print(" ");
	Serial.print(maxPulseLengthY);
	Serial.println();
}

uint8_t isTimeToReadJoystick() {
	return millis() - lastJoyRead > JOY_READ_PERIOD_MS;
}

void processJoystick() {
	if(isTimeToReadJoystick()) {
		joystickRead();
		lastJoyRead = millis();
	}
  
	if(PIN_NONE != stepYPin) {
    	if(isPulseTime(stepYts, curPulseUsY)) {
      		digitalWrite(stepYPin, phaseY);
      		phaseY = !phaseY;
      		stepYts = micros();
    	}
    
	    if(isAccelerationTime(accelerationTs)){
			if(curPulseUsY < pulseUsY) {
				curPulseUsY +=1;
			} else if(curPulseUsY > pulseUsY) {
				curPulseUsY -=1;
			}
			accelerationTs = micros();
	    }
	}

	if(PIN_NONE != stepXPin) {
		if(isPulseTime(stepXts, curPulseUsX)) {
			digitalWrite(stepXPin, phaseX);
			phaseX = !phaseX;
			stepXts = micros();
		}

		if(isAccelerationTime(accelerationTs)){
			if(curPulseUsX < pulseUsX) {
				curPulseUsX +=1;
			} else if(curPulseUsX > pulseUsX) {
				curPulseUsX -=1;
			}
			accelerationTs = micros();
		}

	}
}

