#include <PixelRing.h>
#define PIN 22

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
    ring.flourish(r, g, b);
  }

  ring.render();
  delay(50);
}
