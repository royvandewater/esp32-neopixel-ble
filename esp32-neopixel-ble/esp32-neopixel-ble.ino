
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <PixelRing.h>


// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "2ba75e8a-5b5b-447b-ab9a-b79e21dd64e0"
#define COLOR_CHARACTERISTIC_UUID "08f490bf-28f1-4d55-897d-ab8d74effffb"
#define COMMAND_CHARACTERISTIC_UUID "04b29961-90fd-4ee7-bb48-f203bde84f44"
#define PIN 22

PixelRing ring2 = PixelRing(PIN, 24);
Adafruit_NeoPixel ring = Adafruit_NeoPixel(24, PIN, NEO_GRB + NEO_KHZ800);
uint32_t nextColor = ring.Color(64, 64, 0);
uint32_t currentColor = ring.Color(0, 0, 0);
uint32_t colorCommand = 0;
int32_t brightness = 64;
int32_t brightnessDirection = 1;

class ColorCallback: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();

    if (value.length() >= 3) {
      char buffer [32];
      sprintf(buffer, "New Color: %02x%02x%02x", value[0], value[1], value[2]);
      Serial.println(buffer);
      nextColor = ring.Color(value[0], value[1], value[2]);
    }
  }
};

class CommandCallback: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();

    if (value.length() == 0) {
      return;
    }
    colorCommand = value[0];
    char buffer [32];
    sprintf(buffer, "New command: %d", colorCommand);
    Serial.println(buffer);
  }
};

void setup() {
  Serial.begin(115200);

  Serial.println("Running...");

  BLEDevice::init("esp32-neopixel");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *colorCharacteristic = pService->createCharacteristic(
                                         COLOR_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  colorCharacteristic->setCallbacks(new ColorCallback());

  BLECharacteristic *commandCharacteristic = pService->createCharacteristic(
                                         COMMAND_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  commandCharacteristic->setCallbacks(new CommandCallback());

  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  ring.begin();
  ring.show(); // Initialize all pixels to 'off'
}

void loop() {
  if (nextColor != currentColor) {
    Serial.println("The color changed, time to wipe");
    currentColor = nextColor;
    colorWipe(currentColor, 50);
  }

  colorPulse(currentColor);
  colorRainbow(5);
  theaterChaseRainbow(5);
  rainbowCycle(5);
  delay(50);
}

void colorWipe(uint32_t c, uint8_t wait) {
  ring.setBrightness(255);

  for(uint16_t i=0; i<ring.numPixels(); i++) {
    ring.setPixelColor(i, c);
    ring.show();
    delay(wait);
  }
}

void colorPulse(uint32_t c) {
  if (colorCommand != 1) return;

  brightness += 16 * brightnessDirection;

  if (brightness > 255) {
    brightness = 255;
    brightnessDirection = -1;
  }
  if (brightness < 32) {
    brightness = 32;
    brightnessDirection = 1;
  }

  ring.setBrightness(brightness);
  for(uint16_t i=0; i<ring.numPixels(); i++) {
    ring.setPixelColor(i, c);
  }
  ring.show();
}

void colorRainbow(uint8_t wait) {
  if (colorCommand != 2) return;
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<ring.numPixels(); i++) {
      ring.setPixelColor(i, Wheel((i+j) & 255));
    }
    ring.show();
    delay(wait);
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  if (colorCommand != 3) return;

  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < ring.numPixels(); i=i+3) {
        ring.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      ring.show();

      delay(wait);

      for (uint16_t i=0; i < ring.numPixels(); i=i+3) {
        ring.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

void rainbowCycle(uint8_t wait) {
  if (colorCommand != 4) return;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< ring.numPixels(); i++) {
      ring.setPixelColor(i, Wheel(((i * 256 / ring.numPixels()) + j) & 255));
    }
    ring.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return ring.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return ring.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return ring.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
