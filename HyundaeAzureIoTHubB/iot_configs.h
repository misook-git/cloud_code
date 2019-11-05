
#ifndef IOT_CONFIGS_H
  #define IOT_CONFIGS_H
  #define IOT_CONFIG_WIFI_SSID            "wifi_2.4G"
  #define IOT_CONFIG_WIFI_PASSWORD        "11111111"
  #define IOT_CONFIG_CONNECTION_STRING    "HostName=LLAHub.azure-devices.net;DeviceId=llk001;SharedAccessKey=3IZWKIXuBfmRXefG9XVIgiOaZ+JEIjHOcFSQaRnwuyA="
  #define DEVICE_ID                       "llk001"


  int    i_CO2, i_tVOC;
  int    i_pm1_0, i_pm1_0MIN, i_pm1_0MAX;
  int    i_pm2_5, i_pm2_5MIN, i_pm2_5MAX;
  int    i_pm10, i_pm10MIN, i_pm10MAX;
  float  f_tempC, f_humiP, f_pressure, f_latitudeA, f_longitudeA;

  #define PinA                  4
  
  #define IOT_CONFIG_MQTT
  // #define IOT_CONFIG_HTTP
#endif /* IOT_CONFIGS_H */
