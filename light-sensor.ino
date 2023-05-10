#include <Wire.h>
#include <BH1750.h>
#include <MKRWAN.h>
#include <ArduinoJson.h>
#include "arduino_secrets.h"

BH1750 lightMeter;
int ledPin = 0;

LoRaModem modem;
String appEui = "0000000000000000";
String appKey = "4DE8F1A874E276F98D26E29F1E4E258F";

float brightness;
int brightness_perc;
int power;

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

  // Initialise the I2C bus
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

  // Typical Sunrise/Sunset reading is 400 lux
  // Therefore any brighter than this leads to streetlight being switched off
  // Otherwise inverse scale the brightness between 0 - 255 (8-bit PWM)
  if (lux < 400) {
    brightness = (lux / 1.57) - 255;
    brightness = abs(brightness);
  } else {
    brightness = 0;
  }

  // store percentage level of brightness
  brightness_perc = int((brightness / 255) * 100);

  // Calculating power consumption with a PWM signal is:
  //   Power Consumption = Duty Cycle * Load Power
  //     Duty Cycle == Brightness Percentage in this case
  //     Load Power == Operating voltage * Operating current - in this case 2.3V and 0.02A
  float load_power = 2.3 * 0.02;
  power = int(brightness_perc * load_power);

  Serial.print("Brightness(%): ");
  Serial.println(brightness_perc);
  Serial.print("Power(W): ");
  Serial.println(power);
  analogWrite(ledPin, brightness);

}

/**
* Communicates brightness and power to application server using LoRa
* Strictly uplink messaging (messages aren't received from TTN)
* Adapted from:
*   https://docs.arduino.cc/tutorials/mkr-wan-1310/mkr-wan-library-examples
*/
void loop_lora() {

  StaticJsonDocument<200> stats;
  stats["brightness"] = brightness_perc;
  stats["power"] = power;

  // Serialize the JSON object to a byte array
  String jsonString;
  serializeJson(stats, jsonString);

  Serial.print("Sending: " + jsonString + " - ");

  int err;
  modem.beginPacket();
  modem.print(jsonString);
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
    Serial.println("No manual config!");
    return;
  }
  char rcv[64];
  int i = 0;
  while (modem.available()) {
    rcv[i++] = (char)modem.read();
  }
  Serial.print("Received: ");
  StaticJsonDocument<200> resp;
  DeserializationError error = deserializeJson(resp, jsonString);

  // Check for parsing errors
  if (error) {
    Serial.print("JSON parsing failed: ");
    Serial.println(error.c_str());
    return;
  }

  int b = resp["brightness"];

  Serial.print(b);
  Serial.println();

}
