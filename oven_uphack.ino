/******************************************************************************
  Mux_Analog_Output
  SparkFun Multiplexer Output Example
  Jim Lindblom @ SparkFun Electronics
  August 15, 2016
  https://github.com/sparkfun/74HC4051_8-Channel_Mux_Breakout

  This sketch demonstrates how to use the SparkFun Multiplexer
  Breakout - 8 Channel (74HC4051) to drive eight outputs using
  four digital pins.

  Hardware Hookup:
  Mux Breakout ----------- Arduino
     S0 ------------------- 2LOWLOWLOW
     S1 ------------------- 3
     S2 ------------------- 4
     Z -------------------- 5
    VCC ------------------- 5V
    GND ------------------- GND
    (VEE should be connected to GND)DEBUGX

  Development environment specifics:
  Arduino 1.6.9
  SparkFun Multiplexer Breakout - 8-Channel(74HC4051) v10
  (https://www.sparkfun.com/products/13906)
******************************************************************************/
//#define DEBUG 
/////////////////////
// Pin Definitions //
/////////////////////LOW
const byte muxPinMap[4][3] = {{2, 3, 4}, {5, 6, 7}, {8, 9, 10}, {11, 12, 13}}; // S0~2, S1~3, S2~4
const byte OVEN_ROW_MUX = 0;
const byte OVEN_COL_MUX = 1;
const byte KEYPAD_ROW_MUX = 2;
const byte KEYPAD_COL_MUX = 3;
const byte KEYPAD_ROW_Z = A1;
const byte ENABLE_OVEN_PIN = A0;

const int BUTTON_PRESS_DELAY = 75;


// Lx & Rx are the pins on the oven PC board
// these constants map them to multiplexer pins for
// to be able to simulate pressing them
// Oven Columns
const int L9 = 0;
const int L10 = 1;
const int L11 = 2;
const int L12 = 3;
const int L13 = 4;

// Oven Rows
const int L14 = 0;
const int L15 = 1;
const int L16 = 2;
const int L17 = 3;
const int L18 = 4;
const int L19 = 5;
const int R1 = 6;
const int R2 = 7;

struct Coord {
  byte row;
  byte col;
};

// constants for the names of the buttons on the oven
// that we want to be connecting.
const struct Coord pizza = {L16, L9};
const struct Coord temp_ = {L15, L9};
const struct Coord adMin = {L17, L9};
const struct Coord light = {L18, L9};
const struct Coord cancl = {L19, L9};
const struct Coord stClk = {L14, L9};
const struct Coord slfCn = {L15, L10};
const struct Coord strt_ = {L17, L10};
const struct Coord broil = {L18, L10};
const struct Coord n_1__ = {L17, L13};
const struct Coord n_2__ = {L15, L13};
const struct Coord n_3__ = {L16, L13};
const struct Coord n_4__ = {L17, L12};
const struct Coord n_5__ = {L15, L12};
const struct Coord n_6__ = {L16, L12};
const struct Coord n_7__ = {L17, L11};
const struct Coord n_8__ = {L15, L11};
const struct Coord n_9__ = {L16, L11};
const struct Coord n_0__ = {L16, L10};
const struct Coord cknNg = {L18, L11};
const struct Coord bake_ = {L19, L11};
const struct Coord dlySt = {R1, L11};
const struct Coord bkTim = {R2, L11};
const struct Coord qPrHt = {L18, L12};
const struct Coord cnvct = {L18, L13};
const struct Coord lock_ = {L19, L13};
const struct Coord timer = {L14, L13};
const struct Coord kpWrm = {L19, L12};
const struct Coord cvCnv = {L14, L12};
const struct Coord xxxxx = {0, 0};

// Here's the spreadsheet where we mapped this out
// https://docs.google.com/spreadsheets/d/19LWx6lZHyPTzfsFUzfeQjLOUcBIoyNjJwaMc6eYhULs/edit#gid=1272386141

// The elments of this map are which button to press on the oven
// given a given matrix coordinate from the keypad row & column multiplexers
const struct Coord coordMap[8][8] = {
  /*
    COL: 0     1     2     3     4     5     6     7      ROW:                              */
  {xxxxx, broil, strt_, xxxxx, xxxxx, xxxxx, xxxxx, xxxxx}, // 0
  {xxxxx, cancl, n_0__, bake_, xxxxx, xxxxx, xxxxx, xxxxx}, // 1
  {stClk, slfCn, cvCnv, cnvct, xxxxx, xxxxx, xxxxx, xxxxx}, // 2
  {kpWrm, n_7__, n_4__, n_1__, xxxxx, xxxxx, xxxxx, xxxxx}, // 3
  {temp_, n_8__, n_5__, n_2__, xxxxx, xxxxx, xxxxx, xxxxx}, // 4
  {cknNg, n_9__, n_6__, n_3__, xxxxx, xxxxx, xxxxx, xxxxx}, // 5
  {light, adMin, dlySt, timer, xxxxx, xxxxx, xxxxx, xxxxx}, // 6
  {lock_, qPrHt, bkTim, pizza, xxxxx, xxxxx, xxxxx, xxxxx} // 7
};


