#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>

Chip8 chipInitialize() {
  Chip8 chip8 = (Chip8){
    .pc = 0x200,
    .I  = 0,
    .sp = 0,
    .opcode = 0
  };
  return chip8;
}

int chipLoadROM(Chip8 *chip8, const char *romName) {
  FILE *rom = fopen(romName, "rb");  
  long bufferSize;
  Byte *buffer;

  if (!rom) {
    perror("Failed to open file\n");
    return 0;
  }

  fseek(rom, 0, SEEK_END);

  bufferSize = ftell(rom);
  buffer = malloc(sizeof(Byte) * bufferSize);

  rewind(rom);
  fread(buffer, sizeof(Byte), bufferSize, rom);

  for (int i = 0; i < bufferSize; i++) {
    chip8->memory[0x200 + i] = buffer[i];
  }

  return 1; 
}

void chipEmulateCycle(Chip8 *chip8) {
  chip8->opcode = chip8->memory[chip8->pc] << 8 | chip8->memory[chip8->pc + 1];

  // Decode Instructions
  chip8->opcode = 0x5AC0;
  switch (chip8->opcode & 0xF000) {
    case 0x1000:
      chip8->pc = chip8->opcode & 0x0FFF;
      break;
    case 0x2000:
      chip8->stack[++chip8->sp] = chip8->pc;
      chip8->pc = chip8->opcode & 0x0FFF;
      break;
    case 0x3000:
      if (chip8->V[(chip8->opcode & 0x0F00) >> 8] == (chip8->opcode & 0x00FF))
        chip8->pc += 2;
      break;
    case 0x4000:
      if (chip8->V[(chip8->opcode & 0x0F00) >> 8] != (chip8->opcode & 0x00FF))
        chip8->pc += 2;
      break;
    case 0x5000:
      if (chip8->V[(chip8->opcode & 0x0F00) >> 8] == chip8->V[(chip8->opcode & 0x00F0) >> 4])
        chip8->pc += 2;
      break;
  }
}
