#include "chip8.h"
#include "buzzer.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int virtualKeys[] = { 
  GLFW_KEY_1, // 0
  GLFW_KEY_2, // 1
  GLFW_KEY_3, // 2
  GLFW_KEY_4, // 3
  GLFW_KEY_Q, // 4
  GLFW_KEY_W, // 5
  GLFW_KEY_E, // 6
  GLFW_KEY_R, // 7
  GLFW_KEY_A, // 8
  GLFW_KEY_S, // 9
  GLFW_KEY_D, // A
  GLFW_KEY_F, // B
  GLFW_KEY_Z, // C
  GLFW_KEY_X, // D
  GLFW_KEY_C, // E
  GLFW_KEY_V, // F
};

Byte fontset[80] = { 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void emulateCycle(Chip8 *chip8);

Chip8 chipInitialize() {
  Chip8 chip8 = (Chip8){
    .I  = 0,
    .pc = 0x200,
    .sp = 0,
    .delayTimer = 0,
    .soundTimer = 0,
    .opcode = 0
  };
  chip8.memory = calloc(MEMORY, sizeof(Byte));
  for (int i = 0; i < 80; i++) {
    chip8.memory[i] = fontset[i];
  }
  chip8.display = calloc(DISPLAY_WIDTH * DISPLAY_HEIGHT, sizeof(Byte));
  chip8.screen = screenInit("../vertexShader.glsl", "../fragmentShader.glsl", DISPLAY_WIDTH, DISPLAY_HEIGHT, chip8.display);
  chip8.buzzer = buzzerInit();
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

void chipStartMainLoop(Chip8 *chip8, unsigned instructionFrequency) {
  float lastTime, currentTime, elapsedTime, deltaTime = 0;
  Byte soundPlaying = 0;
  while (!glfwWindowShouldClose(chip8->screen.window)) {
    currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    elapsedTime += deltaTime;
    lastTime = glfwGetTime();

    // Emulation Cycle
    /*if (chip8->soundTimer > 0)*/
    /*  buzzerPlay(&chip8->buzzer);*/
    /*else*/
    /*  buzzerStop(&chip8->buzzer);*/
    chip8->soundTimer = chip8->soundTimer >= 0 ? chip8->soundTimer - 1 : 0;
    chip8->delayTimer = chip8->delayTimer >= 0 ? chip8->delayTimer - 1 : 0;
    if (elapsedTime < DISPLAY_FREQUENCY) continue;
    chipTick(chip8, instructionFrequency);
    elapsedTime = 0;
    printf("Sound Timer: %d\n", chip8->soundTimer);

    // Display Debugger
    /*for (int y = 0; y < DISPLAY_HEIGHT; y++) {*/
    /*  for (int x = 0; x < DISPLAY_WIDTH; x++) {*/
    /*    printf("%d", chip8.display[y * 64 + x]);*/
    /*  }*/
    /*  printf("\n");*/
    /*}*/
    
    // Clear Commands
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

    // Draw Commands
    glBindVertexArray(chip8->screen.VAO);
    shaderUse(chip8->screen.shader);
    shaderSetInt(chip8->screen.shader, "texSample", 0);
    glBindTexture(GL_TEXTURE_2D, chip8->screen.texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, GL_RED, GL_UNSIGNED_BYTE, chip8->display);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Poll Events & Swap Buffers
    glfwPollEvents();
    glfwSwapBuffers(chip8->screen.window);
  }
}

void chipTick(Chip8 *chip8, int steps) {
  for (int i = 0; i < steps; i++) {
    emulateCycle(chip8);
  }
}

void emulateCycle(Chip8 *chip8) {
  Byte x, y;

  chip8->opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
  /*printf("Reading memory[0x%.3x] = 0x%.2x and memory[0x%.3x] = 0x%.2x\n", chip8->pc, chip8->memory[chip8->pc], chip8->pc + 1, chip8->memory[chip8->pc + 1]);*/
  /*printf("Opcode: 0x%.4x\n", chip8->opcode);*/

  // Decodes registers from the opcode
  x = (chip8->opcode & 0x0F00) >> 8; 
  y = (chip8->opcode & 0x00F0) >> 4;

  // Process input before decoding
  chipProcessInput(chip8);

  // Decode Instructions
  switch (chip8->opcode & 0xF000) {
    case 0x0000:
      switch (chip8->opcode) {
        // 0x00E0 - Clear Screen
        case 0x00E0:
          /*printf("Clearing Display\n");*/
          free(chip8->display);
          chip8->display = calloc(DISPLAY_WIDTH * DISPLAY_HEIGHT, sizeof(Byte));
          chip8->pc += 2;
          break;
        // 0x00EE - Return
        case 0x00EE:
          chip8->pc = chip8->stack[--chip8->sp] + 2;
          /*printf("Returning to 0x%.3x\n", chip8->pc);*/
          break;
      }
      break;
    // 0x1nnn - Jump to address nnn
    case 0x1000:
      chip8->pc = chip8->opcode & 0x0FFF;
      /*printf("Setting PC to: %d\n", chip8->pc);*/
      break;
    // 0x2nnn - Call function at nnn
    case 0x2000:
      chip8->stack[chip8->sp++] = chip8->pc;
      chip8->pc = chip8->opcode & 0x0FFF;
      /*printf("Calling function at 0x%.3x\n", chip8->pc);*/
      break;
    // 0x3xbb - Skip next instruction if V[x] == bb
    case 0x3000:
      if (chip8->V[x] == (chip8->opcode & 0x00FF))
        chip8->pc += 2;
      chip8->pc += 2;
      break;
    // 0x4xbb - Skip next instruction if V[x] != bb
    case 0x4000:
      if (chip8->V[x] != (chip8->opcode & 0x00FF))
        chip8->pc += 2;
      chip8->pc += 2;
      break;
    // 0x5xy0 - Skip next instruction if V[x] == V[y]
    case 0x5000:
      if (chip8->V[x] == chip8->V[y])
        chip8->pc += 2;
      chip8->pc += 2;
      break;
    // 0x6xbb - Load bb into V[x]
    case 0x6000:
      chip8->V[x] = chip8->opcode & 0x00FF;
      /*printf("Loaded %d into V[0x%x]\n", chip8->V[x], x);*/
      chip8->pc += 2;
      break;
    // 0x7xbb - Increment V[x] by bb
    case 0x7000:
      /*printf("Incrementing V[%d] by %d\n", x, chip8->opcode & 0x00FF);*/
      chip8->V[x] += chip8->opcode & 0x00FF;
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
        chip8->pc += 2;
      chip8->pc += 2;
      break;
    // 0xAnnn - Load nnn into I
    case 0xA000:
      chip8->I = chip8->opcode & 0x0FFF;
      /*printf("Loaded %d into I\n", chip8->I);*/
      /*printf("memory[I] = 0x%.2x\n", chip8->memory[chip8->I]);*/
      chip8->pc += 2;
      break;
    // 0xBnnn - Jump to address nnn + V[0]
    case 0xB000:
      chip8->pc = chip8->V[0] + chip8->opcode & 0x0FFF;
      /*printf("Jumped to address 0x%.3x\n", chip8->pc);*/
      break;
    // 0xCxbb - Set V[x] = rand(0, 255) AND bb
    case 0xC000:
      srand(time(NULL));
      chip8->V[x] = (rand() % 256) & (chip8->opcode & 0x00FF);
      /*printf("Generated random number in V[0x%x]: 0x%.2x\n", x, chip8->V[x]);*/
      chip8->pc += 2;
      break;
    // 0xDxyn - Draw a sprite of n-bytes high at (V[x], V[y])
    case 0xD000: {
      /*printf("Drawing:\n");*/
      Byte spriteRow;
      Byte height = chip8->opcode & 0x000F;
      chip8->V[0xF] = 0;
      for (int i = 0; i < height; i++) {
        spriteRow = chip8->memory[chip8->I + i];
        for (int j = 0; j < 8; j++) {
          unsigned index = chip8->V[x] + j + ((DISPLAY_HEIGHT - 1 - (chip8->V[y] + i)) * DISPLAY_WIDTH);
          Byte pixel = (spriteRow & (0x80 >> j)) > 0 ? 1 : 0;
          /*printf("%d", pixel);*/
          if (chip8->display[index] == 1)
            chip8->V[0xF] = 1;
          chip8->display[index] ^= pixel;
        }
        /*printf("\n");*/
      }
      chip8->pc += 2;
      break;
    case 0xE000:
        switch (chip8->opcode & 0x00FF) {
          // 0xEx9E - Skip next instruction if the key value of V[x] is pressed
          case 0x009E:
            if (chip8->key[chip8->V[x]])
              chip8->pc += 2;
            chip8->pc += 2;
            break;
          // 0xExA1 - Skip next instruction if the key value of V[x] is NOT pressed
          case 0x00A1:
            if (!chip8->key[chip8->V[x]])
              chip8->pc += 2;
            chip8->pc += 2;
            break;
        }
      break;
    case 0xF000:
        switch (chip8->opcode & 0x00FF) {
          // 0xFx07 - Set V[x] = delayTimer
          case 0x0007:
            chip8->V[x] = chip8->delayTimer;
            chip8->pc += 2;
            break;
          // 0xFx0A - Wait for input and store the key value in V[x]
          case 0x000A:
            /*printf("Waiting for input...\n");*/
            if (chip8->keyPressed < 0) 
              break;
            chip8->V[x] = chip8->keyPressed;
            /*printf("Key 0x%.1x Pressed\n", chip8->V[x]);*/
            chip8->pc += 2;
            break;
          // 0xFx15 - Set delayTimer = V[x]
          case 0x0015:
            chip8->delayTimer = chip8->V[x];
            chip8->pc += 2;
            break;
          // 0xFx18 - Set soundTimer = V[x]
          case 0x0018:
            chip8->soundTimer = chip8->V[x];
            chip8->pc += 2;
            break;
          // 0xFx1E - Set I = I + V[x]
          case 0x001E:
            chip8->I += chip8->V[x];
            chip8->pc += 2;
            break;
          // 0xFx29 - Set I equal to the memory address of the font-sprite for the value in V[x]
          case 0x0029:
            chip8->I = chip8->V[x] * 5;
            chip8->pc += 2;
            break;
          // 0xFx33 - Store BCD representation of V[x] at memory locations I, I + 1, I + 2
          case 0x0033:
            /*printf("V[%.1x] = %d\n", x, chip8->V[x]);*/
            chip8->memory[chip8->I] = chip8->V[x] / 100;
            chip8->memory[chip8->I + 1] = (chip8->V[x] % 100) / 10;
            chip8->memory[chip8->I + 2] = chip8->V[x] % 10;
            /*printf("memory[0x%.3x] = %d\n", chip8->I, chip8->memory[chip8->I]);*/
            /*printf("memory[0x%.3x] = %d\n", chip8->I + 1, chip8->memory[chip8->I + 1]);*/
            /*printf("memory[0x%.3x] = %d\n", chip8->I + 2, chip8->memory[chip8->I + 2]);*/
            chip8->pc += 2;
            break;
          // 0xFx55 - Store values from registers V[0] to V[x] into memory[I] onwards
          case 0x0055:
            for (int i = 0; i <= x; i++)
              chip8->memory[chip8->I + i] = chip8->V[i]; 
            chip8->pc += 2;
            break;
          // 0xFx55 - Store values starting from memory[I] into registers V[0] to V[x]
          case 0x0065:
            for (int i = 0; i <= x; i++)
              chip8->V[i] = chip8->memory[chip8->I + i]; 
            chip8->pc += 2;
            break;
      break;
      }
    }
  }
}

void chipProcessInput(Chip8 *chip8) {
  chip8->keyPressed = -1;
  for (int i = 0; i < 16; i++) {
    if (glfwGetKey(chip8->screen.window, virtualKeys[i]) == GLFW_PRESS) {
      chip8->key[i] = 1;
      chip8->keyPressed = i;
    } else {
      chip8->key[i] = 0;
    }
  }
}
