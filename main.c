// Standard Libraries
#include <stdio.h>

// External Libraries
#include "buzzer.h"
#include "chip8.h"

// TODO: Debug why sound timer is always nonzero in Chip8
// TODO: Use a soundPlaying flag to ensure that the sound is not constantly re-played or stopped
// TODO: Implement Buzzer in Chip8
// TODO: Debug seg faults
// TODO: Load ROMs from CLI

int main(int argc, char **argv) {
  Chip8 chip8 = chipInitialize();
  buzzerPlay(&chip8.buzzer);
  if (!chipLoadROM(&chip8, "../roms/spaceInvaders.ch8"))
    return -1;
  chipStartMainLoop(&chip8, 16);

  return 0;
}
