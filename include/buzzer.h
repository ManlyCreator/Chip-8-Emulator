#ifndef BUZZER_H
#define BUZZER_H

#include <AL/al.h>
#include <AL/alc.h>

#define SAMPLE_RATE 44100
#define FREQUENCY 220
#define NUM_SAMPLES SAMPLE_RATE

typedef struct {
  ALuint source;
  ALCdevice *device;
  ALCcontext *context;
} Buzzer;

Buzzer buzzerInit();
void buzzerPlay(Buzzer *buzzer);
void buzzerStop(Buzzer *buzzer);
void buzzerDelete(Buzzer *buzzer);

#endif
