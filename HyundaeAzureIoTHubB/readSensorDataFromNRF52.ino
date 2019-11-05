// Data from BLE Module
//I2C write 0x00 String pthData = "p,005,006,007,29.2,55.6";     //PM1.0, PM2.5, PM10, Temp, Humi
//I2C write 0x10 String minPmaxP = "m,005,006,006,005,006,007";  //min PM1.0, PM2.5, PM10, max PM1.0, PM2.5, PM10

static void readAccessInfo() {
  //--------------------------- WiFi SSID
  Wire.beginTransmission(0x1F);
  Wire.write(0x01);
  Wire.endTransmission();
  Wire.requestFrom(0x1F, 20);
  while(Wire.available())    // I2C통신을 통해 데이터가 전송되었는지 검사
  { 
    char c = Wire.read(); // 전송되었을 경우 데이터를 char형으로 읽는다    
    //if(c != 0x00)
    ssidA += c;
  }
  ssidA.remove(0,2);
  Serial.println("");
  Serial.print(ssidA);
  Serial.println("");
  //--------------------------- WiFi Password
  Wire.beginTransmission(0x1F);
  Wire.write(0x02);
  Wire.endTransmission();
  Wire.requestFrom(0x1F, 20);
  while(Wire.available())
  { 
    char c = Wire.read();
    //if(c != 0x00)
    passA += c;
  }
  passA.remove(0,2);
  Serial.print(passA);
  Serial.println("");
  //--------------------------- ID
  Wire.beginTransmission(0x1F);
  Wire.write(0x03);
  Wire.endTransmission();
  Wire.requestFrom(0x1F, 20);
  while(Wire.available())
  { 
    char c = Wire.read();
    if(c != 0x00)
    idA += c;
  }
  idA.remove(0,2);
  Serial.print(idA);
  Serial.println("");
  //--------------------------- Latitude
  Wire.beginTransmission(0x1F);
  Wire.write(0x07);
  Wire.endTransmission();
  Wire.requestFrom(0x1F, 20);
  while(Wire.available())
  { 
    char c = Wire.read();
    if(c != 0x00)
    m_latitudeA += c;
  }
  m_latitudeA.remove(0,2);
  if (m_latitudeA == NULL) m_latitudeA = "35.000";
  f_latitudeA = m_latitudeA.toInt();
  Serial.println(m_latitudeA);
  //--------------------------- Longitude
  Wire.beginTransmission(0x1F);
  Wire.write(0x08);
  Wire.endTransmission();
  Wire.requestFrom(0x1F, 20);
  while(Wire.available())
  { 
    char c = Wire.read();
    if(c != 0x00)
    m_longitudeA += c;
  }
  m_longitudeA.remove(0,2);
  if (m_longitudeA == NULL) m_longitudeA = "128.000";
  f_longitudeA = m_longitudeA.toFloat();
  Serial.println(m_longitudeA);
}

// Data from BLE Module
//I2C write 0x00 String pthData = "p,005,006,007,29.2,55.6";     //PM1.0, PM2.5, PM10, Temp, Humi
//I2C write 0x10 String minPmaxP = "m,005,006,006,005,006,007";  //min PM1.0, PM2.5, PM10, max PM1.0, PM2.5, PM10

