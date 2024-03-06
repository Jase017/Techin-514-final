#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <SwitecX25.h>

// 定义要连接的远程服务和特征的UUID
static BLEUUID serviceUUID("ada5d9ba-3acf-4462-8f48-a6f49df86fc1");
static BLEUUID charUUID("43ef269e-e530-485d-99d0-3cc5a2df9268");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;
static String Rdata;

// standard X25.168 ranges from 0 to 315 degrees, at 1/3 degree increments
#define STEPS 945

const int stage0 = 900;
const int stage1 = 835;
const int stage2 = 770;
const int stage3 = 705;
const int stage4 = 635;
const int stage5 = 570;
const int stage6 = 505;
const int stage7 = 440;
const int stage8 = 380;
const int stage9 = 315;

// For motors connected to D0, D1, D2, D3
SwitecX25 motor1(STEPS, D0, D1, D2, D3);

// 通知回调函数
static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");

    String receivedData = "";
    for (int i = 0; i < length; i++) {
        receivedData += (char)pData[i];
    }

    Serial.println(receivedData);
    Rdata = receivedData;
}

// BLE客户端回调
class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());

    BLEClient* pClient = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // 连接到远程BLE服务器
    pClient->connect(myDevice);
    Serial.println(" - Connected to server");

    // 获取远程BLE服务器上的服务
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");

    // 获取服务中的特征
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;
    return true;
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void loop() {
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server.");
    }
    doConnect = false;
  }

  if (connected) {
    // 如果需要的话，这里可以发送数据到服务器
  } else if (doScan) {
    BLEDevice::getScan()->start(0);  // 重新开始扫描
  }
  if(Rdata == "0"){
    motor1.setPosition(stage0);
  while(motor1.currentStep != motor1.targetStep){
    motor1.update();
  }
  }
  if(Rdata == "1"){
    motor1.setPosition(stage1);
  while(motor1.currentStep != motor1.targetStep){
    motor1.update();
  }
  }
  if(Rdata == "2"){
    motor1.setPosition(stage2);
  while(motor1.currentStep != motor1.targetStep){
    motor1.update();
  }
  }if(Rdata == "3"){
    motor1.setPosition(stage3);
  while(motor1.currentStep != motor1.targetStep){
    motor1.update();
  }
  }if(Rdata == "4"){
    motor1.setPosition(stage4);
  while(motor1.currentStep != motor1.targetStep){
    motor1.update();
  }
  }if(Rdata == "5"){
    motor1.setPosition(stage5);
  while(motor1.currentStep != motor1.targetStep){
    motor1.update();
  }
  }if(Rdata == "6"){
    motor1.setPosition(stage6);
  while(motor1.currentStep != motor1.targetStep){
    motor1.update();
  }
  }if(Rdata == "7"){
    motor1.setPosition(stage7);
  while(motor1.currentStep != motor1.targetStep){
    motor1.update();
  }
  }if(Rdata == "8"){
    motor1.setPosition(stage8);
  while(motor1.currentStep != motor1.targetStep){
    motor1.update();
  }
  }if(Rdata == "9"){
    motor1.setPosition(stage9);
  while(motor1.currentStep != motor1.targetStep){
    motor1.update();
  }
  }
  delay(1000);
  

  }
  