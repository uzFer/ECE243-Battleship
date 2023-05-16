/* This files provides address values that exist in the system */
#define BOARD "DE1-SoC"
/* Memory */
#define DDR_BASE 0x00000000
#define DDR_END 0x3FFFFFFF
#define A9_ONCHIP_BASE 0xFFFF0000
#define A9_ONCHIP_END 0xFFFFFFFF
#define SDRAM_BASE 0xC0000000
#define SDRAM_END 0xC3FFFFFF
#define FPGA_ONCHIP_BASE 0xC8000000
#define FPGA_ONCHIP_END 0xC803FFFF
#define FPGA_CHAR_BASE 0xC9000000
#define FPGA_CHAR_END 0xC9001FFF
/* Cyclone V FPGA devices */
#define LEDR_BASE 0xFF200000
#define HEX3_HEX0_BASE 0xFF200020
#define HEX5_HEX4_BASE 0xFF200030
#define SW_BASE 0xFF200040
#define KEY_BASE 0xFF200050
#define JP1_BASE 0xFF200060
#define JP2_BASE 0xFF200070
#define PS2_BASE 0xFF200100
#define PS2_DUAL_BASE 0xFF200108
#define JTAG_UART_BASE 0xFF201000
#define JTAG_UART_2_BASE 0xFF201008
#define IrDA_BASE 0xFF201020
#define TIMER_BASE 0xFF202000
#define AV_CONFIG_BASE 0xFF203000
#define PIXEL_BUF_CTRL_BASE 0xFF203020
#define CHAR_BUF_CTRL_BASE 0xFF203030
#define AUDIO_BASE 0xFF203040
#define VIDEO_IN_BASE 0xFF203060
#define ADC_BASE 0xFF204000
#define RLEDs ((volatile long *) 0xFF200000)
/* Cyclone V HPS devices */
#define HPS_GPIO1_BASE 0xFF709000
#define HPS_TIMER0_BASE 0xFFC08000
#define HPS_TIMER1_BASE 0xFFC09000
#define HPS_TIMER2_BASE 0xFFD00000
#define HPS_TIMER3_BASE 0xFFD01000
#define FPGA_BRIDGE 0xFFD0501C
/* ARM A9 MPCORE devices */
#define PERIPH_BASE 0xFFFEC000 // base address of peripheral devices
#define MPCORE_PRIV_TIMER 0xFFFEC600 // PERIPH_BASE + 0x0600
/* Interrupt controller (GIC) CPU interface(s) */
#define MPCORE_GIC_CPUIF 0xFFFEC100 // PERIPH_BASE + 0x100
#define ICCICR 0x00 // offset to CPU interface control reg
#define ICCPMR 0x04 // offset to interrupt priority mask
#define ICCIAR 0x0C // offset to interrupt acknowledge reg
#define ICCEOIR 0x10 // offset to end of interrupt reg
/* Interrupt controller (GIC) distributor interface(s) */
#define MPCORE_GIC_DIST 0xFFFED000 // PERIPH_BASE + 0x1000
#define ICDDCR 0x00 // offset to distributor control reg
#define ICDISER 0x100 // offset to interrupt set-enable regs
#define ICDICER 0x180 // offset to interrupt clear-enable
#define ICDIPTR 0x800 // offset to interrupt processor
#define ICDICFR 0xC00 // offset to interrupt configuration
#define IRQ_MODE 0b10010
#define SVC_MODE 0b10011
#define INT_ENABLE 0b01000000
#define INT_DISABLE 0b11000000
#define ENABLE 0x1

/* VGA colors */
#define WHITE 0xFFFF
#define BLACK 0x0000
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x00FF
#define CYAN 0x0F1F
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00
	
#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240
	
/* SCREEN VARIABLES */
#define nullScreen -1
#define startScreen 0
#define P1SetUp 1
#define P2SetUp 2
#define gameStarts 3
#define waitForX 4
#define waitForY 5
#define gameOver 6
#define isChecking 7
#define isShipHit 8
#define isMiss 9
#define alreadyGuessed 10
#define timesUp 11
#define instructionScreen 12

#define KEYS_IRQ 73
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <math.h>
#include <stdbool.h>

// old functions from lab5
void swap(int* x, int *y);
void clear_screen();
void draw_line(int x0, int x1, int y0, int y1, short int line_color);
void plot_pixel(int x, int y, short int line_color);
void wait_for_vsync();

// new functions used for the game
void drawGrid(short int line_color);
void randomizeShips(int playerShips[10][10]);
void drawLocations(int playerShips[10][10], int offsetX, int offsetY, short int line_color);
void drawBox(int x, int y, short int line_color, int xMax, int yMax);
void drawTitle(short int line_color, short int line_color2);
void drawLetter(int x, int y, char letter, int scaleFactor, short int line_color);
void drawText(int x, int y, char * text, int scaleFactor, short int line_color);
void isHit(int x, int y, int opponentShips[10][10], int playerGuesses[10][10]);
bool allGuessed(int opponentShips[10][10], int playerGuesses[10][10]);
void clearScreenAnimation(int row);
int decipherSwitch(int coordinate);
void tostring(char * str, int num);
void highlightSquare(int currentRow, int currentCol, short int line_color, int offsetX, int offsetY);
void placeShip(int currentRow, int currentCol, int playerShips[10][10]);
void timerSetup(int timeVal);
void drawTimer(int timerVal);
void restartGrid();

void config_interrupt(int N, int CPU_target);
void __attribute__((interrupt)) __cs3_isr_irq(void);
void pushbutton_ISR(void);
void set_A9_IRQ_stack(void);
void A9_timer_ISR(void);
void keyboard_ISR(void);
void config_GIC(void);
void config_timer();
void config_keyboard();
void config_KEYs(void);
void enable_A9_interrupts(void);

volatile int pixel_buffer_start; // global variable
int colors[] = {WHITE, BLACK, PINK, ORANGE, GREEN, MAGENTA, BLUE, CYAN, GREY, RED, YELLOW};
int currentScreen = 0;
volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
volatile int * sw_ptr = (int *)0xFF200040;

bool p1Turn = false;
bool p2Turn = false;
bool p1Win = false;
bool p2Win = false;
bool isCoordError = false;
bool isError = false;
bool flicker = true;
bool isConfirming = false;

int flickerTime = 1;
int displayTextTime = 2;
int timeVal = 60;
int currentRow = 0;
int currentCol = 0;
int actualRow;
int actualCol;
int p1ShipCounter = 0;
int p2ShipCounter = 0;

int p1ShipLocations[10][10] = { {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
							  	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0} };

int p2ShipLocations[10][10] = { {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
							  	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0} };

int p1Guesses[10][10] =       { {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
							  	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0} };

int p2Guesses[10][10] =       { {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
							  	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
							    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0} };

