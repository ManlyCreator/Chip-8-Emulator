#include <GLFW/glfw3.h>

#define MEMORY 4096
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

#define Byte unsigned char
#define Word unsigned short

typedef struct {
  // Memory
  Byte *memory;
  Byte V[16];
  Byte key[16];
  Word I;

  // Display
  Byte *display;
  GLFWwindow *window;

  // State
  Word pc;
  Word stack[16];
  Byte sp;
  Byte keyPressed;

  // Timers
  Byte delayTimer;
  Byte soundTimer;

  Word opcode;
} Chip8;

Chip8 chipInitialize(GLFWwindow *window);
int chipLoadROM(Chip8 *chip8, const char *romPath);
void chipEmulateCycle(Chip8 *chip8);
void chipProcessInput(Chip8 *chip8, GLFWwindow *window);
