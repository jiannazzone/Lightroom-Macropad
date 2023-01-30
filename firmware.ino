#include <ClickEncoder.h>
#include <TimerOne.h>
#include <HID-Project.h>
#include <Bounce2.h>

#define ENCODER_CLK A0
#define ENCODER_DT A1
#define ENCODER_SW A2
#define LED 10
#define UP_PIN 8
#define DOWN_PIN 9

bool fastMode = false;
ClickEncoder *encoder;  // variable representing the rotary encoder
int last, value;        // variables for current and last rotation value

Bounce2::Button upButton = Bounce2::Button();
Bounce2::Button downButton = Bounce2::Button();

void timerIsr() {
  encoder->service();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);

  digitalWrite(LED, fastMode);
  Keyboard.begin();
  encoder = new ClickEncoder(ENCODER_DT, ENCODER_CLK, ENCODER_SW, 4);

  upButton.attach(UP_PIN, INPUT_PULLUP);
  downButton.attach(DOWN_PIN, INPUT_PULLUP);
  upButton.interval(20);
  downButton.interval(20);
  upButton.setPressedState(LOW);
  downButton.setPressedState(LOW);

  Timer1.initialize(1000);  // Initializes the timer, which the rotary encoder uses to detect rotation
  Timer1.attachInterrupt(timerIsr);
  last = -1;
}

void loop() {
  checkEncoderRotation();
  checkEncoderButton();
  checkSliderSelection();
  delay(10);
}

void checkEncoderRotation() {
  value += encoder->getValue();
  if (value == last) {
    return;
  }
  if (last > value) {
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
  last = value;
}

void checkEncoderButton() {
  ClickEncoder::Button b = encoder->getButton();
  if (b != ClickEncoder::Open) {
    if (b = ClickEncoder::Clicked) {
      fastMode = !fastMode;
      digitalWrite(LED, fastMode);
    }
  }
}

void checkSliderSelection() {
  upButton.update();
  downButton.update();
  if (upButton.pressed()) {
    Keyboard.write(',');
    return;
  }
  if (downButton.pressed()) {
    Keyboard.write('.');
    return;
  }
}