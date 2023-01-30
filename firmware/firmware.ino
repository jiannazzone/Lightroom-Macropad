#include <HID-Project.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#include <TimerThree.h>

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

bool fastMode = false;
const int fastLED = 15;

// Inputs
byte rows[] = { 6, 7, 8, 9};
const int rowCount = sizeof(rows) / sizeof(rows[0]);

// Outputs
byte cols[] = { 2, 3, 4, 5 };
const int colCount = sizeof(cols) / sizeof(cols[0]);

byte lastKeyState[colCount][rowCount];
byte currentKeyState[colCount][rowCount];
String keyCodes[colCount][rowCount] = {
  { "ESC", "Reject", "Adjust Back", "Enter" },
  { "Library", "Unflag", "Adjust Forward", "Left" },
  { "Develop", "Pick", "Undo", "Down" },
  { "Crop", "Redo", "Right", "Up" }
};

void adjustTimerIsr() {
  adjustEncoder->service();
}

void zoomTimerIsr() {
  zoomEncoder->service();
}

void setup() {
  Serial.begin(9600);
  Keyboard.begin();

  adjustEncoder = new ClickEncoder(adjustDT, adjustCLK, adjustSW,4);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(adjustTimerIsr);
  adjustLast = -1;

  zoomEncoder = new ClickEncoder(zoomDT, zoomCLK, zoomSW, 4);
  Timer3.initialize(1000);
  Timer3.attachInterrupt(zoomTimerIsr);
  zoomLast = -1;

  Serial.println("Powered On!");
  for (int x = 0; x < rowCount; x++) {
    // Serial.print(rows[x]);
    // Serial.println(" as input");
    pinMode(rows[x], INPUT);
  }

  for (int x = 0; x < colCount; x++) {
    // Serial.print(cols[x]);
    // Serial.println(" as input-pullup");
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
  // iterate the columns
  for (int colIndex = 0; colIndex < colCount; colIndex++) {
    // col: set to output to low
    byte curCol = cols[colIndex];
    pinMode(curCol, OUTPUT);
    digitalWrite(curCol, LOW);

    // row: interate through the rows
    for (int rowIndex = 0; rowIndex < rowCount; rowIndex++) {
      byte rowCol = rows[rowIndex];
      pinMode(rowCol, INPUT_PULLUP);
      currentKeyState[colIndex][rowIndex] = digitalRead(rowCol);
      if (currentKeyState[colIndex][rowIndex] == LOW) {
        if (keyCodes[colIndex][rowIndex] != "" && lastKeyState[colIndex][rowIndex] == HIGH) {
          executeCommand(colIndex, rowIndex);
        }
      }
      lastKeyState[colIndex][rowIndex] = currentKeyState[colIndex][rowIndex];
      pinMode(rowCol, INPUT);
    }
    // disable the column
    pinMode(curCol, INPUT);
  }
}

void executeCommand(int col, int row) {
  String thisCode = keyCodes[col][row];
  Serial.println(thisCode);

  // First Column
  switch (col) {
    case 0:
      firstCol(row);
      break;
    case 1:
      secondCol(row);
      break;
    case 2:
      thirdCol(row);
      break;
    case 3:
      fourthCol(row);
      break;
  }
}

void firstCol(int row) {
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