#include <Wire.h>
#include <BH1750.h>
#include <MKRWAN.h>
#include "arduino_secrets.h"

BH1750 lightMeter;
int ledPin = 0;

LoRaModem modem;
String appEui = "0000000000000000";
String appKey = "4DE8F1A874E276F98D26E29F1E4E258F";

int brightness;

void setup(){

  setup_lora();

  setup_light_sensor();

}

void loop() {

  loop_light_sensor();

  delay(60000);  // send brightness once a minute

  loop_lora();

}

/**
* Initialises the light sensor
* LED out pin supports PWM to modify brightness
*/
void setup_light_sensor() {

  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);

  // Initialize the I2C bus
  Wire.begin();

  lightMeter.begin();

}

/**
* Initialises connection to TTN
* Adapted from:
*   https://docs.arduino.cc/tutorials/mkr-wan-1310/mkr-wan-library-examples
*/
void setup_lora() {

  Serial.begin(115200);
  while (!Serial);
  // change this to your regional band (eg. US915, AS923, ...)
  if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
    while (1) {}
  };
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());

  int connected = modem.joinOTAA(appEui, appKey);
  if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    while (1) {}
  }

  // Set poll interval to 60 secs.
  modem.minPollInterval(60);

}

/**
* Measures the light level
* Adapted from:
*   https://github.com/claws/BH1750
*/
void loop_light_sensor() {

  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

  // scale the brightness to be in 8 bit range ( 0 - 255 )
  // inverse scale - lower brightness -> brighter led
  brightness = abs((lux / 3.5) - 255);
  Serial.print("Brightness: ");
  Serial.println(brightness);
  analogWrite(ledPin, brightness);

}

/**
* Communicates brightness to application server using LoRa
* Adapted from:
*   https://docs.arduino.cc/tutorials/mkr-wan-1310/mkr-wan-library-examples
*/
void loop_lora() {

  // send brightness LoRa message
  String msg = String(brightness);
  Serial.print("Sending: " + msg + " - ");
  for (unsigned int i = 0; i < msg.length(); i++) {
    Serial.print(msg[i] >> 4, HEX);
    Serial.print(msg[i] & 0xF, HEX);
    Serial.print(" ");
  }
  Serial.println();

  int err;
  modem.beginPacket();
  modem.print(msg);
  err = modem.endPacket(true);
  if (err > 0) {
    Serial.println("Message sent correctly!");
  } else {
    Serial.println("Error sending message :(");
    Serial.println("(you may send a limited amount of messages per minute, depending on the signal strength");
    Serial.println("it may vary from 1 message every couple of seconds to 1 message every minute)");
  }
  delay(1000);
  if (!modem.available()) {
    Serial.println("No downlink message received at this time.");
    return;
  }
  char rcv[64];
  int i = 0;
  while (modem.available()) {
    rcv[i++] = (char)modem.read();
  }
  Serial.print("Received: ");
  for (unsigned int j = 0; j < i; j++) {
    Serial.print(rcv[j] >> 4, HEX);
    Serial.print(rcv[j] & 0xF, HEX);
    Serial.print(" ");
  }
  Serial.println();

}
