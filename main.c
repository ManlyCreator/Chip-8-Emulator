#include <stdio.h>
#include <stdlib.h>

// TODO: Implement simple OpenGL window
// TODO: Start working on instruction decoding
#define Byte char

unsigned char memory[4096];
unsigned char V[16];

int main(int argc, char **argv) {
  Byte *instruction = malloc(sizeof(Byte));
  FILE *bFile = fopen("../roms/eaty.ch8", "rb");

  if (!bFile) {
    printf("Could not open file\n");
    return -1;
  }

  fread(instruction, sizeof(Byte), 1, bFile);
  printf("Instruction: %x\n", *instruction);

  return 0;
}