void setupMux(byte mux_id) {
  // Set up the pins that go to the mux control inputs, as outputs
  for (int i = 0; i < 3; i++)
  {
    pinMode(muxPinMap[mux_id][i], OUTPUT);
    digitalWrite(muxPinMap[mux_id][i], LOW);
  }
}

void setupOvenMux() {
  // the oven multiplexer needs to be enabled and disabled so that it's
  // only active when the button is being "presed"
  pinMode(ENABLE_OVEN_PIN, OUTPUT);
  disableOvenInput();
  setupMux(OVEN_ROW_MUX);
  setupMux(OVEN_COL_MUX);
}

void setupKeypadMux() {
  setupMux(KEYPAD_ROW_MUX);
  setupMux(KEYPAD_COL_MUX);
  // we need to read the row to see which one is getting
  // power during the scan so set a pin to get that input
  pinMode(KEYPAD_ROW_Z, INPUT);
}

void debugPrint(String str) {
#ifdef DEBUG
  Serial.print(str);
#endif
}

void setup()
{
#ifdef DEBUG
  Serial.begin(9600); // Initialize the serial port
  debugPrint("Setting up Muxes\n");
#endif

  setupOvenMux();
  setupKeypadMux();

#ifdef TEST_OVEN_PRESS
  debugPrint("Waiting 5 seconds\n");
  delay(5000);
  struct Coord keypadCoord;

  // bool pressed = scanKeypad(&keypadCoord);

  //pressOvenButton(getOvenCoord(keypadCoord));
  pressOvenButton(cancl);
  delay(1000);
  pressOvenButton(cancl);
#endif
}

// for a given mux, goes through each Y pin and
// selects it, routing its value through Z. Then,
// if a value of HIGH is getting through the Z of the
// row mux, set result to that value. Otherwise just
// return false.
bool readMux(byte mux_id, byte *result) {
  for (int i = 0; i < 8; i++) {
    selectMuxPin(i, mux_id);
    if (digitalRead(KEYPAD_ROW_Z) == HIGH) {
      *result = i;
      return true;
    }
  }
  return false;
}

// for each column, check to see if there is power at a given row, and if so
// we know that button is pressed.
bool scanKeypad(struct Coord *result) {
  for (int col = 0; col < 8; col++)
  {
    // power up each column
    selectMuxPin(col, KEYPAD_COL_MUX);
    // and read each row to see if the power flowed through indicating button press
    byte row;
    if (readMux(KEYPAD_ROW_MUX, &row)) {
      result->row = row;
      result->col = col;
      return true;
    }
  }
  return false;
}

bool wasPressed;

void loop()
{

  /*  // every second, read each pin on the row mux.
    // If there's a value on none of the pins, don't do
    // anything. Otherwise, print the value on the first
    // pin with a value that we come across.
    //  delay(1000);
    byte result;
    if (readMux(KEYPAD_ROW_MUX, &result)) {
      Serial.print("Pressed: "+String(result)+"\n");
    }
  */

  // Every 100ms scan the keypad to see if a button is being pressed.
  // translate the key according to the map and "press" the corresponding oven button
  delay(10);
  struct Coord keypadCoord;
  bool pressed = scanKeypad(&keypadCoord);
  if (pressed && !wasPressed) {
    struct Coord ovenCoord = getOvenCoord(keypadCoord);
    debugPrint("Pressed on keypad: " + String(keypadCoord.row) + ", " + String(keypadCoord.col) + "\n");
    debugPrint(" Pressing on oven: " + String(ovenCoord.row) + ", " + String(ovenCoord.col) + "\n");
    pressOvenButton(ovenCoord);
    wasPressed = true;
  }
  if (!pressed && wasPressed) {
    wasPressed = false;
  }
}


struct Coord getOvenCoord(struct Coord input) {
  return coordMap[input.row][input.col];
}

void pressOvenButton(struct Coord press) {
  debugPrint("Pressing: " + String(press.row) + ", " + String(press.col) + "\n");
  selectMuxPin(press.row, OVEN_ROW_MUX);
  selectMuxPin(press.col, OVEN_COL_MUX);
  enableOvenInput();
  delay(BUTTON_PRESS_DELAY);
  disableOvenInput();
}

void disableOvenInput() {
  digitalWrite(ENABLE_OVEN_PIN, HIGH);
}
void enableOvenInput() {
  digitalWrite(ENABLE_OVEN_PIN, LOW);
}


// The selectMuxPin function sets the S0, S1, and S2 pins
// accordingly, given a pin from 0-7.
void selectMuxPin(byte pin, byte mux_id)
{
  if (pin > 7) return; // Exit if pin is out of scope
  for (int i = 0; i < 3; i++)
  {
    if (pin & (1 << i))
      digitalWrite(muxPinMap[mux_id][i], HIGH);
    else
      digitalWrite(muxPinMap[mux_id][i], LOW);
  }
}
