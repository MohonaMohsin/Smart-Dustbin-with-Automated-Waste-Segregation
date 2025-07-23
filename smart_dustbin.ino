#include <CheapStepper.h>
#include <Servo.h>

Servo servo1;
#define ir 5
#define proxi 6
#define buzzer 12
int potPin = A0;
int soil = 0;
int fsoil = 0;
bool wasteHandled = false;

CheapStepper stepper(8, 9, 10, 11);

void setup() {
  Serial.begin(9600);
  pinMode(proxi, INPUT_PULLUP);
  pinMode(ir, INPUT);
  pinMode(buzzer, OUTPUT);
  servo1.attach(7);
  stepper.setRpm(17);

  servo1.write(70);  // Lid closed position
  delay(1000);
}

void loop() {
  fsoil = 0;
  wasteHandled = false;

  int L = digitalRead(proxi);
  Serial.println("=======================");
  Serial.print("Proximity: ");
  Serial.println(L);

  // === METAL WASTE DETECTION ===
  if (L == 0 && !wasteHandled) {
    Serial.println("         Metal Waste Detected");
    tone(buzzer, 1000, 1000);
    stepper.moveDegreesCW(240);
    delay(1000);
    servo1.write(180);
    delay(1000);
    servo1.write(70);
    delay(1000);
    stepper.moveDegreesCCW(240);
    delay(1000);
    wasteHandled = true;
  }

  // === IR DETECTED: DRY OR WET ===
  if (!wasteHandled && digitalRead(ir) == 0) {
    tone(buzzer, 1000, 500);
    delay(1000);

    soil = 0;
    for (int i = 0; i < 3; i++) {
      soil = analogRead(potPin);
      soil = constrain(soil, 485, 1023);
      fsoil += map(soil, 485, 1023, 100, 0);
      delay(75);
    }
    fsoil = fsoil / 3;

    Serial.print("Moisture Level: ");
    Serial.print(fsoil);
    Serial.println("%");

    if (fsoil > 45) {  // Wet waste
      Serial.println("     Wet Waste Detected");
      stepper.moveDegreesCW(120);
      delay(1000);
      servo1.write(180);
      delay(1000);
      servo1.write(70);
      delay(1000);
      stepper.moveDegreesCCW(120);
      delay(1000);
    } else {  // Dry waste
      Serial.println("          Dry Waste Detected");
      tone(buzzer, 1000, 500);
      delay(1000);
      servo1.write(180);
      delay(1000);
      servo1.write(70);
      delay(1000);
    }
    wasteHandled = true;
  }

  // === WAIT FOR ITEM TO BE REMOVED ===
  if (wasteHandled) {
    Serial.println("Waiting for waste to be removed...");
    delay(2000);
    while (digitalRead(ir) == 0 || digitalRead(proxi) == 0); // Wait until clear
    delay(500);
  }
}

