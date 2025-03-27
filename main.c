// Standard Libraries
#include <stdio.h>
#include <stdlib.h>

// External Libraries
#include "chip8.h"

// TODO: Sound

int main(int argc, char **argv) {
  Chip8 chip8;

  // Chip8
  chip8 = chipInitialize();
  if (!chipLoadROM(&chip8, "../roms/spaceInvaders.ch8"))
    return -1;
  chipStartMainLoop(&chip8, 16);

  return 0;
}
