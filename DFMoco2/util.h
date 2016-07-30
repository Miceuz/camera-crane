uint8_t dir = 0;
uint32_t steps = 0;

void changeDir() {
  if(dir) {
      digitalWrite(DIR_TILT, HIGH);
      digitalWrite(DIR_BASE, HIGH);      
      digitalWrite(DIR_HEAD_TILT, HIGH);
      digitalWrite(DIR_PAN, HIGH);
      digitalWrite(DIR_HEAD_PAN, HIGH);
  } else {
      digitalWrite(DIR_TILT, LOW);
      digitalWrite(DIR_BASE, LOW);      
      digitalWrite(DIR_HEAD_TILT, LOW);
      digitalWrite(DIR_PAN, LOW);
      digitalWrite(DIR_HEAD_PAN, LOW);
  }
  dir = !dir;
  delay(1000);  
}

void stepOnce() {
  digitalWrite(STEP_TILT, HIGH);
  digitalWrite(STEP_BASE, HIGH);      
  digitalWrite(STEP_HEAD_TILT, HIGH);
  digitalWrite(STEP_PAN, HIGH);
  digitalWrite(STEP_HEAD_PAN, HIGH);
  delayMicroseconds(800);
  digitalWrite(STEP_TILT, LOW);
  digitalWrite(STEP_BASE, LOW);      
  digitalWrite(STEP_HEAD_TILT, LOW);
  digitalWrite(STEP_PAN, LOW);
  digitalWrite(STEP_HEAD_PAN, LOW);
  delayMicroseconds(800);
}

void excercise() {
  TCCR1B = 0;
  while(true){
    stepOnce();
    if(steps++ > 3000) {
      changeDir();
      steps = 0;    
    }  

  }
}