// 0x1F is BLE Module TWI Slave Address
// 0x23 == ',' and 0x70 == 'p' and 0x6d == 'm'
//====================================================================================================================================
static void getDustPressTempHumiCO2TVOCData() {
  m_CO2 = "";
  m_tVOC = "";
  m_tempC = "";
  m_humiP = "";
  m_pressure = "";
  m_pm1_0 = "";
  m_pm2_5 = "";
  m_pm10 = "";
  m_pm1_0MIN = "";
  m_pm2_5MIN = "";
  m_pm10MIN = "";
  m_pm1_0MAX = "";
  m_pm2_5MAX = "";
  m_pm10MAX = "";
 // Dust Sensor, Temperature, Humidity, Pressure, CO2, tVOC Data from RecursiveSoft BLE5 Module
 //----------------------------------------- PM1.0 / PM2.5 / PM10
  Wire.beginTransmission(0x1F);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(0x1F, 25);
  int commaIDX = 0;
  for(int i=0;i<25;i++){
    char c = Wire.read();
    if(c == 0x2c) commaIDX++;
//   Serial.print(c);
    if(c!=0x2c && c!=0x70 && c!=0x00) {  // not comma, not 'p'
      if(commaIDX==1) {
        if(c!='0') m_pm1_0 += c;
        else if(m_pm1_0.length() != 0 && c == '0') m_pm1_0 += c;
      }
      else if(commaIDX==2){
        if(c!='0') m_pm2_5 += c;
        else if(m_pm2_5.length() != 0 && c == '0') m_pm2_5 += c;
      }
      else if(commaIDX==3){
        if(c!='0') m_pm10 += c;
        else if(m_pm10.length() != 0 && c == '0') m_pm10 += c;
      }
    }
  }

  // change to int
  i_pm1_0 = m_pm1_0.toInt();
  i_pm2_5 = m_pm2_5.toInt();
  i_pm10 = m_pm10.toInt();
  
  //----------------------------------------- Read Minimum, Maximum of PM1.0 / PM2.5 / PM10
  Wire.beginTransmission(0x1F);
  Wire.write(0x10);
  Wire.endTransmission();
  Wire.requestFrom(0x1F, 25);

  commaIDX = 0;

  for(int i=0;i<25;i++){
    char c = Wire.read();
    if(c == 0x2c) commaIDX++;
    if(c!=0x2c && c!=0x6d && c!=0x00){
      if(commaIDX==1) {
        if(c!='0') m_pm1_0MIN += c;
        else if(m_pm1_0MIN.length() !=0 && c == '0') m_pm1_0MIN += c;
     }
     else if(commaIDX==2) {
        if(c!='0') m_pm2_5MIN += c;
        else if(m_pm2_5MIN.length() !=0 && c == '0') m_pm2_5MIN += c;
     }
     else if(commaIDX==3) {
        if(c!='0') m_pm10MIN += c;
        else if(m_pm10MIN.length() !=0 && c == '0') m_pm10MIN += c;
     }
     else if(commaIDX==4) {
        if(c!='0') m_pm1_0MAX += c;
        else if(m_pm1_0MAX.length() !=0 && c == '0') m_pm1_0MAX += c;
      }
      else if(commaIDX==5) {
        if(c!='0') m_pm2_5MAX += c;
        else if(m_pm2_5MAX.length() !=0 && c == '0') m_pm2_5MAX += c;
      }
      else if(commaIDX==6) {
        if(c!='0') m_pm10MAX += c;
        else if(m_pm10MAX.length() !=0 && c == '0') m_pm10MAX += c;
      }
    }
  }
  
  //change to int
  i_pm1_0MIN = m_pm1_0MIN.toInt();
  i_pm2_5MIN = m_pm2_5MIN.toInt();
  i_pm10MIN = m_pm10MIN.toInt();
  i_pm1_0MAX = m_pm1_0MAX.toInt();
  i_pm2_5MAX = m_pm2_5MAX.toInt();
  i_pm10MAX = m_pm10MAX.toInt();
  
  //----------------------------------------- CO2, tVOC
  Wire.beginTransmission(0x1F);
  Wire.write(0x0C);  // CO2, tVOC
  Wire.endTransmission();
  Wire.requestFrom(0x1F, 13);

  commaIDX = 0;

  for(int i=0;i<13;i++){
    char c = Wire.read();
    if(c == 0x2c) commaIDX++;
    if(c!=0x2c && c!=0x00) {  // not comma, "v,%05d,%05d", m_CO2, m_tVOC : size 13
      if(commaIDX==1) {
        if(c!='0') m_CO2 += c;
        else if(m_CO2.length() != 0 && c == '0') m_CO2 += c;
      }
      else if(commaIDX==2) {
        if(c!='0') m_tVOC += c;
        else if(m_tVOC.length() != 0 && c == '0') m_tVOC += c;
      }
    }
  }
  if(m_CO2.length()>0){
    a_CO2 = m_CO2;
    a_tVOC = m_tVOC;
  } else {
    m_CO2 = a_CO2;
    m_tVOC = a_tVOC;  
  }
  // change to Int
  i_CO2 = m_CO2.toInt();
  i_tVOC = m_tVOC.toInt();
  
  //----------------------------------------- Pressure
  Wire.beginTransmission(0x1F);
  Wire.write(0x0D);  // Pressure
  Wire.endTransmission();
  Wire.requestFrom(0x1F, 13);

  commaIDX = 0;

  for(int i=0;i<13;i++){
    char c = Wire.read();
    if(c == 0x2c) commaIDX++;
    if(c!=0x2c && c!=0x00) {  // not comma, "h,%5.4f", readPressure() : size 12
      if(commaIDX==1) m_pressure += c;
    }
  }

  //change to float
  f_pressure = m_pressure.toFloat();
  
  //----------------------------------------- Temperature, Humidity
  Wire.beginTransmission(0x1F);
  Wire.write(0x0E); //Temperature, Humidity
  Wire.endTransmission();
  Wire.requestFrom(0x1F, 15);

  commaIDX = 0;

  for(int i=0;i<15;i++){
    char c = Wire.read();
    if(c == 0x2c) commaIDX++;
    if(c!=0x2c && c!=0x00) {  // not comma, "c,%3.2f,%3.2f", readTempC(), readHumidity() : size 15
      if(commaIDX==1) m_tempC += c;
      else if(commaIDX==2) m_humiP += c;
    }
  }

  //change to int
  f_tempC = m_tempC.toFloat();
  f_humiP = m_humiP.toFloat();
  
  tickTime++;
  if(tickTime == 6) {
      toSend = true;
      tickTime = 0;
  }
}

