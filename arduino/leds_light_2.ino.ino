#include <Adafruit_NeoPixel.h>
#include "Timer.h"     //to be able to use a timer
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <string>

#define SERVICE_UUID           "2ef8decc-c001-4e2c-8d97-6e9aa61e30e4" // BookBuddy service UUID
#define CHARACTERISTIC_UUID_RX1 "2ef8decc-c002-4e2c-8d97-6e9aa61e30e4"
#define CHARACTERISTIC_UUID_RX2 "2ef8decc-c004-4e2c-8d97-6e9aa61e30e4"
#define CHARACTERISTIC_UUID_TX "2ef8decc-c003-4e2c-8d97-6e9aa61e30e4"
#define TOTAL_READ_TIME_UUID   "2ef8decc-c005-4e2c-8d97-6e9aa61e30e4"

#define PIN_NEO_PIXEL    2   // Arduino pin that connects to NeoPixel
#define NUM_PIXELS      8  // The number of LEDs (pixels) on NeoPixel
#define PIN_VIBRATOR    1 // For vibrator

Timer timer;
Timer timer2;
int pixelsOn;
int currentTimeRead;
int savedTimeRead = 0;
int timeGoal = 30000; //goal till all LEDs are full
int debugNum = 0; // for debugging

bool ledStatus[NUM_PIXELS] = {false}; // Array to store the status of each LED
bool isUpdatingTimeGoal = false;

// the setup function runs once when you press reset or power the board

Adafruit_NeoPixel NeoPixel(NUM_PIXELS, PIN_NEO_PIXEL, NEO_GRB + NEO_KHZ800);

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
BLECharacteristic * pTotalReadTimeCharacteristic;
bool deviceConnected = false;
uint8_t txValue = 0;

//Bluetooth connect/disconnect processing. Triggered automatically when a connect/disconnect event occurs
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {   //This function will be executed when Bluetooth is connected
      Serial.println("Bluetooth connected");
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {  //This function will be executed when Bluetooth is disconnected
      Serial.println("Bluetooth disconnected");
      deviceConnected = false;
      delay(500); // give the bluetooth stack the chance to get things ready
      pServer->startAdvertising(); // restart advertising
    };

    void onStart(BLEServer *pServer) {
      Serial.println("Advertising started");
    }

    void onStop(BLEServer *pServer) {
      Serial.println("Advertising stopped");
    }
};

/****************Data receiving section*************/
/****************************************/
//Bluetooth receive data processing. Triggered automatically when data is received
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      
      std::string rxValue = pCharacteristic->getValue();//Receive data and assign it to rxValue

      //if(rxValue == "ON"){Serial.println("开灯");}   //Determine whether the received character is "ON"

      // Process the first characteristic (CHARACTERISTIC_UUID_RX1)
      if (pCharacteristic->getUUID().equals(BLEUUID(CHARACTERISTIC_UUID_RX1))) {
        debugNum = 1;
        if (rxValue.length() > 0) {
          Serial.println("*********");
          Serial.print("Received Value: ");
          for (int i = 0; i < rxValue.length(); i++){
            Serial.print(rxValue[i]);
          }
          Serial.println();
          

          timeGoal = std::stoi(rxValue) * 1000;
          Serial.printf("New Time Goal: %d\n", timeGoal);
          Serial.println("*********");
          
          isUpdatingTimeGoal = true;
          Serial.println(isUpdatingTimeGoal);
          timer.stop();       //stop timer
          currentTimeRead = 0;
          savedTimeRead = 0;

          Serial.print("timer state during resetting timegoal: ");
          Serial.print(timer.state());
          Serial.println();

          NeoPixel.clear();   //turn off leds
          NeoPixel.show();

          for (int i = 1; i < NUM_PIXELS; i++) {
            ledStatus[i] = false;
          }

          for (int i = 0; i < 3; i++) {
            for (int pixel = 1; pixel < NUM_PIXELS; pixel++) {
              NeoPixel.setPixelColor(pixel, NeoPixel.Color(255, 0, 255));
            }
            NeoPixel.show();
            delay(500);
            for (int pixel = 1; pixel < NUM_PIXELS; pixel++) {
              NeoPixel.setPixelColor(pixel, NeoPixel.Color(0, 0, 0));
            }
            NeoPixel.show();
            delay(500);
          }

          isUpdatingTimeGoal = false;
        }
        Serial.println("end resetting timegoal");
      }
      
      // Process the second characteristic (CHARACTERISTIC_UUID_RX2)
      if (pCharacteristic->getUUID().equals(BLEUUID(CHARACTERISTIC_UUID_RX2))) {
        if (rxValue.length() > 0) {
          Serial.println("*********");
          Serial.print("Received Value: ");
          for (int i = 0; i < rxValue.length(); i++){
            Serial.print(rxValue[i]);
          }
          Serial.println();
          Serial.println("*********");

          if (rxValue == "friendReading") {
            timer2.start();
            digitalWrite(PIN_VIBRATOR, HIGH);

            while (timer2.read() <= 2000) {
              if (timer2.read() % 500 == 0) {
                if (timer2.read() % 250 == 0) {
                  NeoPixel.setPixelColor(0, NeoPixel.Color(255, 0, 255));
                  NeoPixel.show();
                }
                NeoPixel.setPixelColor(0, NeoPixel.Color(0, 0, 0));
                NeoPixel.show();
              }
            }
            digitalWrite(PIN_VIBRATOR, LOW);
            timer2.stop();

            ledStatus[0]= true;
            NeoPixel.setPixelColor(0, NeoPixel.Color(0, 0, 255));
            NeoPixel.show();
          }
          if (rxValue == "friendStopReading") {
            timer2.start();
            digitalWrite(PIN_VIBRATOR, HIGH);

            while (timer2.read() <= 2000) {
              if (timer2.read() % 500 == 0) {
                if (timer2.read() % 250 == 0) {
                  NeoPixel.setPixelColor(0, NeoPixel.Color(255, 0, 255));
                  NeoPixel.show();
                }
                NeoPixel.setPixelColor(0, NeoPixel.Color(0, 0, 0));
                NeoPixel.show();
              }
            }
            digitalWrite(PIN_VIBRATOR, LOW);
            timer2.stop();

            ledStatus[0]= false;
            NeoPixel.setPixelColor(0, NeoPixel.Color(0, 0, 0));
            NeoPixel.show();
          }
        }
      }

    }
};

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  NeoPixel.begin(); 
  Serial.begin(115200);
  analogReadResolution(12);
  pinMode(8, OUTPUT);
  pinMode(PIN_VIBRATOR, OUTPUT);

  BLEBegin();  //Initialize Bluetooth
}