int letterCodes[26][5][5] = {
	{{0,1,1,0,0}, // A
	 {1,0,0,1,0},
	 {1,1,1,1,0},
	 {1,0,0,1,0},
	 {1,0,0,1,0}},
	{{1,1,1,0,0}, // B
	 {1,0,0,1,0},
	 {1,1,1,0,0},
	 {1,0,0,1,0},
	 {1,1,1,0,0}},
	{{0,1,1,1,0}, // C
	 {1,0,0,0,0},
	 {1,0,0,0,0},
	 {1,0,0,0,0},
	 {0,1,1,1,0}},
	{{1,1,1,0,0}, // D
	 {1,0,0,1,0},
	 {1,0,0,1,0},
	 {1,0,0,1,0},
	 {1,1,1,0,0}},
	{{1,1,1,1,0}, // E
	 {1,0,0,0,0},
	 {1,1,1,1,0},
	 {1,0,0,0,0},
	 {1,1,1,1,0}},
	{{1,1,1,1,0}, // F
	 {1,0,0,0,0},
	 {1,1,1,1,0},
	 {1,0,0,0,0},
	 {1,0,0,0,0}},
	{{1,1,1,1,0}, // G
	 {1,0,0,0,0},
	 {1,0,1,1,0},
	 {1,0,0,1,0},
	 {1,1,1,1,0}},
	{{1,0,0,0,1}, // H uses 5x5
	 {1,0,0,0,1},
	 {1,1,1,1,1},
	 {1,0,0,0,1},
	 {1,0,0,0,1}},
	{{1,1,1,0,0}, // I
	 {0,1,0,0,0},
	 {0,1,0,0,0},
	 {0,1,0,0,0},
	 {1,1,1,0,0}},
	{{0,1,1,1,0}, // J
	 {0,0,0,1,0},
	 {0,0,0,1,0},
	 {1,0,0,1,0},
	 {1,1,1,1,0}},
	{{1,0,0,1,0}, // K
	 {1,0,1,0,0},
	 {1,1,0,0,0},
	 {1,0,1,0,0},
	 {1,0,0,1,0}},
	{{1,0,0,0,0}, // L
	 {1,0,0,0,0},
	 {1,0,0,0,0},
	 {1,0,0,0,0},
	 {1,1,1,1,0}},
	{{1,0,0,0,1}, // M uses 5x5
	 {1,1,0,1,1},
	 {1,0,1,0,1},
	 {1,0,0,0,1},
	 {1,0,0,0,1}},
	{{1,0,0,1,0}, // N
	 {1,1,0,1,0},
	 {1,0,1,1,0},
	 {1,0,0,1,0},
	 {1,0,0,1,0}},
	{{1,1,1,1,0}, // O
	 {1,0,0,1,0},
	 {1,0,0,1,0},
	 {1,0,0,1,0},
	 {1,1,1,1,0}},
	{{1,1,1,1,0}, // P
	 {1,0,0,1,0},
	 {1,1,1,1,0},
	 {1,0,0,0,0},
	 {1,0,0,0,0}},
	{{0,1,1,0,0}, // Q
	 {1,0,0,1,0},
	 {1,0,0,1,0},
	 {1,0,1,0,0},
	 {0,1,0,1,0}},
	{{1,1,1,0,0}, // R
	 {1,0,0,1,0},
	 {1,1,1,0,0},
	 {1,0,1,0,0},
	 {1,0,0,1,0}},
	{{1,1,1,1,0}, // S
	 {1,0,0,0,0},
	 {1,1,1,1,0},
	 {0,0,0,1,0},
	 {1,1,1,1,0}},
	{{1,1,1,1,1}, // T uses 5x5
	 {0,0,1,0,0},
	 {0,0,1,0,0},
	 {0,0,1,0,0},
	 {0,0,1,0,0}},
	{{1,0,0,1,0}, // U
	 {1,0,0,1,0},
	 {1,0,0,1,0},
	 {1,0,0,1,0},
	 {1,1,1,1,0}},
	{{1,0,0,0,1}, // V uses 5x5
	 {1,0,0,0,1},
	 {1,0,0,0,1},
	 {0,1,0,1,0},
	 {0,0,1,0,0}},
	{{1,0,0,0,1}, // W uses 5x5
	 {1,0,1,0,1},
	 {1,0,1,0,1},
	 {1,0,1,0,1},
	 {1,1,1,1,1}},
	{{1,0,0,0,1}, // X
	 {0,1,0,1,0},
	 {0,0,1,0,0},
	 {0,1,0,1,0},
	 {1,0,0,0,1}},
	{{1,0,0,0,1}, // Y uses 5x5
	 {1,0,0,0,1},
	 {0,1,1,1,0},
	 {0,0,1,0,0},
	 {0,0,1,0,0}},
	{{1,1,1,1,0}, // Z
	 {0,0,1,0,0},
	 {0,1,0,0,0},
	 {1,0,0,0,0},
	 {1,1,1,1,0}}};

int numberCodes[10][5][5] = {
	{{1,1,1,1,0}, // 0
	 {1,0,0,1,0},
	 {1,0,0,1,0},
	 {1,0,0,1,0},
	 {1,1,1,1,0}},
	{{0,0,1,0,0}, // 1
	 {0,1,1,0,0},
	 {0,0,1,0,0},
	 {0,0,1,0,0},
	 {0,1,1,1,0}},
	{{1,1,1,1,0}, // 2
	 {0,0,0,1,0},
	 {1,1,1,1,0},
	 {1,0,0,0,0},
	 {1,1,1,1,0}},
	{{1,1,1,1,0}, // 3
	 {0,0,0,1,0},
	 {1,1,1,1,0},
	 {0,0,0,1,0},
	 {1,1,1,1,0}},
	{{1,0,0,1,0}, // 4
	 {1,0,0,1,0},
	 {1,1,1,1,0},
	 {0,0,0,1,0},
	 {0,0,0,1,0}},
	{{1,1,1,1,0}, // 5
	 {1,0,0,0,0},
	 {1,1,1,1,0},
	 {0,0,0,1,0},
	 {1,1,1,1,0}},
	{{1,1,1,1,0}, // 6
	 {1,0,0,0,0},
	 {1,1,1,1,0},
	 {1,0,0,1,0},
	 {1,1,1,1,0}},
	{{1,1,1,1,0}, // 7
	 {0,0,0,1,0},
	 {0,0,0,1,0},
	 {0,0,0,1,0},
	 {0,0,0,1,0}},
	{{1,1,1,1,0}, // 8
	 {1,0,0,1,0},
	 {1,1,1,1,0},
	 {1,0,0,1,0},
	 {1,1,1,1,0}},
	{{1,1,1,1,0}, // 9
	 {1,0,0,1,0},
	 {1,1,1,1,0},
	 {0,0,0,1,0},
	 {1,1,1,1,0}}};

int randoCodes[10][5][5] = {
	{{0,0,0,0,0}, // .
	 {0,0,0,0,0},
	 {0,0,0,0,0},
	 {0,0,0,0,0},
	 {0,0,1,0,0}},
	{{0,0,0,0,1}, // /
	 {0,0,0,1,0},
	 {0,0,1,0,0},
	 {0,1,0,0,0},
	 {1,0,0,0,0}},
	{{0,0,0,0,0}, // :
	 {0,0,1,0,0},
	 {0,0,0,0,0},
	 {0,0,0,0,0},
	 {0,0,1,0,0}},
	{{0,0,1,0,0}, // !
	 {0,0,1,0,0},
	 {0,0,1,0,0},
	 {0,0,0,0,0},
	 {0,0,1,0,0}},
	{{0,0,1,0,0}, // up arrow
	 {0,1,1,1,0},
	 {0,0,1,0,0},
	 {0,0,1,0,0},
	 {0,0,1,0,0}},
	{{0,0,1,0,0}, // down arrow
	 {0,0,1,0,0},
	 {0,0,1,0,0},
	 {0,1,1,1,0},
	 {0,0,1,0,0}},
	{{0,0,0,0,0}, // left arrow
	 {0,1,0,0,0},
	 {1,1,1,1,1},
	 {0,1,0,0,0},
	 {0,0,0,0,0}},
	{{0,0,0,0,0}, // right arrow
	 {0,0,0,1,0},
	 {1,1,1,1,1},
	 {0,0,0,1,0},
	 {0,0,0,0,0}},
	{{0,0,0,0,0}, // checkmark
	 {0,0,0,0,1},
	 {0,0,0,1,0},
	 {1,0,1,0,0},
	 {0,1,0,0,0}},
	{{0,0,1,0,0}, // ship
	 {0,1,1,0,0},
	 {0,0,1,0,0},
	 {1,1,1,1,1},
	 {0,1,1,1,0}}};

