#include <PixelRing.h>


// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define PIN 22

PixelRing ring = PixelRing(PIN, 24);
uint32_t nextColor = ring.Color(64, 64, 0);
uint32_t currentColor = ring.Color(0, 0, 0);
uint32_t colorCommand = 0;
int32_t brightness = 64;
int32_t brightnessDirection = 1;

void setup() {
  Serial.begin(115200);
  Serial.println("Running...");

  ring.render();
}

void loop() {
  if (nextColor != currentColor) {
    Serial.println("The color changed, time to flourish");
    currentColor = nextColor;
    ring.flourish(currentColor);
  }

  ring.render();
  delay(50);
}
