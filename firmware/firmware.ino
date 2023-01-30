/*
Lightroom Macropad by Adam Iannazzone
Keyboard matrix code adapted from: https://www.baldengineer.com/arduino-keyboard-matrix-tutorial.html
*/

#include <HID-Project.h> // https://github.com/NicoHood/HID
#include <ClickEncoder.h> // https://github.com/0xPIT/encoder/blob/master/ClickEncoder.h
#include <TimerOne.h> // https://github.com/PaulStoffregen/TimerOne
#include <TimerThree.h> // https://github.com/PaulStoffregen/TimerThree

// Encoder Definitions
const int adjustSW = 14;
int adjustClickState = HIGH;
const int adjustDT = A0;
const int adjustCLK = A1;

ClickEncoder *adjustEncoder;
int adjustLast, adjustVal;

const int zoomSW = 16;
int zoomClickState = HIGH;
const int zoomDT = A2;
const int zoomCLK = A3;

int zoomLast, zoomVal;
ClickEncoder *zoomEncoder;

// LED Definitions
bool fastMode = false;
const int fastLED = 15;

// Inputs
byte rows[] = { 6, 7, 8, 9};
const int rowCount = sizeof(rows) / sizeof(rows[0]);

// Outputs
byte cols[] = { 2, 3, 4, 5 };
const int colCount = sizeof(cols) / sizeof(cols[0]);

// Variables for tracking key state (to prevent duplicate presses)
byte lastKeyState[colCount][rowCount];
byte currentKeyState[colCount][rowCount];

// Friendly key descriptions for printing to serial
String keyCodes[colCount][rowCount] = {
  { "ESC", "Reject", "Adjust Back", "Enter" }, // Keys labeled (x, 1)
  { "Library", "Unflag", "Adjust Forward", "Left" }, // Keys labeled (x, 2)
  { "Develop", "Pick", "Undo", "Down" }, // Keys labeled (x, 3)
  { "Crop", "Redo", "Right", "Up" } // Keys labeled (x, 4)
};

// Timers for each encoder. ClickEncoder takes care of system interrupts
void adjustTimerIsr() {
  adjustEncoder->service();
}

void zoomTimerIsr() {
  zoomEncoder->service();
}

void setup() {
  Serial.begin(9600);

  // Initialize HID keyboard
  Keyboard.begin();

  // Set up ClickEncoders. Each one needs its own timer.
  adjustEncoder = new ClickEncoder(adjustDT, adjustCLK, adjustSW, 4);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(adjustTimerIsr);
  adjustLast = -1;

  zoomEncoder = new ClickEncoder(zoomDT, zoomCLK, zoomSW, 4);
  Timer3.initialize(1000);
  Timer3.attachInterrupt(zoomTimerIsr);
  zoomLast = -1;

  // Set pinModes for rows and columns
  for (int x = 0; x < rowCount; x++) {
    pinMode(rows[x], INPUT);
  }

  for (int x = 0; x < colCount; x++) {
    pinMode(cols[x], INPUT_PULLUP);
  }
  pinMode(fastLED, OUTPUT);
}

void checkAdjustEncoder() {
  // Push Button
  ClickEncoder::Button b = adjustEncoder->getButton();
  if (b != ClickEncoder::Open) {
    if (b = ClickEncoder::Clicked) {
      fastMode = !fastMode;
      digitalWrite(fastLED, fastMode);
    }
  }

  // Encoder Rotation
  adjustVal += adjustEncoder->getValue();
  if (adjustVal == adjustLast) {
    return;
  }
  if (adjustLast > adjustVal) {
    if (fastMode) {
      Keyboard.write('+');
    } else {
      Keyboard.write('=');
    }
  } else {
    if (fastMode) {
      Keyboard.write('_');
    } else {
      Keyboard.write('-');
    }
  }
  adjustLast = adjustVal;
}

void checkZoomEncoder() {
  // Push Button
  ClickEncoder::Button c = zoomEncoder->getButton();
  if (c != ClickEncoder::Open) {
    if (c = ClickEncoder::Clicked) {
      Keyboard.write(' ');
    }
  }

  // Encoder Rotation
  zoomVal += zoomEncoder->getValue();
  if (zoomVal == zoomLast) {
    return;
  }
  if (zoomLast > zoomVal) {
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.write('=');
  } else {
    Keyboard.press(KEY_RIGHT_GUI);
    Keyboard.write('-');
  }
  Keyboard.releaseAll();
  zoomLast = zoomVal;
}

