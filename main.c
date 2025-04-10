// Standard Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// External Libraries
#include "buzzer.h"
#include "chip8.h"

// TODO: Ensure that all instructions work correctly and that clock cycles are accurate
// TODO: Debug sprites reappearing in Space Invaders (timers?)
// TODO: Debug seg faults

int main(int argc, char **argv) {
  char rom[256] = "../roms/";
  Chip8 chip8;

  if (argv[1]) {
    strcat(rom, argv[1]);
  } else {
    char temp[256];
    printf("Insert a ROM: ");
    scanf("%s", temp);
    strcat(rom, temp);
  }
  strcat(rom, ".ch8");

  chip8 = chipInitialize();
  if (!chipLoadROM(&chip8, rom))
    return -1;
  chipStartMainLoop(&chip8, 16);

  return 0;
}
