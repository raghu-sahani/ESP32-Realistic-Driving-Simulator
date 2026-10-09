// Classic ESP32 Dev Module + mechanical quadrature encoder (KY-040 style).
// CLK=32, DT=33, SW=25, +=3.3V, GND=GND.
// Accelerator button: GPIO26 to GND. Brake button: GPIO27 to GND.
// Normally-open momentary buttons; internal pullups; no extra libraries.
#include <Arduino.h>

constexpr uint8_t CLK_PIN = 32, DT_PIN = 33, SW_PIN = 25;
constexpr uint8_t ACCEL_PIN = 26, BRAKE_PIN = 27;
constexpr int STEERING_STEP = 5;        // 20 clicks from center to either lock
constexpr int TRANSITIONS_PER_CLICK = 4; // Try 2 if your encoder needs two clicks
constexpr bool REVERSE_ENCODER = false; // Set true if physical directions are reversed
constexpr uint32_t DEBOUNCE_MS = 35;
// Valid Gray-code transitions; reversed transitions cancel contact bounce.
const int8_t TRANSITIONS[16] = {0,-1,1,0, 1,0,0,-1, -1,0,0,1, 0,1,-1,0};
int steering = 0, partial = 0, lastSent = 999;
uint8_t previousAB;
bool lastRawButton = HIGH, stableButton = HIGH;
uint32_t buttonChangedAt = 0, lastTransmit = 0;

struct PedalButton {
  uint8_t pin;
  bool raw = HIGH, stable = HIGH;
  uint32_t changedAt = 0;
  explicit PedalButton(uint8_t buttonPin) : pin(buttonPin) {}
  void begin() { pinMode(pin, INPUT_PULLUP); }
  void update(uint32_t now) {
    const bool reading = digitalRead(pin);
    if (reading != raw) { raw = reading; changedAt = now; }
    if (now - changedAt >= DEBOUNCE_MS) stable = raw;
  }
  bool pressed() const { return stable == LOW; }
};
PedalButton accelerator{ACCEL_PIN}, brake{BRAKE_PIN};
int lastAccel = -1, lastBrake = -1;

void transmitControls(int accel, int braking) {
  // Keep the original format for compatibility with the v1 simulator.
  Serial.print("STEER:"); Serial.println(steering);
  // Atomic v2 snapshot: steering, accelerator, brake (0=released, 1=pressed).
  Serial.print("CTRL:"); Serial.print(steering);
  Serial.print(","); Serial.print(accel);
  Serial.print(","); Serial.println(braking);
}

uint8_t readAB() {
  return (digitalRead(CLK_PIN) << 1) | digitalRead(DT_PIN);
}

void setup() {
  pinMode(CLK_PIN, INPUT_PULLUP);
  pinMode(DT_PIN, INPUT_PULLUP);
  pinMode(SW_PIN, INPUT_PULLUP);
  accelerator.begin();
  brake.begin();
  previousAB = readAB();
  Serial.begin(115200);
  // The first loop sends the initial snapshot; subsequent snapshots heartbeat.
}

void loop() {
  const uint32_t now = millis();
  accelerator.update(now);
  brake.update(now);
  const uint8_t ab = readAB();
  if (ab != previousAB) {
    // If both bits jump at once, discard the incomplete click.
    if ((ab ^ previousAB) == 3) partial = 0;
    else partial += TRANSITIONS[(previousAB << 2) | ab];
    previousAB = ab;
    if (partial >= TRANSITIONS_PER_CLICK || partial <= -TRANSITIONS_PER_CLICK) {
      int direction = partial > 0 ? 1 : -1;
      if (REVERSE_ENCODER) direction = -direction;
      steering = constrain(steering + direction * STEERING_STEP, -100, 100);
      partial = 0;
    }
  }
  const bool rawButton = digitalRead(SW_PIN);
  if (rawButton != lastRawButton) {
    lastRawButton = rawButton;
    buttonChangedAt = now;
  }
  if (rawButton != stableButton && now - buttonChangedAt >= DEBOUNCE_MS) {
    stableButton = rawButton;
    if (stableButton == LOW) {
      steering = 0;
      partial = 0;
    }
  }
  // Limit change transmissions to 50 Hz; heartbeat every 100 ms.
  // No delay(): keep sampling the encoder between transmissions.
  const int braking = brake.pressed() ? 1 : 0;
  const int accel = accelerator.pressed() ? 1 : 0;
  // Transmit raw debounced button states so the browser can enforce release
  // before rearming after a crash/reset. The browser gives braking priority.
  const bool changed = steering != lastSent || accel != lastAccel || braking != lastBrake;
  if ((changed && now - lastTransmit >= 20) || now - lastTransmit >= 100) {
    transmitControls(accel, braking);
    lastSent = steering;
    lastAccel = accel;
    lastBrake = braking;
    lastTransmit = now;
  }
}
