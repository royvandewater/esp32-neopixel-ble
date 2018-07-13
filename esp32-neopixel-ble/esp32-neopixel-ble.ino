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

PixelRing ring = PixelRing(PIN, 22);
uint32_t red = 0, green = 0, blue = 0;

class ColorCallback: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();

    if (value.length() >= 3) {
      if (red == value[0] && green == value[1] && blue == value[2]) return;

      red = value[0];
      green = value[1];
      blue = value[2];

      char buffer [32];
      sprintf(buffer, "New Color: %02x%02x%02x", red, green, blue);
      Serial.println(buffer);

      ring.setColor(red, green, blue);
      ring.flourish();
    }
  }
};

class CommandCallback: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();

    if (value.length() == 0) {
      return;
    }
    uint32_t colorCommand = value[0];

    char buffer [32];
    sprintf(buffer, "New command: %d", colorCommand);
    Serial.println(buffer);

    if (colorCommand == 1) ring.pulse();
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
}

void loop() {
  ring.render();
  delay(50);
}