void BLEBegin(){
  // Create the BLE Device
  BLEDevice::init(/*BLE name*/"BookBuddy");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLESecurity *pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
                    CHARACTERISTIC_UUID_TX,
                    BLECharacteristic::PROPERTY_NOTIFY
                  );

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic1 = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_RX1,
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pRxCharacteristic1->setCallbacks(new MyCallbacks());

  BLECharacteristic * pRxCharacteristic2 = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_RX2,
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pRxCharacteristic2->setCallbacks(new MyCallbacks());

  pTotalReadTimeCharacteristic = pService->createCharacteristic(
                                    TOTAL_READ_TIME_UUID,
                                    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
                                  );

  pTotalReadTimeCharacteristic->addDescriptor(new BLE2902());
  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

// the loop function runs over and over again forever

void loop() {
  digitalWrite(8, HIGH);
  int analogValue = analogRead(A4);       //read light sensor
  //Serial.printf("ADC millivolts value = %d\n",analogValue);

  if(analogValue < 100 && timer.state()== RUNNING && isUpdatingTimeGoal == false){    //checks if it is dark 
    timer.stop();       //stop timer
    savedTimeRead = savedTimeRead + currentTimeRead;
    currentTimeRead = 0;
    
    // for (int i = 0; i < NUM_PIXELS; i++) { // reset ALL LED lights
    //     ledStatus[i] = false;
    // }
    NeoPixel.clear();   //turn off leds
    NeoPixel.show();
    if (deviceConnected) {
      pTxCharacteristic->setValue("closed");  //Send string
      pTxCharacteristic->notify();
      delay(10); // bluetooth stack will go into congestion, if too many packets are sent
    } 

  } else if (analogValue >= 100 && timer.state() == STOPPED && isUpdatingTimeGoal == false){
      if (deviceConnected) {
        pTxCharacteristic->setValue("open");  //Send string
        pTxCharacteristic->notify();
        delay(10); // bluetooth stack will go into congestion, if too many packets are sent
      } 

      for (int pixel = 0; pixel < NUM_PIXELS; pixel ++) {
        if (ledStatus[pixel] == true) {
          NeoPixel.setPixelColor(pixel, NeoPixel.Color(153, 0, 153));
          if (pixel == 0) {
            NeoPixel.setPixelColor(pixel, NeoPixel.Color(0, 0, 255));
          }
          Serial.printf("pixel that is true: %d\n", pixel);
        }
      }
      NeoPixel.show();
      timer.start();
    
  } else if (timer.state() == RUNNING && isUpdatingTimeGoal == false){ //checks if it's light again and starts timer
      currentTimeRead = timer.read();
      int totalTime = 0;
      if (currentTimeRead != 0) {
        totalTime = savedTimeRead + currentTimeRead;
      }
      
      // if (deviceConnected) {
      //   pTxCharacteristic->setValue(totalTime);  //Send string
      //   pTxCharacteristic->notify();
      //   delay(10); // bluetooth stack will go into congestion, if too many packets are sent
      // } 
      if(totalTime <= timeGoal){
        pixelsOn = floor(totalTime/(timeGoal/NUM_PIXELS-1)); //calculate amount of pixels that should be on
      } else{
        pixelsOn = NUM_PIXELS;
      }

      // turn pixels to green one by one with delay between each pixel
      for (int n = 1; n < pixelsOn; n++) {           // for each pixel
        int pixel;
        if (n == 1) {
          pixel = 6;
        } else if (n == 2) {
          pixel = 1;
        } else if (n == 3) {
          pixel = 7;
        } else if (n == 4) {
          pixel = 5;
        } else if (n == 5) {
          pixel = 2;
        } else if (n == 6) {
          pixel = 4;
        } else if (n == 7) {
          pixel = 3;
        }
        if (ledStatus[pixel] == false && isUpdatingTimeGoal == false) {
          
          ledStatus[pixel] = true;
          NeoPixel.setPixelColor(pixel, NeoPixel.Color(153, 0, 153));  // it only takes effect if pixels.show() is called
          NeoPixel.show();                                           // send the updated pixel colors to the NeoPixel hardware.
          Serial.printf("pixel one by one: %d\n", pixel);
        }
      }
  }

  static unsigned long lastTotalTimeUpdate = 0;
  if (millis() - lastTotalTimeUpdate >= 1000) {
    lastTotalTimeUpdate = millis();
    int totalTime = floor((savedTimeRead + currentTimeRead) / 1000.0);
    Serial.println(totalTime);
    pTotalReadTimeCharacteristic->setValue(totalTime);
    pTotalReadTimeCharacteristic->notify();
  }  

}
