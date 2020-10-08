#include <lzp.h>
#include "helper.h"
#include "chip8.h"

/* OT and Packet Buffer sizes */
#define OT_LEN			256
#define PACKET_LEN		1024

/* Double buffer structure */
typedef struct {
	DISPENV	disp;			/* Display environment */
	DRAWENV	draw;			/* Drawing environment */
	int 	ot[OT_LEN];		/* Ordering table */
	char 	p[PACKET_LEN];	/* Packet buffer */
} DB;

int SCREEN_SCALE = 2;
int cpf = 20;

int quit = 0;
int mode = 2;

DB db[2];
int db_active = 0;
char *db_nextpri;

char pad_buff[2][34];

TIM_IMAGE screen_tim;

unsigned int* screen_sprite;
extern unsigned int resources[];
char fileNames[64][17];

int debugMode = 0;


// Init function
void init(void)
{
	// This not only resets the GPU but it also installs the library's
	// ISR subsystem to the kernel
	ResetGraph(0);
	
	// Define display environments, first on top and second on bottom
	SetDefDispEnv(&db[0].disp, 0, 0, 320, 240);
	SetDefDrawEnv(&db[0].draw, 0, 240, 320, 240);
	
	
	// Define drawing environments, first on bottom and second on top
	SetDefDispEnv(&db[1].disp, 0, 240, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 0, 320, 240);
	
	// Set and enable clear color
	setRGB0(&db[0].draw, 69, 4, 20);
	setRGB0(&db[1].draw, 69, 4, 20);
	db[0].draw.isbg = 1;
	db[1].draw.isbg = 1;
	
	// Clear double buffer counter
	db_active = 0;
	
	// Apply the GPU environments
	PutDispEnv(&db[db_active].disp);
	PutDrawEnv(&db[db_active].draw);
	
	/* Clear both ordering tables to make sure they are clean at the start */
	ClearOTagR( db[0].ot, OT_LEN );
	ClearOTagR( db[1].ot, OT_LEN );
	
	db_nextpri = db[0].p;
	
	// Load test font
	FntLoad(960, 0);
	
	// Open up a test font text stream of 100 characters
	FntOpen(0, 8, 320, 224, 0, 100);
	
	GetTimInfo(screen_sprite, &screen_tim);
	*(screen_tim.caddr) = 0xffff8000;
	
	LoadImage(screen_tim.prect, screen_tim.paddr);
	DrawSync(0);
	
	LoadImage(screen_tim.crect, screen_tim.caddr);
	DrawSync(0);
	
	// Init BIOS pad driver and set pad buffers (buffers are updated
	// automatically on every V-Blank)
	InitPAD(&pad_buff[0][0], 34, &pad_buff[1][0], 34);
	
	// Start pad
	StartPAD();
	
	// Don't make pad driver acknowledge V-Blank IRQ (recommended)
	ChangeClearPAD(0);
}

// Display function
void display(void)
{
	
	// Wait for all drawing to complete
	DrawSync(0);
	
	// Wait for vertical sync to cap the logic to 60fps (or 50 in PAL mode)
	// and prevent screen tearing
	VSync(0);
	
	// Flip buffer index
	db_active ^= 1;
	db_nextpri = db[db_active].p;
	
	ClearOTagR( db[db_active].ot, OT_LEN );

	// Switch pages	
	PutDispEnv(&db[db_active].disp);
	PutDrawEnv(&db[db_active].draw);
	
	// Enable display output, ResetGraph() disables it by default
	SetDispMask(1);
	
	/* Start drawing the OT of the last buffer */
	DrawOTag(db[1-db_active].ot+(OT_LEN-1));
	
}

void loadProgramPsx(int index) {
	unsigned char* rom;
	int fileSize = 0;
	index++;
	
	// Get the filesize of the rom
	fileSize = lzpFileSize(resources, index);
	rom = (unsigned char*)malloc(fileSize);
	// Put the file's data into "rom"
	lzpUnpackFile(rom, resources, index);
	// Load the rom into CHIP-84
	loadProgram(rom, fileSize);
	// Clear the screen
	memset(screen_tim.paddr, 0, 8192);
}

