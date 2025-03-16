#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Chip8 chipInitialize() {
  Chip8 chip8 = (Chip8){
    .I  = 0,
    .pc = 0x200,
    .sp = 0,
    .numPixels = 0,
    .delayTimer = 0,
    .soundTimer = 0,
    .opcode = 0
  };
  chip8.memory = calloc(MEMORY, sizeof(Byte));
  chip8.display = calloc(DISPLAY_WIDTH * DISPLAY_HEIGHT, sizeof(Byte));
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
  Byte x, y;

  chip8->opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
  printf("Reading addresses 0x%.3x and 0x%.3x\n", chip8->pc, chip8->pc + 1);
  printf("Opcode: 0x%.4x\n", chip8->opcode);

  // Decodes registers from the opcode
  x = (chip8->opcode & 0x0F00) >> 8; 
  y = (chip8->opcode & 0x00F0) >> 4;

  // Decode Instructions
  switch (chip8->opcode & 0xF000) {
    case 0x0000:
      switch (chip8->opcode) {
        // 0x00E0 - Clear Screen
        case 0x00E0:
          printf("Clearing Display\n");
          free(chip8->display);
          chip8->display = calloc(DISPLAY_WIDTH * DISPLAY_HEIGHT, sizeof(Byte));
          break;
        // 0x00EE - Return
        case 0x00EE:
          chip8->pc = chip8->stack[--chip8->sp] + 2;
          printf("Returning to 0x%.3x\n", chip8->pc);
          break;
      }
      break;
    // 0x1nnn - Jump to address nnn
    case 0x1000:
      chip8->pc = chip8->opcode & 0x0FFF;
      break;
    // 0x2nnn - Call function at nnn
    case 0x2000:
      chip8->stack[chip8->sp++] = chip8->pc;
      chip8->pc = chip8->opcode & 0x0FFF;
      printf("Calling function at 0x%.3x\n", chip8->pc);
      break;
    // 0x3xbb - Skip next instruction if V[x] == bb
    case 0x3000:
      if (chip8->V[x] == (chip8->opcode & 0x00FF))
        chip8->pc += 4;
      break;
    // 0x4xbb - Skip next instruction if V[x] != bb
    case 0x4000:
      if (chip8->V[x] != (chip8->opcode & 0x00FF))
        chip8->pc += 4;
      break;
    // 0x5xy0 - Skip next instruction if V[x] == V[y]
    case 0x5000:
      if (chip8->V[x] == chip8->V[y])
        chip8->pc += 4;
      break;
    // 0x6xbb - Load bb into V[x]
    case 0x6000:
      chip8->V[x] = chip8->opcode & 0x00FF;
      chip8->pc += 2;
      break;
    // 0x7xbb - Increment V[x] by bb
    case 0x7000:
      chip8->V[(chip8->opcode & 0x0F00) >> 8] += chip8->opcode & 0x00FF;
      chip8->pc += 2;
      break;
    case 0x8000:
      switch (chip8->opcode & 0x000F) {
        // 0x8xy0 - Load V[y] into V[x]
        case 0x0000:
          chip8->V[x] = chip8->V[y];
          chip8->pc += 2;
          break;
        // 0x8xy1 - Set V[x] = V[x] OR V[y]
        case 0x0001:
          chip8->V[x] |= chip8->V[y];
          chip8->pc += 2;
          break;
        // 0x8xy2 - Set V[x] = V[x] AND V[y]
        case 0x0002:
          chip8->V[x] &= chip8->V[y];
          chip8->pc += 2;
          break;
        // 0x8xy3 - Set V[x] = V[x] XOR V[y]
        case 0x0003:
          chip8->V[x] ^= chip8->V[y];
          chip8->pc += 2;
          break;
        // 0x8xy4 - Increment V[x] by V[y]
        case 0x0004:
          if (chip8->V[x] + chip8->V[y] > 0xFF)
            chip8->V[0xF] = 1;
          else
            chip8->V[0xF] = 0;
          chip8->V[x] += chip8->V[y] & 0xFF;
          chip8->pc += 2;
          break;
        // 0x8xy5 - Decrement V[x] by V[y]
        case 0x0005:
          if (chip8->V[x] > chip8->V[y]) 
            chip8->V[0xF] = 1;
          else
            chip8->V[0xF] = 0;
          chip8->V[x] -= chip8->V[y];
          chip8->pc += 2;
          break;
        // 0x8xy6 - Shift right V[x] by 1 bit
        case 0x0006:
          if ((chip8->V[x] & 0x01) == 0x01)
            chip8->V[0xF] = 1;
          else
            chip8->V[0xF] = 0;
          chip8->V[x] /= 2;
          chip8->pc += 2;
          break;
        // 0x8xy7 - Set V[x] = V[y] - V[x]
        case 0x0007:
          if (chip8->V[y] > chip8->V[x])
            chip8->V[0xF] = 1;
          else
            chip8->V[0xF] = 0;
          chip8->V[x] = chip8->V[y] - chip8->V[x];
          chip8->pc += 2;
          break;
        // 0x8xyE - Shift left V[x] by 1 bit
        case 0x000E:
          if ((chip8->V[x] & 0x80) == 0x80) 
            chip8->V[0xF] = 1;
          else
            chip8->V[0xF] = 0;
          chip8->V[x] *= 2;
          chip8->pc += 2;
          break;
      }
      break;
    // 0x9xy0 - Skip next instruction if V[x] != V[y]
    case 0x9000:
      if (chip8->V[x] != chip8->V[y])
        chip8->pc += 4;
      break;
    // 0xAnnn - Load nnn into I
    case 0xA000:
      chip8->I = chip8->opcode & 0x0FFF;
      chip8->pc += 2;
      break;
    // 0xBnnn - Jump to address nnn + V[0]
    case 0xB000:
      chip8->pc = chip8->V[0] + chip8->opcode & 0x0FFF;
      printf("Jumped to address 0x%.3x\n", chip8->pc);
      break;
    // 0xCxbb - Set V[x] = rand(0, 255) AND bb
    case 0xC000:
      srand(time(NULL));
      chip8->V[x] = (rand() % 256) & (chip8->opcode & 0x00FF);
      printf("Generated random number in V[0x%x]: 0x%.2x\n", x, chip8->V[x]);
      break;
    // 0xDxyn - Draw a sprite of n-bytes high at (x, y)
    case 0xD000: {
      Byte spriteRow;
      Byte height = chip8->opcode & 0x000F;
      printf("Drawing:\n");
      chip8->V[0xF] = 0;
      for (int i = 0; i < height; i++) {
        spriteRow = chip8->memory[chip8->I + i];
        for (int j = 0; j < 8; j++) {
          unsigned index = x + j + ((y + i) * DISPLAY_WIDTH);
          Byte pixel = (spriteRow & (0x1 << j)) >> j;
          if (chip8->display[index] == 1) {
            chip8->V[0xF] = 1;
            chip8->numPixels--;
          }
          chip8->display[index] ^= pixel;
          printf("%d", chip8->display[index]);
          if (chip8->display[index] == 1)
            chip8->numPixels++;
        }
        printf("\n");
      }
      break;
    }
  }
}
