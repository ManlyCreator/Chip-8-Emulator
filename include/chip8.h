#ifndef CHIP_8_H
#define CHIP_8_H

#include "screen.h"

#define MEMORY 4096
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define DISPLAY_FREQUENCY (float)1 / 60

#define Byte unsigned char
#define SignedByte char
#define Word unsigned short

typedef struct {
  // Memory
  Byte *memory;
  Byte V[16];
  Byte key[16];
  Word I;

  // Display
  Byte *display;
  Screen screen;

  // State
  Word pc;
  Word stack[16];
  Byte sp;
  SignedByte keyPressed;

  // Timers
  Byte delayTimer;
  Byte soundTimer;

  Word opcode;
} Chip8;

Chip8 chipInitialize();
int chipLoadROM(Chip8 *chip8, const char *romPath);
void chipStartMainLoop(Chip8 *chip8, unsigned instructionFrequency);
void chipTick(Chip8 *chip8, int steps);
void chipProcessInput(Chip8 *chip8);

#endif
