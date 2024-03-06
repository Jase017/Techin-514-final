#include <Encoder.h>
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <stdlib.h>

// 旋转编码器的引脚
Encoder myEnc(7, 9);

// 定义按钮和LED的引脚
const int buttonPin = 2;
const int ledPin = 3;

// 用于检测按钮状态变化的变量
int buttonState = 0;         
int lastButtonState = 0;     

// 定义编码器是否启用的标志
bool encoderEnabled = false;

int num = 0;

// BLE变量
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
unsigned long previousMillis = 0;
const long interval = 1000;

#define SERVICE_UUID        "ada5d9ba-3acf-4462-8f48-a6f49df86fc1"
#define CHARACTERISTIC_UUID "43ef269e-e530-485d-99d0-3cc5a2df9268"

// 用于BLE连接状态的回调类
class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
    }
};

// newPosition作为全局变量
long newPosition = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE work!");

    // 初始化按钮和LED的引脚
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);

    // 初始化BLE设备
    BLEDevice::init("XIAO_ESP32S3");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharacteristic->addDescriptor(new BLE2902());

    pCharacteristic->setValue(String(newPosition).c_str());
    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);  
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
    // 读取按钮当前状态
    buttonState = digitalRead(buttonPin);
    
    // 如果按钮状态从未按下变为按下
    if (buttonState != lastButtonState) {
    if (buttonState == LOW) { // 注意这里的条件改为 LOW，匹配上拉按钮按下的状态
        encoderEnabled = !encoderEnabled;
        digitalWrite(ledPin, encoderEnabled ? HIGH : LOW);
    }
    lastButtonState = buttonState;
}

    if (encoderEnabled) {
        // 仅当编码器启用时读取新位置
        long tempPosition = myEnc.read();
        if (tempPosition != newPosition) {
            newPosition = tempPosition;
            num = (newPosition/2500);
            Serial.println(num);
        }
    }

    // BLE连接逻辑
    if (deviceConnected) {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
            pCharacteristic->setValue(String(num).c_str());
            pCharacteristic->notify();
            Serial.print("Notify value: ");
            Serial.println(num);
            previousMillis = currentMillis;
        }
    }

    if (!deviceConnected && oldDeviceConnected) {
        delay(500);
        pServer->startAdvertising();
        Serial.println("Start advertising");
        oldDeviceConnected = deviceConnected;
    }

    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
}
