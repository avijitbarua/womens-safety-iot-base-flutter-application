#include <BLEPeripheral.h>

// Nordic UART Service UUIDs
const char* SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
const char* TX_UUID      = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"; // Notify
const char* RX_UUID      = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"; // Write

BLEPeripheral blePeripheral = BLEPeripheral();
BLEService uartService = BLEService(SERVICE_UUID);
BLECharacteristic txChar = BLECharacteristic(TX_UUID, BLENotify, 20);
BLECharacteristic rxChar = BLECharacteristic(RX_UUID, BLEWrite, 20);

// FSR Pins
const int fsr1Pin = 5;  // P0.05
const int fsr2Pin = 4;  // P0.04
const int fsr3Pin = 3;  // P0.03
const int fsr4Pin = 2;  // P0.02

// Send interval
const unsigned long sendInterval = 200;
unsigned long lastSendTime = 0;

void setup() {
  Serial.begin(9600);
  delay(1000);

  pinMode(fsr1Pin, INPUT);
  pinMode(fsr2Pin, INPUT);
  pinMode(fsr3Pin, INPUT);
  pinMode(fsr4Pin, INPUT);

  blePeripheral.setLocalName("Women's Safety V2");
  blePeripheral.setDeviceName("Women's Safety V2");
  blePeripheral.setAdvertisedServiceUuid(uartService.uuid());

  blePeripheral.addAttribute(uartService);
  blePeripheral.addAttribute(txChar);
  blePeripheral.addAttribute(rxChar);

  blePeripheral.begin();

  Serial.println("BLE 4-FSR Monitor Started");
}

void loop() {
  blePeripheral.poll();

  BLECentral central = blePeripheral.central();

  if (central && central.connected()) {

    if (millis() - lastSendTime >= sendInterval) {

      int fsr1 = analogRead(fsr1Pin);
      int fsr2 = analogRead(fsr2Pin);
      int fsr3 = analogRead(fsr3Pin);
      int fsr4 = analogRead(fsr4Pin);

      Serial.print("FSR1: "); Serial.print(fsr1);
      Serial.print("  FSR2: "); Serial.print(fsr2);
      Serial.print("  FSR3: "); Serial.print(fsr3);
      Serial.print("  FSR4: "); Serial.println(fsr4);

      char buffer[20];

      // Format must stay under 20 bytes (BLE limit)
      sprintf(buffer, "%d,%d,%d,%d", fsr1, fsr2, fsr3, fsr4);

      txChar.setValue((const unsigned char*)buffer, strlen(buffer));

      lastSendTime = millis();
    }
  }
}
