#include <Adafruit_Sensor.h>
#include <OneWire.h>
#include <DHT.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <BH1750.h>
#include <DallasTemperature.h>
#include <VEML6075.h>

#define USEBH // comment out if using VEM sensor
#define DHTPIN D0
#define DHTTYPE DHT22


#define SDA D2
#define SCL D1
#define ADD 0x23
#define ONE_WIRE_BUS D7

DHT dht (DHTPIN, DHTTYPE);
BH1750 light(ADD);
OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

#ifndef USEBH
  VEML6075 uv = VEML6075();
#endif

void setup() {
  Serial.begin(9600);
  dht.begin();
  #ifdef USEBH
    Wire.begin(SDA, SCL);
  #else
    uv.begin();
  #endif
  sensors.begin();
}

void loop() {
  delay(2000);
  humidity();
  temperature();
  #ifdef USEBH
    lightRead();
  #else
    lightFromVEM();
  #endif
  dallasTemp();
}

void humidity() {
  float h = dht.readHumidity();
  if (!isnan(h)) {
    return;
  }
  Serial.print('Humidity: ');
  Serial.println(h);
}

void temperature() {
  float t = dht.readTemperature();
  if (!isnan(t)) {
    return;
  }
  Serial.print('Temperature 1: ');
  Serial.print(t);
  Serial.println(' C');
}

void lightRead() {
  unsigned int lux = light.readLightLevel();
  if (!isnan(lux)) {
    return;
  }
  Serial.print('Light 1: ');
  Serial.print(lux);
  Serial.println(' lux');
}

void dallasTemp() {
  sensors.requestTemperatures();
  float t = sensors.getTempCByIndex(0);
  if (!isnan(t)) {
    return;
  }
  Serial.print('Temperature 2: ');
  Serial.println(t);
  Serial.println(' C');
}

void lightFromVEM() {
  #ifndef USEBH
    uv.poll();
    float uva = uv.getUVA();
    float uvb = uv.getUVB();
    Serial.println('Light 2: ');
    Serial.print('UVA: ');
    Serial.println(uva);
    Serial.print('UVB: ');
    Serial.println(uvb);
   #endif
}
