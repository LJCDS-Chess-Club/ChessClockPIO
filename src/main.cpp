#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#define SEC (millis()/1000)

#define pressure analogRead(A0) //make sure FSR is set to the right analog pin

#define whiteTime times[0]
#define blackTime times[1]

// LCD Setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Timer vars
int top, bot, change = 0;

//int whiteTime;
//int blackTime;
int times[2];
int increment;
int delayVal;
int delayLeft;
int startTime;

int prevTurn;
bool turnChanged = false;
bool turn = 0; // 0: White  1: Black

// Function declarations
String formatTime(int sec);
bool checkTurn(int pressureVal);
void printData();

//time txt
String firstLine;

bool useFlags = false;


String formatTime(int sec) {
	if (sec < 0) return "-"+formatTime(-sec);
	String out = "";
	int seconds = sec%60;
	int mins = sec/60;
	
	out += String(mins) + ":" ;
	if (String(seconds).length() == 1) {
		out += "0";
	}
	out += String(seconds);
	
	return out;
}

bool checkTurn(int pressureVal) {
	if (pressureVal < 100) {
		return 0; //white's turn
	}else{
		return 1; //black's turn
	}
}


void setup() {
	lcd.init();
	lcd.backlight();
	Serial.begin(9600);
	startTime = 0.05*60;
	increment = 5;
	delayVal = 3;
	delayLeft = delayVal;
//  whiteTime = startTime;
//  blackTime = startTime;
	times[0] = startTime;
	times[1] = startTime;
	turn = false;
	lcd.setCursor(0,0);
	lcd.print("LJCD CHESS CLOCK");
	lcd.setCursor(0,1);
	lcd.print("TIME:"+String(startTime/60.0)+"+"+String(increment)+" D" + delayVal);
	delay(2500);
	lcd.clear();
	lcd.setCursor(0,1);
	printData();
	Serial.println(delayVal);

}

unsigned int fll = 0;

void loop() {

	top = SEC;
	//checking who's turn it is
	turn = checkTurn(pressure);

	//compare if the turn changed. If it does, then add increment to time.
	if (turn != prevTurn) { // On turn change
		times[prevTurn] += increment;
		delayLeft = delayVal;
		printData();
	}

	//changes the time value
//  if (turn) {
//    blackTime -= change;
//  } else {
//    whiteTime -= change;
//  }
	if (delayLeft > 0 ) {
		delayLeft -= change;
		lcd.setCursor(turn ? 13 : 2, 1);
		lcd.print(int(delayLeft));
	} else {
		times[turn] -= change;
	}

	
	//must save the previous turn value in order to compare if the turn ever changes
	prevTurn = turn;

	//formats the string for times 
	firstLine = formatTime(whiteTime) +"||"+ formatTime(blackTime);
	if (fll != firstLine.length()) {
		lcd.clear();
		printData();
	}
	fll = firstLine.length();
	

	// display the times
	lcd.setCursor(1, 0);
	if (whiteTime > 0 || !useFlags) {
		lcd.print(formatTime(whiteTime));
	}
	else {
		lcd.print(" ⚐ ");
	}

	lcd.setCursor(7,0);
	lcd.print("||");

	lcd.setCursor(15 - formatTime(blackTime).length() , 0);
	if (blackTime > 0 || !useFlags) {
		lcd.print(formatTime(blackTime));
	}
	else {
		lcd.print(" ⚑ ");
	}

	bot = SEC;
	change = (bot-top);

	//Serial.println(String(times[0])+ "_" + String(times[1]));
	//Serial.println(delayLeft);
}

void printData() {
	String inc = "";
	String del = "";
	lcd.setCursor(0, 1);
//   0123456789abcdef
//  " W | INC000 | B "

	inc += String(increment).length() == 1 ? "0" : "";
	inc += String(increment);

	del += String(delayVal).length() == 1 ? "0" : "";
	del += String(delayVal);

	lcd.print(
		" W |I" + inc + "  D" + del + "| B "
		);

	lcd.setCursor( turn ? 15 : 0, 1);
	lcd.print(turn ? "<" : ">");

}