unsigned char byte1 = 0;
unsigned char byte2 = 0;
unsigned char byte3 = 0;
int main(void) {
	int keyVal;
	//srand(time(0));
	
    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen(); // pixel_buffer_start points to the pixel buffer
	// adding
	set_A9_IRQ_stack(); // initialize the stack pointer for IRQ mode
	config_GIC(); // configure the general interrupt controller
	// interrupts
	config_timer();
	config_keyboard();
	config_KEYs(); // configure pushbutton KEYs to generate interrupts
	enable_A9_interrupts(); 
	
    while (1)
    {
		clear_screen();
		if(currentScreen != instructionScreen) drawGrid(colors[0]);

		/* START SCREEN */
		if(currentScreen == startScreen) { 
			char * intro = "by: uzma and raida";
			drawText(112, 48, intro, 1, colors[0]);
			
			char * startText = "press keys or spacebar to start game!";
			drawText(65, 62, startText, 1, colors[0]);
			
			intro = "*";
			drawText(25, 48, intro, 4, colors[6]);
		}
		
		if(currentScreen == instructionScreen) {
			char * instructions = "instructions";
			drawText(95, 12, instructions, 2, colors[0]);

			// step 1
			char * one = "1. two 10 x 10 grids are created for player one and two.";
			drawText(5, 35, one, 1, colors[0]);

			// step 2
			char * two1 = "2. each player takes turns placing five 1 x 1 ships in";
			drawText(5, 45, two1, 1, colors[0]);
			char * two2 = "random locations on their designated grids.";
			drawText(20, 55, two2, 1, colors[0]);

			// step 3
			char * three1 = "3. players then take turns guessing the location of the";
			drawText(5, 65, three1, 1, colors[0]);
			char * three2 = "opponents ships. if the guess is wrong an";
			drawText(20, 75, three2, 1, colors[0]);
			char * x = "x";
			drawText(237, 75, x, 1, colors[9]);
			char * three3 = "will be";
			drawText(248, 75, three3, 1, colors[0]);
			char * three4 = "placed on the chosen coordinate to indicate a miss.";
			drawText(20, 85, three4, 1, colors[0]);
			char * three5 = "if the guess is right a ";
			drawText(20, 95, three5, 1, colors[0]);
			char * checkmark = "checkmark";
			drawText(144, 95, checkmark, 1, GREEN);
			char * three6 = "will be placed on the";
			drawText(198, 95, three6, 1, colors[0]);
			char * three7 = "chosen coordinate to indicate a hit.";
			drawText(20, 105, three7, 1, colors[0]);

			// step 4
			char * four1 = "4. the first player to guess all five opponent ships win!";
			drawText(5, 115, four1, 1, colors[0]);
			char * four2 = "in the case of a tied game there is a draw.";
			drawText(20, 125, four2, 1, colors[0]);

			// continue to game setup
			char * gl = "good luck!";
			drawText(130, 145, gl, 1, CYAN);
			char * cont = "press any key or spacebar";
			drawText(35, 175, cont, 2, RED);
			cont = "to continue!";
			drawText(95, 190, cont, 2, RED);
		}
		
		
		/* PLAYER ONE SETUP */
		if(currentScreen == P1SetUp) {
			drawLocations(p1ShipLocations, 6, 86, colors[7]);
			char * player1inst = "p1 setup: ";
			drawText(20, 50, player1inst, 1, colors[7]);
			player1inst = "press keys to # $ % &! toggle sw0 or press";
			drawText(70, 50, player1inst, 1, colors[0]);
			player1inst = "enter to confirm!";
			drawText(120, 60, player1inst, 1, colors[0]);
			
			if(p1ShipCounter == 5) {
				/* CONFIRM SCREEN BEFORE MOVING ON TO P2 OR GAME START */
				drawBox(0, 50, colors[1], 310, 10);
				char * gameOn = "setup complete! press any key to continue or";
				drawText(50, 50, gameOn, 1, colors[0]);
			}
			
			// drawing the box
			if(flicker) {
				highlightSquare(currentRow, currentCol, colors[9], 5, 85);
			}

			else if(!flicker) {
				highlightSquare(currentRow, currentCol, colors[0], 5, 85);
			}
			
			int swVal = *(sw_ptr);
			while(swVal == 0b0000000001) {
				int swVal = *(sw_ptr);
				
				if(swVal == 0) {
					placeShip(currentRow, currentCol, p1ShipLocations);
					
					if(!isCoordError) {
						p1ShipCounter++;
					}
					else {
						isCoordError = false;
					}
					break;
				}
			}
		}
		
		
		/* PLAYER TWO SETUP */
		if(currentScreen == P2SetUp) {
			drawLocations(p2ShipLocations, 176, 86, colors[3]);
			char * player2inst = "p2 setup: ";
			drawText(20, 50, player2inst, 1, colors[3]);
			player2inst = "press keys to # $ % &! toggle sw0 or press";
			drawText(70, 50, player2inst, 1, colors[0]);
			player2inst = "enter to confirm!";
			drawText(120, 60, player2inst, 1, colors[0]);
			
			if(p2ShipCounter == 5) {
				/* CONFIRM SCREEN BEFORE MOVING ON TO P2 OR GAME START */
				drawBox(0, 50, colors[1], 310, 10);
				char * gameOn = "setup complete! press any key to continue or";
				drawText(50, 50, gameOn, 1, colors[0]);
			}
			
			// drawing the box
			if(flicker) {
				highlightSquare(currentRow, currentCol, colors[9], 175, 85);
			}
			else if(!flicker) {
				highlightSquare(currentRow, currentCol, colors[0], 175, 85);
			}
			
			int swVal = *(sw_ptr);
			while(swVal == 0b0000000001) {
				int swVal = *(sw_ptr);
				if(swVal == 0) {
					placeShip(currentRow, currentCol, p2ShipLocations);
					if(!isCoordError) {
						p2ShipCounter++;
					}
					else {
						isCoordError = false;
					}
					break;
				}
			}
		}
		
		
		/* GAME STARTS */
		if(currentScreen == gameStarts) {
			clear_screen();
			drawGrid(colors[0]);
			
			char * gameOn = "let the games begin!";
			drawText(50, 50, gameOn, 2, colors[0]);
				
			if(displayTextTime == 0) {
				clear_screen();
				drawGrid(colors[0]);
				displayTextTime = 2;
				currentScreen++;
				p1Turn = true;
			}
		}
		
		
		/* WAITING FOR ROW INPUT */
		if(currentScreen == waitForX) {
			if(timeVal == 60) {
				drawText(115, 65, "time left 1:00", 1, colors[9]);
			}
			else if(timeVal >= 10) {
				drawText(115, 65, "time left 0:", 1, colors[9]);
				drawTimer(timeVal);
			}
			else if(timeVal < 10) {
				drawText(115, 65, "time left 0:0", 1, colors[9]);
				drawTimer(timeVal);
			}
			else {
				drawText(115, 65, "time left 0:00", 1, colors[9]);
				drawTimer(timeVal);
			}
			
			int swVal = *(sw_ptr);
			char * value;
			int row = *(sw_ptr);
			tostring(value, actualRow);
			drawText(295, 50, value, 1, colors[9]);
			
			if(swVal != 0) {
				actualRow = decipherSwitch(row);
			}
			
			
			// display text
			if(p1Turn) {
				char * instr = "p1 turn: ";
				drawText(10, 50, instr, 1, colors[7]);
				instr = "input a row and press any key!";
				drawText(55, 50, instr, 1, colors[0]);
				
				instr = "row: ";
				drawText(255, 50, instr, 1, colors[0]);
			}
			
			if(p2Turn) {
				char * instr = "p2 turn: ";
				drawText(10, 50, instr, 1, colors[3]);
				instr = "input a row and press any key!";
				drawText(55, 50, instr, 1, colors[0]);
				
				instr = "row: ";
				drawText(255, 50, instr, 1, colors[0]);
			}
		}

		
		/* WAITING FOR COLUMN INPUT */
		if(currentScreen == waitForY) {
			if(timeVal == 60) {
				drawText(115, 65, "time left 1:00", 1, colors[9]);
			}
			else if(timeVal >= 10) {
				drawText(115, 65, "time left 0:", 1, colors[9]);
				drawTimer(timeVal);
			}
			else if(timeVal < 10) {
				drawText(115, 65, "time left 0:0", 1, colors[9]);
				drawTimer(timeVal);
			}
			else {
				drawText(115, 65, "time left 0:00", 1, colors[9]);
				drawTimer(timeVal);
			}
			
			int swVal = *(sw_ptr);
			char * value = "";
			int col = *(sw_ptr);
			tostring(value, actualCol);
			drawText(295, 50, value, 1, colors[9]);
			
			if(swVal != 0) {
				actualCol = decipherSwitch(col);
			}
			
			if(p1Turn) {
				char * instr = "p1 turn: ";
				drawText(10, 50, instr, 1, colors[7]);
				instr = "input a column and press any key!";
				drawText(55, 50, instr, 1, colors[0]);
				
				instr = "col: ";
				drawText(255, 50, instr, 1, colors[0]);
			}
			
			else if(p2Turn) {
				char * instr = "p2 turn: ";
				drawText(10, 50, instr, 1, colors[3]);
				instr = "input a column and press any key!";
				drawText(55, 50, instr, 1, colors[0]);

				instr = "col: ";
				drawText(255, 50, instr, 1, colors[0]);
			}
		}
		
		
		// checking if hit or miss
		if(currentScreen == isChecking) {
			if(p1Turn) {
				isHit(actualRow - 1, actualCol - 1, p2ShipLocations, p1Guesses);
				if(allGuessed(p2ShipLocations, p1Guesses)) {
					currentScreen = gameOver;
				}
			}
			
			else if(p2Turn) {
				isHit(actualRow - 1, actualCol - 1, p1ShipLocations, p2Guesses);
				if(allGuessed(p1ShipLocations, p2Guesses)) {
					currentScreen = gameOver;
				}
			}
		}
		
		
		/* DISPLAY THE HIT */
		if(currentScreen == isShipHit) {
			clear_screen();
			drawGrid(colors[0]);
			char * gameOn = "@ hit!";
			drawText(130, 55, gameOn, 2, colors[4]);
			
			if(displayTextTime == 0) {
				clear_screen();
				drawGrid(colors[0]);
				p1Turn = !p1Turn;
				p2Turn = !p2Turn;
				
				if(currentScreen != gameOver)
					currentScreen = waitForX;
				
				timeVal = 60;
				displayTextTime = 2;
			}
		}
		
		
		/* DISPLAY THE MISS */
		if(currentScreen == isMiss) {
			clear_screen();
			drawGrid(colors[0]);
			char * gameOn = "x miss!";
			drawText(130, 55, gameOn, 2, colors[9]);
			
			if(displayTextTime == 0) {
				clear_screen();
				drawGrid(colors[0]);
				currentScreen = waitForX;
				p1Turn = !p1Turn;
				p2Turn = !p2Turn;
				timeVal = 60;
				displayTextTime = 2;
			}
		}
		
		
		/* DISPLAY A TIMES UP MSG */
		if(currentScreen == timesUp) {
			clear_screen();
			drawGrid(colors[0]);
			char * time_msg = "times up!";
			drawText(120, 55, time_msg, 2, colors[0]);
			
			if(displayTextTime == 0) {
				clear_screen();
				drawGrid(colors[0]);
				currentScreen = waitForX;
				p1Turn = !p1Turn;
				p2Turn = !p2Turn;
				timeVal = 60;
				displayTextTime = 2;
			}
		}
		
		
		/* DISPLAY ALREADY GUESSED */
		if(currentScreen == alreadyGuessed) {
			clear_screen();
			drawGrid(colors[0]);
			char * error_msg = "already guessed this!";
			drawText(50, 50, error_msg, 2, colors[9]);

			if(displayTextTime == 0) {
				clear_screen();
				drawGrid(colors[0]);
				currentScreen = waitForX;
				p1Turn = !p1Turn;
				p2Turn = !p2Turn;
				timeVal = 60;
				displayTextTime = 2;
			}
		}
		
		
		/* GAME OVER SCREEN */
		if(currentScreen == gameOver) {
			clear_screen();
			drawTitle(colors[9], colors[8]);
			
			// display correct winner
			if(p1Win) {
				char * gameOn = "p1 wins!";
				drawText(20, 80, gameOn, 3, colors[7]);
			}
			else if(p2Win) {
				char * gameOn = "p2 wins!";
				drawText(20, 80, gameOn, 3, colors[3]);
			}
			
			char * returnStart = "press any key to return to start screen";
			drawText(10, 200, returnStart, 1, colors[0]);
		}
		
        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    }
}


