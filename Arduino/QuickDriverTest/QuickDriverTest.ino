const int limitSwitch = 4;  // use pin 4 for limit switch

void setup() {
  Serial.begin(9600);
  pinMode(limitSwitch, INPUT_PULLUP);  // internal pull-up resistor
}

void loop() {
  int state = digitalRead(limitSwitch);

  if (state == LOW) {
    Serial.println("Limit switch PRESSED");
  } else {
    Serial.println("Limit switch RELEASED");
  }

  delay(500); // small delay for readability
}
