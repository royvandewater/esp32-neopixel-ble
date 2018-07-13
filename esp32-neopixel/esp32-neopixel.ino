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
    if (flourish) {
      delay(1000);
      Serial.println("Flourish");
      uint8_t r = rand() % 255;
      uint8_t g = rand() % 255;
      uint8_t b = rand() % 255;
      ring.setColor(r, g, b);
      ring.flourish();
    } else {
      Serial.println("Pulse");
      ring.pulse();
    }
    flourish = !flourish;
  }

  ring.render();
  delay(50);
}