// code given to us in lecture
void wait_for_vsync() {
	volatile int* pixel_ctrl_ptr = 0xFF203020;
	register int status;
	
	*pixel_ctrl_ptr = 1;
	
	// retrieve bit
	status = *(pixel_ctrl_ptr + 3);
	
	// 'and' to retrieve only the last bit
	while((status & 0x01) != 0) {
		status = *(pixel_ctrl_ptr + 3);
	}
}


// loop through the screen and make all pixels white? maybe
void clear_screen() {
	for(int x = 0; x < RESOLUTION_X; x++) {
		for(int y = 0; y < RESOLUTION_Y; y++) {
			plot_pixel(x, y, colors[1]);
		}
	}
}


void clearScreenAnimation(int row) {
	volatile int * priv_timer = (int*)0xFFFEC600;
	*(priv_timer) = 1;
	*(priv_timer + 2) = 0b111;
	
	while(*(priv_timer + 3) == 0) {
		//waiting
	}
	drawBox(0, 0, colors[1], row, 239);
	*(priv_timer + 3) = 0x1;
}


// plot pixel given to us
void plot_pixel(int x, int y, short int line_color) {
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}


void restartGrid() {
	for(int row = 0; row < 10; row++) {
		for(int col = 0; col < 10; col++) {
			p1ShipLocations[row][col] = 0;
			p2ShipLocations[row][col] = 0;
			p1Guesses[row][col] = 0;
			p2Guesses[row][col] = 0;
			currentCol = 0;
			currentRow = 0;
		}
	}
}


