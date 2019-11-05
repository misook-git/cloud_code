#include "iot_configs.h"
#include <AzureIoTHub.h>
#if defined(IOT_CONFIG_MQTT)
    #include <AzureIoTProtocol_MQTT.h>
#elif defined(IOT_CONFIG_HTTP)
    #include <AzureIoTProtocol_HTTP.h>
#endif

#include "sample.h"
#include "esp8266/sample_init.h"
#include <Wire.h>
#include <Ticker.h>

static char ssid[] = IOT_CONFIG_WIFI_SSID;
static char pass[] = IOT_CONFIG_WIFI_PASSWORD;

static String ssidA, passA, idA;
static String m_CO2, m_tVOC, a_CO2, a_tVOC, m_tempC, m_humiP, m_pressure, m_altitude;
static String m_pm1_0, m_pm1_0MIN, m_pm1_0MAX;
static String m_pm2_5, m_pm2_5MIN, m_pm2_5MAX;
static String m_pm10, m_pm10MIN, m_pm10MAX;
static String m_latitudeA, m_longitudeA;

static bool toSend = false; // Send or Not : Azure Function

Ticker tickerA;
int tickTime = 0;

void setup() {
  sample_init(ssid, pass);
  delay(3000);
  Serial.begin(115200);

  pinMode(PinA, OUTPUT);
  digitalWrite(PinA, LOW);

  Wire.begin(2,14);
  Wire.setClock(400000L);
  Wire.setClockStretchLimit(1000000);

  readAccessInfo();
  initClient();
  tickerA.attach(10, getDustPressTempHumiCO2TVOCData);
}

// Azure IoT samples contain their own loops, so only run them once
void loop() {

  doWork();

  if(!toSend) return;
  sample_run();
  toSend = false;
}

