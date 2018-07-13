#include <PixelRing.h>
#define PIN 22

bool flourish = false;
PixelRing ring = PixelRing(PIN, 22);

void setup() {
  Serial.begin(115200);
  Serial.println("Running...");

  ring.begin();
}

void loop() {
  if (ring.animationComplete()) {
    delay(1000);
    uint8_t r = rand() % 255;
    uint8_t g = rand() % 255;
    uint8_t b = rand() % 255;

    if (flourish) {
      Serial.println("Flourish");
      ring.flourish(r, g, b);
    } else {
      Serial.println("Pulse");
      ring.pulse(r, g, b);
    }
    flourish = !flourish;
  }

  ring.render();
  delay(50);
}