void highlightSquare(int currentRow, int currentCol, short int line_color, int offsetX, int offsetY) {
	draw_line(currentCol * 14 + offsetX, currentRow * 14 + offsetY, currentCol * 14 + offsetX + 14, currentRow * 14 + offsetY, line_color);
	draw_line(currentCol * 14 + offsetX, currentRow * 14 + offsetY + 14, currentCol * 14 + offsetX + 14, currentRow * 14 + offsetY + 14, line_color);
	draw_line(currentCol * 14 + offsetX, currentRow * 14 + offsetY, currentCol * 14 + offsetX, currentRow * 14 + offsetY + 14, line_color);
	draw_line(currentCol * 14 + offsetX + 14, currentRow * 14 + offsetY, currentCol * 14 + offsetX + 14, currentRow * 14 + offsetY + 14, line_color);
}

/*
void timerSetup(int time) {
	*(timer_ptr) = 200000000;
	*(timer_ptr + 2) = 0b111;
}
*/

void drawTimer(int timerVal) {
	char * timer = "";
	
	if(timeVal < 10) {
		tostring(timer, timeVal);
		drawText(184, 65, timer, 1, colors[9]);
	}
	else {
		tostring(timer, timeVal);
		drawText(178, 65, timer, 1, colors[9]);
	}
	
	if(timeVal == 0) {
		drawText(184, 65, "0", 1, colors[9]);
		currentScreen = timesUp;
		timeVal = 60;
	}
}


// code to check the value of a switch
int decipherSwitch(int coordinate) {
	if(coordinate == 0b0000000001) {
		return 1;
	}
	else if(coordinate == 0b0000000010) {
		return 2;
	}
	else if(coordinate == 0b0000000100) {
		return 3;
	}
	else if(coordinate == 0b0000001000) {
		return 4;
	}
	else if(coordinate == 0b0000010000) {
		return 5;
	}
	else if(coordinate == 0b0000100000) {
		return 6;
	}
	else if(coordinate == 0b0001000000) {
		return 7;
	}
	else if(coordinate == 0b0010000000) {
		return 8;
	}
	else if(coordinate == 0b0100000000) {
		return 9;
	}
	else if(coordinate == 0b1000000000) {
		return 10;
	}
	else {
		isCoordError = true;
		return 0;
	}
}


// draws a single letter
void drawLetter(int x, int y, char letter, int scaleFactor, short int line_color) {
	for(int i = 0; i < 5 * scaleFactor; i++) {
		for(int j = 0; j < 5 * scaleFactor; j++) {
			if(letter >= 97 && letter <= 122) {
				if(letterCodes[letter - 'a'][j / scaleFactor][i / scaleFactor] == 1) {
					plot_pixel(x + i, y + j, line_color);
				}
			}
			else if(letter >= 48 && letter <= 57) {
				if(numberCodes[letter - '0'][j / scaleFactor][i / scaleFactor] == 1) {
					plot_pixel(x + i, y + j, line_color);
				}
			}
			else if(letter == 46) { // period
				if(randoCodes[0][j / scaleFactor][i / scaleFactor] == 1) {
					plot_pixel(x + i, y + j, line_color);
				}
			}
			else if(letter == 47) { // slash
				if(randoCodes[1][j / scaleFactor][i / scaleFactor] == 1) {
					plot_pixel(x + i, y + j, line_color);
				}
			}
			else if(letter == 58) { // colon
				if(randoCodes[2][j / scaleFactor][i / scaleFactor] == 1) {
					plot_pixel(x + i, y + j, line_color);
				}
			}
			else if(letter == 33) { // exclamation point
				if(randoCodes[3][j / scaleFactor][i / scaleFactor] == 1) {
					plot_pixel(x + i, y + j, line_color);
				}
			}
			else if(letter == 35) { // up
				if(randoCodes[4][j / scaleFactor][i / scaleFactor] == 1) {
					plot_pixel(x + i, y + j, line_color);
				}
			}
			else if(letter == 36) { // down
				if(randoCodes[5][j / scaleFactor][i / scaleFactor] == 1) {
					plot_pixel(x + i, y + j, line_color);
				}
			}
			else if(letter == 37) { // left
				if(randoCodes[6][j / scaleFactor][i / scaleFactor] == 1) {
					plot_pixel(x + i, y + j, line_color);
				}
			}
			else if(letter == 38) { // right
				if(randoCodes[7][j / scaleFactor][i / scaleFactor] == 1) {
					plot_pixel(x + i, y + j, line_color);
				}
			}
			else if(letter == 64) { // checkmark
				if(randoCodes[8][j / scaleFactor][i / scaleFactor] == 1) {
					plot_pixel(x + i, y + j, line_color);
				}
			}
			else if(letter == 42) { // ship
				if(randoCodes[9][j / scaleFactor][i / scaleFactor] == 1) {
					plot_pixel(x + i, y + j, line_color);
				}
			}
		}
	}
}


void tostring(char * str, int num) {
    int i, rem, len = 0, n;
 
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}


// draws a given string
void drawText(int x, int y, char * text, int scaleFactor, short int line_color) {
	for(int strIndex = 0; strIndex < strlen(text); strIndex++) {
		drawLetter(x, y, *(text + strIndex), scaleFactor, line_color);
		if(*(text + strIndex) == 'y' || *(text + strIndex) == 'h' || 
		   *(text + strIndex) == 't' || *(text + strIndex) == 'w' 
		   || *(text + strIndex) == 'm'|| *(text + strIndex) == 'x'
		   || *(text + strIndex) == 'v') {
			x += 5 * scaleFactor + 1;
		}
		else {
			x += 5 * scaleFactor;
		}
	}
}