// Set keypad flags for menus
void menuKeypad(PADTYPE* pad) {
	if(pad->stat == 0) {
		if((pad->type == 0x4) || (pad->type == 0x5) || (pad->type == 0x7)) {
			if(!(pad->btn&PAD_UP)) {
				if(keys[0x1] == 0)
					keys[0x1] = 1;
				else if(keys[0x1] == 1)
					keys[0x1] = 2;
			} else {
				keys[0x1] = 0;
			}
			if(!(pad->btn&PAD_DOWN)) {
				if(keys[0x2] == 0)
					keys[0x2] = 1;
				else if(keys[0x2] == 1)
					keys[0x2] = 2;
			} else {
				keys[0x2] = 0;
			}
			if(!(pad->btn&PAD_LEFT)) {
				if(keys[0x3] == 0)
					keys[0x3] = 1;
				else if(keys[0x3] == 1)
					keys[0x3] = 2;
			} else {
				keys[0x3] = 0;
			}
			if(!(pad->btn&PAD_RIGHT)) {
				if(keys[0xc] == 0)
					keys[0xc] = 1;
				else if(keys[0xc] == 1)
					keys[0xc] = 2;
			} else {
				keys[0xc] = 0;
			}
			if(!(pad->btn&PAD_TRIANGLE)) {
				if(keys[0x4] == 0)
					keys[0x4] = 1;
				else if(keys[0x4] == 1)
					keys[0x4] = 2;
			} else {
				keys[0x4] = 0;
			}
			if(!(pad->btn&PAD_CROSS)) {
				if(keys[0x5] == 0)
					keys[0x5] = 1;
				else if(keys[0x5] == 1)
					keys[0x5] = 2;
			} else {
				keys[0x5] = 0;
			}
			if(!(pad->btn&PAD_SQUARE)) {
				if(keys[0x6] == 0)
					keys[0x6] = 1;
				else if(keys[0x6] == 1)
					keys[0x6] = 2;
			} else {
				keys[0x6] = 0;
			}
			if(!(pad->btn&PAD_CIRCLE)) {
				if(keys[0xd] == 0)
					keys[0xd] = 1;
				else if(keys[0xd] == 1)
					keys[0xd] = 2;
			} else {
				keys[0xd] = 0;
			}
			if(!(pad->btn&PAD_R1)) {
				if(keys[0x7] == 0)
					keys[0x7] = 1;
				else if(keys[0x7] == 1)
					keys[0x7] = 2;
			} else {
				keys[0x7] = 0;
			}
			if(!(pad->btn&PAD_R2)) {
				keys[0x8] = 1;
			} else {
				keys[0x8] = 0;
			}
			if(!(pad->btn&PAD_L1)) {
				keys[0x9] = 1;
			} else {
				keys[0x9] = 0;
			}
			if(!(pad->btn&PAD_L2)) {
				keys[0xe] = 1;
			} else {
				keys[0xe] = 0;
			}
			if(!(pad->btn&PAD_SELECT)) {
				
			}
			if(!(pad->btn&PAD_START)) {
				
			}
		}
	}
}