void readMatrix() {
  // Iterate through each column
  for (int colIndex = 0; colIndex < colCount; colIndex++) {
    byte curCol = cols[colIndex];
    pinMode(curCol, OUTPUT);
    digitalWrite(curCol, LOW);

    // Iterate through each row for this column only
    for (int rowIndex = 0; rowIndex < rowCount; rowIndex++) {
      byte rowCol = rows[rowIndex];
      pinMode(rowCol, INPUT_PULLUP);
      currentKeyState[colIndex][rowIndex] = digitalRead(rowCol);

      // Check if key is pressed
      if (currentKeyState[colIndex][rowIndex] == LOW) {
        // Ensure that current key state differs from previous state. This will avoid duplicate presses
        // Remove this inner if-statement if you want to allow burst mode
        if (keyCodes[colIndex][rowIndex] != "" && lastKeyState[colIndex][rowIndex] == HIGH) {
          executeCommand(colIndex, rowIndex);
        }
      }

      // Update key states
      lastKeyState[colIndex][rowIndex] = currentKeyState[colIndex][rowIndex];
      
      // Disable row
      pinMode(rowCol, INPUT);
    }

    // Disable column
    pinMode(curCol, INPUT);
  }
}

void executeCommand(int col, int row) {
  String thisCode = keyCodes[col][row];
  Serial.println(thisCode);

  // Go to appropriate function for active column
  switch (col) {
    case 0:
      // Keys labeled (x, 1)
      firstCol(row);
      break;
    case 1:
      // Keys labeled (x, 2)
      secondCol(row);
      break;
    case 2:
      // Keys labeled (x, 3)
      thirdCol(row);
      break;
    case 3:
      // Keys labeled (x, 4)
      fourthCol(row);
      break;
  } // switch
} // executeCommand

/*
Each function below represents one column in the keyboard matrix.
You will need to modify the keypresses for each item if you want to alter the macropad.
NOTE: The rows and columns do NOT directly correspond to they key layout.
Refer to the diagram at https://github.com/jiannazzone/Lightroom-Macropad for more information.
*/
void firstCol(int row) {
  // Keys labeled (x, 1)
  switch (row) {
    case 0:
      Keyboard.write(KEY_ESC);
      return;
    case 1:
      Keyboard.write('x');
      return;
    case 2:
      Keyboard.write(',');
      return;
    case 3:
      Keyboard.write(KEY_RETURN);
      return;
  }
}

void secondCol(int row) {
  // Keys labeled (x, 2)
  switch (row) {
    case 0:
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.write('1');
      Keyboard.releaseAll();
      return;
    case 1:
      Keyboard.write('u');
      return;
    case 2:
      Keyboard.write('.');
      return;
    case 3:
      Keyboard.write(KEY_LEFT_ARROW);
      return;
  }
}

void thirdCol(int row) {
  // Keys labeled (x, 3)
  switch (row) {
    case 0:
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.write('2');
      Keyboard.releaseAll();
      return;
    case 1:
      Keyboard.write('p');
      return;
    case 2:
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.write('z');
      Keyboard.releaseAll();
      return;
    case 3:
      Keyboard.write(KEY_DOWN_ARROW);
      return;
  }
}

void fourthCol(int row) {
  switch (row) {
    // Keys labeled (x, 4)
    case 0:
      Keyboard.write('r');
      return;
    case 1:
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.write('z');
      Keyboard.releaseAll();
    case 2:
      Keyboard.write(KEY_RIGHT_ARROW);
      return;
    case 3:
      Keyboard.write(KEY_UP_ARROW);
      return;
  }
}

void printMatrix() {
  for (int rowIndex = 0; rowIndex < rowCount; rowIndex++) {
    if (rowIndex < 10)
      Serial.print(F("0"));
    Serial.print(rowIndex);
    Serial.print(F(": "));

    for (int colIndex = 0; colIndex < colCount; colIndex++) {
      Serial.print(lastKeyState[colIndex][rowIndex]);
      if (colIndex < colCount)
        Serial.print(F(", "));
    }
    Serial.println("");
  }
  Serial.println("");
}

void loop() {
  readMatrix();
  checkAdjustEncoder();
  checkZoomEncoder();
}