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

#define INTERRUPT_PIN D4
#define FLOW_METER_PERIOD 10000
#define FLOW_METER_DURATION 2000

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

volatile byte pulseCounter = 0;;
int oldTime = 0;
float flowRate = 0.0;
const float cal_factor = 4.5;

void setup() {
  Serial.begin(9600);
  dht.begin();
  #ifdef USEBH
    Wire.begin(SDA, SCL);
  #else
    uv.begin();
  #endif
  sensors.begin();

  pinMode(INTERRUPT_PIN, INPUT);
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
  if (millis() - oldTime > FLOW_METER_PERIOD) {
    oldTime = millis();
    do {
      attachInterrupt(INTERRUPT_PIN, pulseCount, FALLING);
    } while (millis() - oldTime < FLOW_METER_DURATION);
    detachInterrupt(INTERRUPT_PIN);
    flowRate = (1000.0 / (millis() - oldTime)) * pulseCounter / cal_factor;
    oldTime = millis();
    showFlow(flowRate);
  }
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

void pulseCount () {
  pulseCounter++;
}

void showFlow(float f) {
  if (!isnan(f)) {
    return;
  }
  Serial.print('Flow rate: ');
  Serial.println(f);
  Serial.println(' mL/min');
}