int main(int argc, char* argv[]) {
	POLY_FT4 *quad;
	PADTYPE *pad;
	
	int i;
	int fileSize = 0;
	int fileCount = 0;
	int frame = 0;
	int frameRef = 0;
	
	// Load the screen_sprite tim file
	i = lzpSearchFile("screen_sprite", resources);
	screen_sprite = (unsigned int*)malloc(lzpFileSize(resources, i));
	lzpUnpackFile(screen_sprite, resources, i);
	
	// Add rom file names to fileNames list
	for(i = 1; i < 128; i++) {
		LZP_FILE* lzpFile;
		// If file exists, add it to list, else break the loop
		if(lzpFile = lzpFileEntry(resources, i))
			strcpy(fileNames[i-1], lzpFile->fileName);
		else
			break;
		fileCount++;
	}
	
	init();
	
	loadProgramPsx(0);
	
	int sx = 32, sy = 52;
	int selection = 0;
	int fileSelection = 0;
	
	int fntStream = FntOpen(0, 10, 320, 224, 0, 1024);
	
	memset(screen_tim.paddr, 0, 8192);
	
	while(!quit) {
		srand(frame);
		pad = (PADTYPE*)&pad_buff[0][0];
		
		switch(mode) {
		case 0: {					// Emulation screen
			// Resize screen sprite to current CHIP-8 screen size
			screen_tim.prect->w = screen_height;
			screen_tim.prect->h = screen_height;
			
			if(pad->stat == 0) {
				if((pad->type == 0x4) || (pad->type == 0x5) || (pad->type == 0x7)) {
					if(!(pad->btn&PAD_UP)) {
						keys[0x1] = 1;
					} else {
						keys[0x1] = 0;
					}
					if(!(pad->btn&PAD_DOWN)) {
						keys[0x2] = 1;
					} else {
						keys[0x2] = 0;
					}
					if(!(pad->btn&PAD_LEFT)) {
						keys[0x3] = 1;
					} else {
						keys[0x3] = 0;
					}
					if(!(pad->btn&PAD_RIGHT)) {
						keys[0xc] = 1;
					} else {
						keys[0xc] = 0;
					}
					if(!(pad->btn&PAD_TRIANGLE)) {
						keys[0x4] = 1;
					} else {
						keys[0x4] = 0;
					}
					if(!(pad->btn&PAD_CROSS)) {
						keys[0x5] = 1;
					} else {
						keys[0x5] = 0;
					}
					if(!(pad->btn&PAD_SQUARE)) {
						keys[0x6] = 1;
					} else {
						keys[0x6] = 0;
					}
					if(!(pad->btn&PAD_CIRCLE)) {
						keys[0xd] = 1;
					} else {
						keys[0xd] = 0;
					}
					if(!(pad->btn&PAD_R1)) {
						keys[0x7] = 1;
					} else {
						keys[0x7] = 0;
					}
					if(!(pad->btn&PAD_R2)) {
						keys[0x8] = 1;
					} else {
						keys[0x8] = 0;
					}
					if(!(pad->btn&PAD_L1)) {
						keys[0x9] = 1;
					} else {
						keys[0x9] = 0;
					}
					if(!(pad->btn&PAD_L2)) {
						keys[0xe] = 1;
					} else {
						keys[0xe] = 0;
					}
					if(!(pad->btn&PAD_SELECT)) {
						
					}
					if(!(pad->btn&PAD_START)) {
						mode = 3;
						selection = 0;
						break;
					}
				}
				
				if((pad->type == 0x5) || (pad->type == 0x7)) {
					if((pad->ls_y-128) < -16) {
						keys[0xa] = 1;
					} else {
						keys[0xa] = 0;
					}
					if((pad->ls_y-128) >  16) {
						keys[0x0] = 1;
					} else {
						keys[0x0] = 0;
					}
					if((pad->ls_x-128) < -16) {
						keys[0xb] = 1;
					} else {
						keys[0xb] = 0;
					}
					if((pad->ls_x-128) >  16) {
						keys[0xf] = 1;
					} else {
						keys[0xf] = 0;
					}
					if(!(pad->btn&PAD_R3)) {
						debugMode = -1;
					} else {
						if(debugMode == -1) debugMode = 1;
					}
				}
			}
			
			emulateCycle(cpf);
			
			DrawSync(0);
			// Load screen sprite into the tim from memory
			LoadImage(screen_tim.prect, screen_tim.paddr);
			
			if(drawFlag) {
				// Copy canvas data to the screen sprite's pixel data
				memcpy(screen_tim.paddr, canvas_data, pixel_number);
			}
			
			// Draw screen quad
			quad = (POLY_FT4*)db_nextpri;
			setPolyFT4(quad);
			setXY4(quad, sx, sy, sx+127*SCREEN_SCALE, sy, sx, sy+63*SCREEN_SCALE, sx+127*SCREEN_SCALE, sy+63*SCREEN_SCALE);
			setRGB0(quad, 128, 128, 128);
			quad->tpage = getTPage(screen_tim.mode, 0, screen_tim.prect->x, screen_tim.prect->y);
			setClut(quad, screen_tim.crect->x, screen_tim.crect->y);
			setUVWH(quad, 0, 0, screen_width-1, screen_height-1);
			
			addPrim(db[db_active].ot, quad);
			
			
			if(debugMode == 1) {
				int sfState = 0;
				int btnState = 0;
				while(sfState != 2) {
					pad = (PADTYPE*)&pad_buff[0][0];
					if(pad->stat == 0) {
						if((pad->type == 0x4) || (pad->type == 0x5) || (pad->type == 0x7)) {
							if(!(pad->btn&PAD_START)) {
								mode = 3;
								selection = 0;
								sfState = 2;
								break;
							}
							if(!(pad->btn&PAD_SELECT)) {
								sfState = 1;
							} else {
								if(sfState == 1) sfState = 2;
							}
						}
						if((pad->type == 0x5) || (pad->type == 0x7)) {
							if(!(pad->btn&PAD_R3)) {
								btnState = 1;
							} else {
								if(btnState == 1) btnState = 2;
							}
						}
					}
					FntPrint(fntStream, "OP: %04x PC: %04x\n", opcode, pc);
					FntPrint(fntStream, "V0:%02x V1:%02x V2:%02x V3:%02x V4:%02x V5:%02x\nV6:%02x V7:%02x V8:%02x V9:%02x VA:%02x VB:%02x\nVC:%02x VD:%02x VE:%02x VF:%02x", V[0],V[1],V[2],V[3],V[4],V[5],V[6],V[7],V[8],V[9],V[10],V[11],V[12],V[13],V[14],V[15]);
					FntFlush(fntStream);
					
					memcpy(screen_tim.paddr, canvas_data, pixel_number);
					LoadImage(screen_tim.prect, screen_tim.paddr);
					
					if(btnState == 2) {
						debugMode = 0;
						sfState = 2;
						break;
					}
				}
			} else {
				FntPrint(fntStream, "\n %s", fileNames[fileSelection]);
				FntFlush(fntStream);
			}
			
			break;
		}
		case 1: {					// ROM file selection screen
			int tmp = keys[0x5];
			menuKeypad(pad);
			
			if(keys[0x1] == 1) {
				frameRef = frame;
				selection--;
			} else if(keys[0x2] == 1) {
				frameRef = frame;
				selection++;
			}
			if(keys[0x1] == 2) {
				if(frame - frameRef > 10 && frame % 2 == 0) {
					selection--;
				}
			} else if(keys[0x2] == 2) {
				if(frame - frameRef > 10 && frame % 2 == 0) {
					selection++;
				}
			}
			if(selection < 0) selection = fileCount - 1;
			if(selection > fileCount - 1) selection = 0;
			
			if(keys[0x5] == 0 && tmp == 2) {
				loadProgramPsx(selection);
				fileSelection = selection;
				mode = 0;
				break;
			}
			if(keys[0xd] == 1) {
				mode = 3;
				selection = 0;
				break;
			}
			
			FntPrint(fntStream, "\n CHOOSE A ROM\n\n");
			for(i = 0; i < fileCount; i++) {
				if(selection - i < 20) {
					char fileNameBuffer[20];
					if(selection == i) {
						sprintf(fileNameBuffer, " > %s", fileNames[i]);
					} else {
						sprintf(fileNameBuffer, "   %s", fileNames[i]);
					}
					FntPrint(fntStream, "%s\n", fileNameBuffer);
				}
			}
			FntFlush(fntStream);
			
			break;
		}
		case 2: {					// Splash screen
			FntPrint(fntStream, "\n\n\n\n\n\n\n\n\n\n\n\n           CHIP-84 PSX EDITION\n\n");
			FntPrint(fntStream, "          2020 CHRISTIAN KOSMAN\n");
			FntFlush(fntStream);
			if(frame > 120) mode = 3;
			break;
		}
		case 3: {					// Main menu screen
			int tmp = keys[0x5];
			menuKeypad(pad);
			
			if(keys[0x1] == 1) {
				frameRef = frame;
				selection--;
			} else if(keys[0x2] == 1) {
				frameRef = frame;
				selection++;
			}
			if(keys[0x1] == 2) {
				if(frame - frameRef > 10 && frame % 2 == 0) {
					selection--;
				}
			} else if(keys[0x2] == 2) {
				if(frame - frameRef > 10 && frame % 2 == 0) {
					selection++;
				}
			}
			if(selection < 0) selection = 3;
			if(selection > 3) selection = 0;
			
			if(keys[0x5] == 0 && tmp == 2) {
				if(selection == 0) {
					mode = 0;
					break;
				} else if(selection == 1) {
					selection = 0;
					mode = 1;
					break;
				} else if(selection == 3) {
					debugMode ^= 1;
					//mode = 0;
					cpf = 1;
					break;
				}
			}
			
			
			FntPrint(fntStream, "\n MAIN MENU\n\n");
			char menuItems[4][32] = {
				"RESUME",
				"CHOOSE A ROM...",
				"CYCLES PER FRAME: ",
				"DEBUG MODE: OFF"
			};
			if(debugMode)
				strcpy(menuItems[3], "DEBUG MODE: ON ");
			for(i = 0; i < 4; i++) {
				char menuItemBuffer[20];
				if(i == 2) {
					if(selection == 2) {
						if(keys[0x3] == 1) {
							frameRef = frame;
							cpf--;
						} else if(keys[0xc] == 1) {
							frameRef = frame;
							cpf++;
						}
						if(keys[0x3] == 2) {
							if(frame - frameRef > 10 && frame % 2 == 0) {
								cpf--;
							}
						} else if(keys[0xc] == 2) {
							if(frame - frameRef > 10 && frame % 2 == 0) {
								cpf++;
							}
						}
					}
					if(cpf < 1) cpf = 100;
					if(cpf > 100) cpf = 1;
					if(selection == i) {
						sprintf(menuItemBuffer, " > %s%d", menuItems[i], cpf);
					} else {
						sprintf(menuItemBuffer, "   %s%d", menuItems[i], cpf);
					}
				} else {
					if(selection == i) {
						sprintf(menuItemBuffer, " > %s", menuItems[i]);
					} else {
						sprintf(menuItemBuffer, "   %s", menuItems[i]);
					}
				}
				FntPrint(fntStream, "%s\n", menuItemBuffer);
			}
			
			FntFlush(fntStream);
			break;
		}
		}
		
		display();
		frame++;
	}
	
	return 0;
}
