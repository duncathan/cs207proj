#include <Nintendo.h>

/********PINOUT********/
//controller
#define A 22
#define B 23
#define X 24
#define Y 25
#define Z 26

#define START 27

#define L 28
#define R 29
#define R_ANALOG 42

#define LEFT 30
#define RIGHT 31
#define UP 32
#define DOWN 33

#define C_LEFT 38
#define C_RIGHT 39
#define C_UP 40
#define C_DOWN 41

#define X_AXIS_1 34
#define X_AXIS_2 35
#define Y_AXIS_1 36
#define Y_AXIS_2 37

//SD card
#define SD_DO  50
#define SD_DI  51
#define SD_CLK 52
#define SD_CS  53

//data output
#define DATA 8

/********CONSTANTS********/
#define UNPUSHED 0
#define PUSHED   1

#define STICK_MIN     0
#define STICK_NEUTRAL 128
#define STICK_MAX     255

/********GLOBALS********/
CGamecubeConsole Console(DATA); //creates a virtual console sending and receiving data to/from the DATA pin
Gamecube_Data_t data = defaultGamecubeData; //initializes the data struct which is sent to console

int testing = UNPUSHED;

/********LOGIC********/
void setup() {
	int controllerInputs[] = {A, B, X, Y, Z, START, L, R, R_ANALOG, LEFT, RIGHT, UP, DOWN, C_LEFT, C_RIGHT, C_UP, C_DOWN, X_AXIS_1, X_AXIS_2, Y_AXIS_1, Y_AXIS_2};
	for(int i = 0; i < (sizeof(controllerInputs) / sizeof(int)); i++) {
		pinMode(controllerInputs[i], INPUT_PULLUP);
	}
}

void loop() {
	//put your main code here, to run repeatedly:
	data.report.start = ++testing;
	testing %= 2;
	Console.write(data);
	delay(1000/60); //one frame at 60fps
}