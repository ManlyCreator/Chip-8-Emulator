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
  chip8->opcode = 0x8016;
  chip8->V[0x0] = 0x03;
  chip8->V[0x1] = 0x11;
  switch (chip8->opcode & 0xF000) {
    printf("Reached\n");
    Byte x = (chip8->opcode & 0x0F00) >> 8; 
    Byte y = (chip8->opcode & 0x00F0) >> 4;
    // JP addr
    case 0x1000:
      chip8->pc = chip8->opcode & 0x0FFF;
      break;
    // CALL addr
    case 0x2000:
      chip8->stack[chip8->sp++] = chip8->pc;
      chip8->pc = chip8->opcode & 0x0FFF;
      break;
    // SE V[x], byte
    case 0x3000:
      if (chip8->V[x] == (chip8->opcode & 0x00FF))
        chip8->pc += 2;
      break;
    // SNE V[x], byte
    case 0x4000:
      if (chip8->V[x] != (chip8->opcode & 0x00FF))
        chip8->pc += 2;
      break;
    // SE V[x], V[y]
    case 0x5000:
      if (chip8->V[x] == chip8->V[y])
        chip8->pc += 2;
      break;
    // LD V[x], byte
    case 0x6000:
      chip8->V[x] = chip8->opcode & 0x00FF;
      break;
    // Add V[x], byte
    case 0x7000:
      chip8->V[(chip8->opcode & 0x0F00) >> 8] += chip8->opcode & 0x00FF;
      break;
    case 0x8000:
      switch (chip8->opcode & 0x000F) {
        // LD V[x], V[y]
        case 0x0000:
          chip8->V[x] = chip8->V[y];
          break;
        // OR V[x], V[y]
        case 0x0001:
          chip8->V[x] |= chip8->V[y];
          break;
        // AND V[x], V[y]
        case 0x0002:
          chip8->V[x] &= chip8->V[y];
          break;
        // XOR V[x], V[y]
        case 0x0003:
          chip8->V[x] ^= chip8->V[y];
          break;
        // ADD V[x], V[y]
        case 0x0004:
          if (chip8->V[x] + chip8->V[y] > 0xFF)
            chip8->V[0xF] = 1;
          else
            chip8->V[0xF] = 0;
          chip8->V[x] += chip8->V[y] & 0xFF;
          break;
        // SUB V[x], V[y]
        case 0x0005:
          if (chip8->V[x] > chip8->V[y]) 
            chip8->V[0xF] = 1;
          else
            chip8->V[0xF] = 0;
          chip8->V[x] -= chip8->V[y];
          break;
        // SHR V[x]
        case 0x0006:
          if ((chip8->V[x] & 0x01) == 0x01)
            chip8->V[0xF] = 1;
          else
            chip8->V[0xF] = 0;
          chip8->V[x] /= 2;
          break;
        // SUBN V[x], V[y]
        case 0x0007:
          if (chip8->V[y] > chip8->V[x])
            chip8->V[0xF] = 1;
          else
            chip8->V[0xF] = 0;
          chip8->V[x] = chip8->V[y] - chip8->V[x];
          break;
        // SHL V[x]
        case 0x000E:
          if ((chip8->V[x] & 0x80) == 0x80) 
            chip8->V[0xF] = 1;
          else
            chip8->V[0xF] = 0;
          chip8->V[x] *= 2;
          break;
      }
      break;
  }

  chip8->pc += 2;
}
