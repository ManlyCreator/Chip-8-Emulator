#include "opcodes.h"
#include "chip8.h"

void (*opcodes[16])(Chip8*) = {
  [0x0] = op0xxx,
  [0x1] = op1xxx,
  [0x2] = op2xxx,
  [0x3] = op3xxx,
  [0x4] = op4xxx,
  [0x5] = op5xxx,
  [0x6] = op6xxx,
  [0x7] = op7xxx,
  [0x8] = op8xxx,
  [0x9] = op9xxx,
  [0xA] = opAxxx,
  [0xB] = opBxxx,
  [0xC] = opCxxx,
  [0xD] = opDxxx,
  [0xE] = opExxx,
  [0xF] = opFxxx,
};

void op0xxx(Chip8 *chip8) {
  switch (chip8->opcode) {
    // 0x00E0 - Clear Screen
    case 0x00E0:
      if (chip8->debugFlag) printf("Clearing Display\n");
      free(chip8->display);
      chip8->display = calloc(DISPLAY_WIDTH * DISPLAY_HEIGHT, sizeof(Byte));
      chip8->pc += 2;
      break;
    // 0x00EE - Return
    case 0x00EE:
      chip8->pc = chip8->stack[--chip8->sp] + 2;
      if (chip8->debugFlag) printf("Returning to 0x%.3x\n", chip8->pc);
      break;
  }
}

// 0x1nnn - Jump to address nnn
void op1xxx(Chip8 *chip8) {
  chip8->pc = chip8->opcode & 0x0FFF;
  if (chip8->debugFlag) printf("Setting PC to: %d\n", chip8->pc);
}

// 0x2nnn - Call function at nnn
void op2xxx(Chip8 *chip8) {
  chip8->stack[chip8->sp++] = chip8->pc;
  chip8->pc = chip8->opcode & 0x0FFF;
  if (chip8->debugFlag) printf("Calling function at 0x%.3x\n", chip8->pc);
}

// 0x3xbb - Skip next instruction if V[x] == bb
void op3xxx(Chip8 *chip8) {
  Byte x = (chip8->opcode & 0x0F00) >> 8;
  if (chip8->V[x] == (chip8->opcode & 0x00FF))
    chip8->pc += 2;
  chip8->pc += 2;
}

// 0x4xbb - Skip next instruction if V[x] != bb
void op4xxx(Chip8 *chip8) {
  Byte x = (chip8->opcode & 0x0F00) >> 8;
  if (chip8->V[x] != (chip8->opcode & 0x00FF))
    chip8->pc += 2;
  chip8->pc += 2;
}

// 0x5xy0 - Skip next instruction if V[x] == V[y]
void op5xxx(Chip8 *chip8) {
  Byte x = (chip8->opcode & 0x0F00) >> 8;
  Byte y = (chip8->opcode & 0x00F0) >> 4;
  if (chip8->V[x] == chip8->V[y])
    chip8->pc += 2;
  chip8->pc += 2;
}

// 0x6xbb - Load bb into V[x]
void op6xxx(Chip8 *chip8) {
  Byte x = (chip8->opcode & 0x0F00) >> 8;
  chip8->V[x] = chip8->opcode & 0x00FF;
  if (chip8->debugFlag) printf("Loaded %d into V[0x%x]\n", chip8->V[x], x);
  chip8->pc += 2;
}

// 0x7xbb - Increment V[x] by bb
void op7xxx(Chip8 *chip8) {
  Byte x = (chip8->opcode & 0x0F00) >> 8;
  if (chip8->debugFlag) printf("Incrementing V[%d] by %d\n", x, chip8->opcode & 0x00FF);
  chip8->V[x] += chip8->opcode & 0x00FF;
  chip8->pc += 2;
}

void op8xxx(Chip8 *chip8) {
  Byte x = (chip8->opcode & 0x0F00) >> 8;
  Byte y = (chip8->opcode & 0x00F0) >> 4;
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
}
