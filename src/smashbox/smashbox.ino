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

#define D_PAD 43

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

#define X_MOD_NONE 86
#define X_MOD_1    27
#define X_MOD_2    55
#define X_MOD_BOTH 73

#define Y_MOD_NONE 86
#define Y_MOD_1    27
#define Y_MOD_2    53
#define Y_MOD_BOTH 74

#define ANALOG_SHOULDER 74

/********GLOBALS********/
CGamecubeConsole Console(DATA); //creates a virtual console sending and receiving data to/from the DATA pin
Gamecube_Data_t data = defaultGamecubeData; //initializes the data struct which is sent to console

/********LOGIC********/
void setup() {
	int controllerInputs[] = {A, B, X, Y, Z, START, L, R, R_ANALOG, LEFT, RIGHT, UP, DOWN, C_LEFT, C_RIGHT, C_UP, C_DOWN, X_AXIS_1, X_AXIS_2, Y_AXIS_1, Y_AXIS_2};
	for(int i = 0; i < (sizeof(controllerInputs) / sizeof(int)); i++) {
		pinMode(controllerInputs[i], INPUT_PULLUP);
	}

	//used for debugging
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(9600);
}

//gamecube reads values 1 and 0, not true and false
//this function encodes the value of a pin into a gamecube-readable format
int readButton(int pin) {
	return (digitalRead(pin) == LOW ? 1 : 0);
}

void loop() {
	//data = defaultGamecubeData; //reset all values to default so that the previous cycle's inputs do not carry over
	/*
	static int testing = UNPUSHED;
	data.report.start = ++testing;
    testing %= 2;
	*/

	/********CONTROL STICK********/
	data.report.xAxis = STICK_NEUTRAL;
	if(readButton(LEFT) != readButton(RIGHT)) { //both at the same time is considered neutral
		int sign = (readButton(LEFT) ? -1 : +1); //subtract modifiers if inputting left; add modifiers if inputting right
		int mod = 0;

		if(readButton(X_AXIS_1))     mod += X_MOD_1;
		if(readButton(X_AXIS_2))     mod += X_MOD_2;
		if(mod == X_MOD_1 + X_MOD_2) mod = X_MOD_BOTH;
		else if(mod == 0)            mod = X_MOD_NONE;

		data.report.xAxis += mod * sign;
	}

	data.report.yAxis = STICK_NEUTRAL;
	if(readButton(UP) != readButton(DOWN)) { //same logic as L/R
		int sign = (readButton(UP) ? +1 : -1);
		int mod = 0;

		if(readButton(Y_AXIS_1))     mod += Y_MOD_1;
		if(readButton(Y_AXIS_2))     mod += Y_MOD_2;
		if(mod == Y_MOD_1 + Y_MOD_2) mod = Y_MOD_BOTH;
		else if(mod == 0)            mod = Y_MOD_NONE;

		data.report.yAxis += mod * sign;
	}

	/********C STICK********/
	data.report.cxAxis = STICK_NEUTRAL;
	data.report.cyAxis = STICK_NEUTRAL;
	if(readButton(C_LEFT) && !readButton(C_RIGHT)) data.report.cxAxis = STICK_MIN
	if(!readButton(C_LEFT) && readButton(C_RIGHT)) data.report.cxAxis = STICK_MAX
	if(readButton(C_UP) && !readButton(C_DOWN))    data.report.cyAxis = STICK_MAX
	if(!readButton(C_UP) && readButton(C_DOWN))    data.report.cyAxis = STICK_MIN

	/********FACE BUTTONS********/
	data.report.a = readButton(A);
	data.report.b = readButton(B);
	data.report.x = readButton(X);
	data.report.y = readButton(Y);

	data.report.start = readButton(START);

	/********SHOULDER BUTTONS********/
	data.report.l = readButton(L);
	data.report.r = readButton(R);
	data.report.z = readButton(Z);
	data.report.right = (readButton(R_ANALOG) ? ANALOG_SHOULDER : 0);

	/********D-PAD********/
	//the modifier buttons double as the D-Pad when a switch on the controller is toggled
	data.report.dleft  = readButton(D_PAD) & readButton(X_AXIS_1);
	data.report.dright = readButton(D_PAD) & readButton(Y_AXIS_2);
	data.report.dup    = readButton(D_PAD) & readButton(Y_AXIS_1);
	data.report.ddown  = readButton(D_PAD) & readButton(X_AXIS_2);


	if(!Console.write(data)) {
		//uh oh, something went wrong
		Serial.println(F("Error writing data to console!")); //if i ever have this plugged into USB while debugging
		digitalWrite(LED_BUILTIN, HIGH); //the real useful stuff, flash the LED if something goes wrong
		delay(1000); //this delay is important in case the error occurs at the very beginning of write() and we miss its inherent delay
		digitalWrite(LED_BUILTIN, LOW);
	}

	//when successful, write() has a delay so we do not need an additional one
	//in fact, adding a delay after a succesful write will lead to undesirable behavior, namely an unresponsive controller
}
