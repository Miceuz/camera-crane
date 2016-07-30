#ifndef __COMMON_H
#define __COMMON_H

#define STEP_TILT 4
#define DIR_TILT 5

#define STEP_PAN 6
#define DIR_PAN 7

#define STEP_BASE 8
#define DIR_BASE 9

//galva

#define STEP_HEAD_TILT 28
#define DIR_HEAD_TILT 29

#define STEP_HEAD_PAN 10
#define DIR_HEAD_PAN 11

inline static uint8_t isPulseTime(uint32_t stepTs, uint16_t timeout) {
  uint32_t us = micros();
  uint16_t diff;

  if(us < stepTs){
    diff = 65535L - stepTs + us;
  } else {
    diff = us - stepTs;
  }

  return diff > timeout;
}


#endif