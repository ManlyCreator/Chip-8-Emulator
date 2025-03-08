#define Byte unsigned char
#define Word unsigned short

typedef struct {
  Word memory[4098];
  Byte V[16];
  Word I;

  Word pc;
  Word stack[16];
  Byte sp;

  Word opcode;
} Chip8;

Chip8 chipInitialize();
int chipLoadROM(Chip8 *chip8, const char *romPath);
void chipEmulateCycle(Chip8 *chip8);