// draws the title in our own font
void drawTitle(short int line_color, short int line_color2) {
	// 'B'
	drawBox(10, 10, line_color, 5, 30);
	drawBox(15, 10, line_color, 10, 5);
	drawBox(15, 35, line_color, 15, 5);
	drawBox(15, 22, line_color, 15, 5);
	drawBox(25, 10, line_color, 5, 12);
	drawBox(30, 22, line_color, 5, 18);
	
	// 'A'
	drawBox(40, 10, line_color, 5, 30);
	drawBox(45, 10, line_color, 10, 5);
	drawBox(55, 10, line_color, 5, 30);
	drawBox(45, 20, line_color, 10, 5);
	
	// 'T'
	drawBox(75, 10, line_color, 5, 30);
	drawBox(65, 10, line_color, 25, 5);
	
	// 'T'
	drawBox(105, 10, line_color, 5, 30);
	drawBox(95, 10, line_color, 25, 5);
	
	// 'L'
	drawBox(125, 10, line_color, 5, 30);
	drawBox(125, 35, line_color, 20, 5);
	
	// 'E'
	drawBox(150, 10, line_color, 5, 30);
	drawBox(150, 10, line_color, 20, 5);
	drawBox(150, 35, line_color, 20, 5);
	drawBox(150, 22, line_color, 20, 5);
	
	// 'S'
	drawBox(175, 10, line_color, 5, 12);
	drawBox(175, 10, line_color, 20, 5);
	drawBox(175, 35, line_color, 20, 5);
	drawBox(190, 22, line_color, 5, 13);
	drawBox(175, 22, line_color, 20, 5);
	
	// 'H'
	drawBox(200, 10, line_color, 5, 30);
	drawBox(200, 22, line_color, 25, 5);
	drawBox(225, 10, line_color, 5, 30);
	
	// 'I'
	drawBox(245, 10, line_color, 5, 30);
	drawBox(235, 10, line_color, 25, 5);
	drawBox(235, 35, line_color, 25, 5);
	
	// 'P'
	drawBox(265, 10, line_color, 5, 30);
	drawBox(270, 10, line_color, 10, 5);
	drawBox(280, 10, line_color, 5, 15);
	drawBox(270, 20, line_color, 10, 5);
	
	// 'X'
	drawBox(293, 13, line_color2, 3, 3);
	drawBox(296, 18, line_color2, 3, 3);
	drawBox(305, 31, line_color2, 3, 3);
	drawBox(308, 36, line_color2, 3, 3);
	drawBox(300, 24, line_color2, 3, 3);
	drawBox(308, 13, line_color2, 3, 3);
	drawBox(305, 18, line_color2, 3, 3);
	drawBox(296, 31, line_color2, 3, 3);
	drawBox(293, 36, line_color2, 3, 3);
}


// code for drawing the grid
void drawGrid(short int line_color) {
	drawTitle(colors[9], colors[8]);
	int number = 0;
	char * numberString = "";
	
	for(int col = 5; col <= 145; col = col + 14) { 
		tostring(numberString, number);
		drawText(col - 11, 78, numberString, 1, colors[8]);
		draw_line(col, 85, col, 225, line_color);
		number++;
	}
	number = 0;
	for(int col = 175; col <= 319; col = col + 14) { 
		tostring(numberString, number);
		drawText(col - 11, 78, numberString, 1, colors[8]);
		draw_line(col, 85, col, 225, line_color);
		number++;
	}
	number = 0;
	for(int row = 85; row <= 225; row = row + 14) { 
		tostring(numberString, number);
		if(number == 10) {
			drawText(147, row - 10, numberString, 1, colors[8]);
		}
		
		else {
			drawText(148, row - 10, numberString, 1, colors[8]);
		}
		draw_line(5, row, 145, row, line_color);
		number++;
	}
	number = 0;
	for(int row = 85; row <= 225; row = row + 14) { 
		tostring(numberString, number);
		if(number == 10) {
			drawText(164, row - 10, numberString, 1, colors[8]);
		}
		
		else {
			drawText(169, row - 10, numberString, 1, colors[8]);
		}
		draw_line(175, row, 315, row, line_color);
		number++;
	}
	
	double offsetX = 0;
	
	for(int row = 0; row < 10; row++) {
		for(int col = 0; col < 10; col++) {
			if(p2Guesses[row][col] == 1 && p1ShipLocations[row][col] == 1) {
				offsetX = 10;
				drawLetter((col * 14) + offsetX, (row * 14) + 88, '@', 1, colors[4]);
			}
			else if(p2Guesses[row][col] == -1 && p1ShipLocations[row][col] == 0) {
				offsetX = 10;
				drawLetter((col * 14) + offsetX, (row * 14) + 90, 'x', 1, colors[9]);
			}
			
			if(p1Guesses[row][col] == 1 && p2ShipLocations[row][col] == 1) {
				offsetX = 180;
				drawLetter((col * 14) + offsetX, (row * 14) + 88, '@', 1, colors[4]);
			}
			else if(p1Guesses[row][col] == -1 && p2ShipLocations[row][col] == 0) {
				offsetX = 180;
				drawLetter((col * 14) + offsetX, (row * 14) + 90, 'x', 1, colors[9]);
			}
		}
	}
	
	char * playerName = "p l a y e r  1";
	drawText(40, 230, playerName, 1, colors[7]);
	playerName = "p l a y e r  2";
	drawText(210, 230, playerName, 1, colors[3]);
}



// code for seeing if player hit an opponent ship
void isHit(int row, int col, int opponentShips[10][10], int playerGuesses[10][10]) {
	if (row < 0 || row > 9 || col < 0 || col > 9) {
		currentScreen = isError;
	}
	
	else {
		// already guessed this
		if(playerGuesses[row][col] == 1 || playerGuesses[row][col] == -1) {
			currentScreen = alreadyGuessed;
			return;
		}
		else {
			playerGuesses[row][col] = 1;
		}
		
		// check if the guess is a location
		if(playerGuesses[row][col] == opponentShips[row][col]) {
			currentScreen = isShipHit;
			return;
		}
		else {
			playerGuesses[row][col] = -1;
			currentScreen = isMiss;
			return;
		}
	}
}


// code for seeing if player hit all the opponents ships
bool allGuessed(int opponentShips[10][10], int playerGuesses[10][10]) {
	int shipsGuessed = 0;
	
	for(int row = 0; row < 10; row++) {
		for(int col = 0; col < 10; col++) {
			if(playerGuesses[row][col] == 1 && opponentShips[row][col] == 1)
				shipsGuessed++;
		}
	}
	
	if(shipsGuessed == 5) {
		if(p1Turn) p1Win = true;
		else if(p2Turn) p2Win = true;
		return true;
	}
	else {
		return false;
	}
}


// code for randomizing ships of each player, saves in p1ShipLocations and p2ShipLocations
// range is the board coords for both x and y 
void randomizeShips(int playerShips[10][10]) {
	for(int i = 0; i < 10; i++) {
		for(int j = 0; j < 10; j++) {
			playerShips[i][j] = 0;
		}
    }
	
	for(int shipIdx = 0; shipIdx < 5; shipIdx++) {
		int row = rand() % 10;
		int col = rand() % 10;
		playerShips[row][col] = 1;
    }
}


void placeShip(int currentRow, int currentCol, int playerShips[10][10]) {
	if(playerShips[currentRow][currentCol] != 1) {
		playerShips[currentRow][currentCol] = 1;
	}
	else {
		isCoordError = true;
	}
}


// code for drawing the new locations of the ships
void drawLocations(int playerShips[10][10], int offsetX, int offsetY, short int line_color) {
	for(int i = 0; i < 10; i++) {
		for(int j = 0; j < 10; j++) {
			if(playerShips[i][j] == 1)
				drawBox((j) * 14 + offsetX, (i) * 14 + offsetY, line_color, 13, 13);
		}
	}
}


// code for drawing a box
void drawBox(int x, int y, short int line_color, int xMax, int yMax) {
	for(int i = 0; i < xMax; i++) {
		for(int j = 0; j < yMax; j++) {
			plot_pixel(x + i, y + j, line_color);
		}
	}
}


// code not shown for clear_screen() and draw_line() subroutines
void swap (int *x, int *y) {
    int temp;
    temp = *x;
    *x = *y;
    *y = temp;
}


