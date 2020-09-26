#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <psxetc.h>
#include <psxgte.h>
#include <psxgpu.h>
#include <psxapi.h>
#include <psxpad.h>
#include <psxsio.h>
#include <psxspu.h>
#include <psxcd.h>

#define false 0
#define true 1
typedef unsigned char bool;

extern unsigned char canvas_data[3][8192];

extern unsigned short int opcode;
extern unsigned char memory[0x10000];
extern unsigned char SV[8];
extern unsigned char V[16];
extern unsigned short int I;
extern unsigned short int pc;
extern short int delay_timer;
extern short int sound_timer;
extern unsigned short int stack[16];
extern unsigned char sp;
extern unsigned char keys[16];
extern bool drawFlag;

extern bool paused;
extern bool playing;
extern bool extendedScreen;

extern bool xochip;
extern unsigned char pattern[16];
extern unsigned char plane;

extern unsigned char screen_width;
extern unsigned char screen_height;
extern unsigned short int pixel_number;

extern unsigned char game_data[3584];
extern unsigned char controlMap[16];

void loadFontset(void);
void initialize(void);
void loadProgram(unsigned char *data, int fileSize);
void emulateCycle(unsigned char steps);
