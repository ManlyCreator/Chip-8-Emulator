// Standard Libraries
#include <AL/alc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// External Libraries
#include "chip8.h"
#include <AL/al.h>

#define SAMPLE_RATE 44100
#define FREQUENCY 440
#define DURATION 1.0
#define NUM_SAMPLES (int)(SAMPLE_RATE * DURATION)

// TODO: Implement OpenAL tone from Audio code snippet
// TODO: Debug seg faults
// TODO: Load ROMs from CLI

int main(int argc, char **argv) {

  Chip8 chip8 = chipInitialize();
  if (!chipLoadROM(&chip8, "../roms/astroDodge.ch8"))
    return -1;
  chipStartMainLoop(&chip8, 16);

  return 0;
}