// draw line code using algorithm
void draw_line(int x0, int y0, int x1, int y1, short int line_color) {
    bool is_steep = abs(y1 - y0) > abs(x1 - x0);
		
	if(is_steep) {
		swap(&x0, &y0);
     	swap(&x1, &y1);
	}
	
	if(x0 > x1) {
		swap(&x0, &x1);
	    swap(&y0, &y1);
	}
    
	int deltax = x1 - x0;
    int deltay = abs(y1 - y0);
    int error = -(deltax / 2);
    int y = y0;
	int y_step;
    
	if(y0 < y1){
		y_step = 1;
	}
	
	else {
		y_step = -1;
	}
	
	
	for(int x = x0; x <= x1; x++) {
    	if(is_steep) {
			plot_pixel(y, x, line_color);
		}
        
		else {
			plot_pixel(x, y, line_color);
		}
		
		error = error + deltay;
        
		if(error > 0) {
			y = y + y_step;
			error = error - deltax;
		}
	}
}


void set_A9_IRQ_stack(void) {
	int stack, mode;
	stack = A9_ONCHIP_END - 7; // top of A9 onchip memory, aligned to 8 bytes
	/* change processor to IRQ mode with interrupts disabled */
	mode = INT_DISABLE | IRQ_MODE;
	__asm__("msr cpsr, %[ps]" : : [ps] "r"(mode));
	/* set banked stack pointer */
	__asm__("mov sp, %[ps]" : : [ps] "r"(stack));
	/* go back to SVC mode before executing subroutine return! */
	mode = INT_DISABLE | SVC_MODE;
	__asm__("msr cpsr, %[ps]" : : [ps] "r"(mode));
}

/*
* Turn on interrupts in the ARM processor
*/
void enable_A9_interrupts(void) {
	int status = SVC_MODE | INT_ENABLE;
	__asm__("msr cpsr, %[ps]" : : [ps] "r"(status));
}

/*
* Configure the Generic Interrupt Controller (GIC)
*/
void config_GIC(void) {
	int address; // used
	config_interrupt(73, 1); // keys
	config_interrupt(79, 1); // private timer
	config_interrupt(29, 1); // private timer
	// Set Interrupt Priority Mask Register (ICCPMR). Enable interrupts of all
	// priorities
	address = MPCORE_GIC_CPUIF + ICCPMR;
	*((int *)address) = 0xFFFF;
	// Set CPU Interface Control Register (ICCICR). Enable signaling of
	// interrupts
	address = MPCORE_GIC_CPUIF + ICCICR;
	*((int *)address) = ENABLE;
	// Configure the Distributor Control Register (ICDDCR) to send pending
	// interrupts to CPUs
	address = MPCORE_GIC_DIST + ICDDCR;
	*((int *)address) = ENABLE;
}

void config_KEYs() {
	volatile int * KEY_ptr = (int *)KEY_BASE; // pushbutton KEY address
	*(KEY_ptr + 2) = 0xF; // enable interrupts for KEY
}

void config_timer() {
	volatile int * A9_timer_ptr = (int *)0xFFFEC600; //  address
	*(A9_timer_ptr) = 200000000; 
	*(A9_timer_ptr + 2) = 0b111; // enable interrupts for IAE
}

void config_keyboard() {
	volatile int * keyboard = (int *)PS2_BASE; //  address
	*(keyboard + 1) = 0x1; 
}

/*
 * Configure Set Enable Registers (ICDISERn) and Interrupt Processor Target
 * Registers (ICDIPTRn). The default (reset) values are used for other registers
 * in the GIC.
 */
void config_interrupt(int N, int CPU_target) {
    int reg_offset, index, value, address;
    /* Configure the Interrupt Set-Enable Registers (ICDISERn).
     * reg_offset = (integer_div(N / 32) * 4
     * value = 1 << (N mod 32) */
    reg_offset = (N >> 3) & 0xFFFFFFFC;
    index = N & 0x1F;
    value = 0x1 << index;
    address = 0xFFFED100 + reg_offset;
    /* Now that we know the register address and value, set the appropriate bit */
    *(int *)address |= value;
    /* Configure the Interrupt Processor Targets Register (ICDIPTRn)
     * reg_offset = integer_div(N / 4) * 4
     * index = N mod 4 */
    reg_offset = (N & 0xFFFFFFFC);
    index = N & 0x3;
    address = 0xFFFED800 + reg_offset + index;
    /* Now that we know the register address and value, write to (only) the
     * appropriate byte */
    *(char *)address = (char)CPU_target;
}


// Define the IRQ exception handler
void __attribute__((interrupt)) __cs3_isr_irq(void) {
	// Read the ICCIAR from the processor interface
	int address = MPCORE_GIC_CPUIF + ICCIAR;
	int int_ID = *((int *)address);
	
	if (int_ID == 29) { // A9 Private Timer
        A9_timer_ISR();
    }
	else if (int_ID == KEYS_IRQ) { // check if interrupt is from the KEYs
		pushbutton_ISR();
	}
	else if (int_ID == 79) { // A9 Private Timer
        keyboard_ISR();
    }
	else
	while (1)
	; // if unexpected, then stay here
	// Write to the End of Interrupt Register (ICCEOIR)
	address = MPCORE_GIC_CPUIF + ICCEOIR;
	*((int *)address) = int_ID;
	return;
}

/***************************************************************************************
* Pushbutton - Interrupt Service Routine
*
* This routine toggles the key_dir variable from 0 <-> 1
****************************************************************************************/
void pushbutton_ISR(void) {
	volatile int * KEY_ptr = (int *)KEY_BASE;
	int press;
	press = *(KEY_ptr + 3); // read the pushbutton interrupt register
	*(KEY_ptr + 3) = press; // Clear the interrupt
	
	if(currentScreen == startScreen) {
		clear_screen();
		currentScreen = instructionScreen;
	}
	else if(currentScreen == instructionScreen) {
		currentScreen = P1SetUp;
		flickerTime = 1;
	}
	
	else if(currentScreen == P1SetUp) {
		if(p1ShipCounter == 5) {
			if(press != 0) {
				currentRow = 0;
				currentCol = 0;
				currentScreen++;
				p1ShipCounter = 0;
				clear_screen();
				drawGrid(colors[0]);
				flickerTime = 1;
			}
		}

		// key 0 --> right
		else {
			if(press == 0b0001) {
				if(currentCol < 10)
					currentCol++;
			}
			// key 1 --> left
			else if(press == 0b0010) {
				if(currentCol > 0)
					currentCol--;
			}
			// key 2 --> down
			else if(press == 0b0100) {
				if(currentRow < 10)
					currentRow++;
			}
			// key 3 --> up
			else if(press == 0b1000) {
				if(currentRow > 0)
					currentRow--;
			}
		}
	}
	
	else if(currentScreen == P2SetUp) {
		if(p2ShipCounter == 5) {
			if(press != 0) {
				p2ShipCounter = 0;
				currentScreen++;
				clear_screen();
				drawGrid(colors[0]);
			}
		}
			
		else {
			// key 0 --> right
			if(press == 0b0001) {
				if(currentCol < 10)
					currentCol++;
			}
			// key 1 --> left
			else if(press == 0b0010) {
				if(currentCol > 0)
					currentCol--;
			}
			// key 2 --> down
			else if(press == 0b0100) {
				if(currentRow < 10)
					currentRow++;
			}
			// key 3 --> up
			else if(press == 0b1000) {
				if(currentRow > 0)
					currentRow--;
			}
		}
	}
	
	else if(currentScreen == waitForX) {
		//int swVal = *(sw_ptr);
		//char * value;
		//int row = *(sw_ptr);
		//actualRow = decipherSwitch(row);

		//if(swVal != 0) {
			//tostring(value, actualRow);
			//drawText(295, 50, value, 1, colors[9]);

			if(actualRow != 0) {
				if(press != 0) {
					currentScreen++;
					//clear_screen();
					drawGrid(colors[0]);
				}
			}
		//}
	}
	
	else if(currentScreen == waitForY) {
		//int swVal = *(sw_ptr);
		//char * value;

		//if(swVal != 0) {
			//int col = *(sw_ptr);
			//actualCol = decipherSwitch(col);
			//tostring(value, actualCol);
			//drawText(295, 50, value, 1, colors[9]);

			if(actualCol != 0) {
				if(press != 0) {		
					currentScreen = isChecking;
					//clear_screen();
					drawGrid(colors[0]);
				}
			}
		//}
	}
	
	else if(currentScreen == gameOver) {
		// player presses button to restart
		if(press != 0) {
			restartGrid();
			currentScreen = startScreen;
			p1Win = false;
			p2Win = false;
		}
	}
	
	return;
}


void A9_timer_ISR() {
	volatile int* timer_interrupt = (int *)0xFFFEC600;
    *(timer_interrupt + 3) = 0x1;
	
	flicker = !flicker;
	
	if(currentScreen == gameStarts) {
		displayTextTime--;
	}
	else if(currentScreen == timesUp) {
		displayTextTime--;
	}
	else if(currentScreen == isShipHit) {
		displayTextTime--;
	}
	else if(currentScreen == alreadyGuessed) {
		displayTextTime--;
	}
	else if(currentScreen == isMiss) {
		displayTextTime--;
	}
	else if(currentScreen == waitForX) {
		timeVal--;
	}
	else if(currentScreen == waitForY) {
		timeVal--;
	}
}


void keyboard_ISR() {
	unsigned char byteprev3 = 0;
	unsigned char byteprev2 = 0;
	unsigned char byteprev1 = 0;
  	volatile int * PS2_ptr = (int *) 0xFF200100;  // PS/2 port address

	int PS2_data, RVALID;
 	//int keyPressed = *((int *)PS2_BASE) & 0xFF; 
	PS2_data = *(PS2_ptr);	// read the Data register in the PS/2 port
	RVALID = (PS2_data & 0x8000);	// extract the RVALID field
	while(RVALID != 0) {
		
		/* always save the last three bytes received */

		byte1 = byte2;
		byte2 = byte3;
		byte3 = PS2_data & 0xFF;
		PS2_data = *(PS2_ptr);	// read the Data register in the PS/2 port
		RVALID = (PS2_data & 0x8000);	// extract the RVALID field
		if(currentScreen == startScreen) {
			if ((byte2 == 0xF0) && (byte3 == 0x29) ) {
				currentScreen = instructionScreen;
			}
		}
		else if(currentScreen == instructionScreen) {
			if ((byte2 == 0xF0) && (byte3 == 0x29) ) {
				currentScreen = P1SetUp;
			}
		}

		else if(currentScreen == P1SetUp || currentScreen == P2SetUp) {
			// up arrow
			if ((byte1 == 0xE0) && (byte2 == 0xF0) && (byte3 == 0x75) ) {
				if(currentRow > 0)
					currentRow--;
			}
			// down arrow
			else if ((byte1 == 0xE0) && (byte2 == 0xF0) && (byte3 == 0x72) ) {
				if(currentRow < 10)
					currentRow++;
			}
			// left arrow
			else if ((byte1 == 0xE0) && (byte2 == 0xF0) && (byte3 == 0x6B) ) {
				if(currentCol > 0)
					currentCol--;
			}
			// right arrow
			else if ((byte1 == 0xE0) && (byte2 == 0xF0) && (byte3 == 0x74) ) {
				if(currentCol < 10)
					currentCol++;
			}
			// place location
			else if ( (byte2 == 0xF0) && (byte3 == 0x5A) ) {
				if(currentScreen == P1SetUp)
					placeShip(currentRow, currentCol, p1ShipLocations);
				else if(currentScreen == P2SetUp)
					placeShip(currentRow, currentCol, p2ShipLocations);
				
				if(p1ShipCounter == 5) {
					currentRow = 0;
					currentCol = 0;
					currentScreen++;
					p1ShipCounter = 0;
					clear_screen();
					drawGrid(colors[0]);
					flickerTime = 1;
				}
				else if(p2ShipCounter == 5) {
					p2ShipCounter = 0;
					currentScreen++;
					clear_screen();
					drawGrid(colors[0]);
				}
				
				if(!isCoordError) {
					if(currentScreen == P1SetUp)
						p1ShipCounter++;
					if(currentScreen == P2SetUp)
						p2ShipCounter++;
				}
				else {
					isCoordError = false;
				}
			}
		}


else if(currentScreen == waitForX) {
			if ((byte2 == 0xF0) && (byte3 == 0x16) ) { // 1
				actualRow = 1;
			}
			else if ((byte2 == 0xF0) && (byte3 == 0x1E) ) { // 2
				actualRow = 2;
			}
			else if ((byte2 == 0xF0) && (byte3 == 0x26) ) { // 3
				actualRow = 3;
			}
			else if ((byte2 == 0xF0) && (byte3 == 0x25) ) { // 4
				actualRow = 4;
			}
			else if ((byte2 == 0xF0) && (byte3 == 0x2E) ) { // 5
				actualRow = 5;
			}
			else if ((byte2 == 0xF0) && (byte3 == 0x36) ) { // 6
				actualRow = 6;
			}
			else if ((byte2 == 0xF0) && (byte3 == 0x3D) ) { // 7
				actualRow = 7;
			}
			else if ((byte2 == 0xF0) && (byte3 == 0x3E) ) { // 8
				actualRow = 8;
			}
			else if ((byte2 == 0xF0) && (byte3 == 0x46) ) { // 9
				actualRow = 9;
			}
			else if( (byte2 == 0xF0) && (byte3 == 0x45) ) {
				actualRow = 10;
			}
		}
		
		else if(currentScreen == waitForY) {
			if ((byte2 == 0xF0) && (byte3 == 0x16) ) { // 1
				actualCol = 1;
			}
			else if ((byte2 == 0xF0) && (byte3 == 0x1E) ) { // 2
				actualCol = 2;
			}
			else if ((byte2 == 0xF0) && (byte3 == 0x26) ) { // 3
				actualCol = 3;
			}
			else if ((byte2 == 0xF0) && (byte3 == 0x25) ) { // 4
				actualCol = 4;
			}
			else if ((byte2 == 0xF0) && (byte3 == 0x2E) ) { // 5
				actualCol = 5;
			}
			else if ((byte2 == 0xF0) && (byte3 == 0x36) ) { // 6
				actualCol = 6;
			}
			else if ((byte2 == 0xF0) && (byte3 == 0x3D) ) { // 7
				actualCol = 7;
			}
			else if ((byte2 == 0xF0) && (byte3 == 0x3E) ) { // 8
				actualCol = 8;
			}
			else if ((byte2 == 0xF0) && (byte3 == 0x46) ) { // 9
				actualCol = 9;
			}
			else if( (byte2 == 0xF0) && (byte3 == 0x45) ) {
				actualCol = 10;
			}
		}
		else if(currentScreen == gameOver) {
			if ((byte2 == 0xF0) && (byte3 == 0x29) ) {
				currentScreen = startScreen;
				restartGrid();
			}
		}




	}
}