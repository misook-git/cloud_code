// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "AzureIoTHub.h"
#include "iot_configs.h"
#include "sample.h"

/*String containing Hostname, Device Id & Device Key in the format:             */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"    */
static const char* connectionString = IOT_CONFIG_CONNECTION_STRING;
IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

BEGIN_NAMESPACE(RecursiveSoft);
DECLARE_MODEL(AtmosphericModel,
  WITH_DATA(ascii_char_ptr, t_DeviceID),
  WITH_DATA(int32_t, t_CO2),
  WITH_DATA(int32_t, t_tVOC),
  WITH_DATA(int32_t, t_pm1_0),
  WITH_DATA(int32_t, t_pm2_5),
  WITH_DATA(int32_t, t_pm10),
  WITH_DATA(int32_t, t_pm1_0MIN),
  WITH_DATA(int32_t, t_pm2_5MIN),
  WITH_DATA(int32_t, t_pm10MIN),
  WITH_DATA(int32_t, t_pm1_0MAX),
  WITH_DATA(int32_t, t_pm2_5MAX),
  WITH_DATA(int32_t, t_pm10MAX),
  WITH_DATA(float, t_tempC),
  WITH_DATA(float, t_humiP),
  WITH_DATA(float, t_pressure),
  WITH_DATA(float, t_latitudeA),
  WITH_DATA(float, t_longitudeA)
  //WITH_ACTION 추가.
);
END_NAMESPACE(RecursiveSoft);

AtmosphericModel* atmos;

// Direct Method 추가


// 송신 메시지에 대한 콜백.
void sendCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback) {
    unsigned int messageTrackingId = (unsigned int)(uintptr_t)userContextCallback;
    (void)printf("Message Id: %u - Received.\r\n", messageTrackingId);
    (void)printf("Result Call Back Called! Result is: %s \r\n", ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
}

static char propText[1024];

static void sendMessage(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const unsigned char* buffer, size_t size, AtmosphericModel *atmos) {
    
    static unsigned int messageTrackingId;
    
    // 데이터를 담을 크기의 "메시지 핸들러" 생성.
    IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(buffer, size);
    
    if (messageHandle == NULL) printf("unable to create a new IoTHubMessage\r\n");
    else {  
        // 메시지 핸들러로 propMap 형태로 만든다.
        MAP_HANDLE propMap = IoTHubMessage_Properties(messageHandle);

        // 이상 상황에 대한 메시지를 Map에 추가.
        
        
        // 콜백 설정 후 전송.
        if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, sendCallback, (void*)(uintptr_t)messageTrackingId) != IOTHUB_CLIENT_OK) {
            printf("failed to hand over the message to IoTHubClient");
        } else printf("IoTHubClient accepted the message for delivery\r\n");
        
        // 메시지 삭제.
        IoTHubMessage_Destroy(messageHandle);
    }
    messageTrackingId++;
}

// IoT Hub 로 부터의 메시지.
/*this function "links" IoTHub to the serialization library*/
static IOTHUBMESSAGE_DISPOSITION_RESULT ReceiveMessageCallback(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback) {
    size_t size = 0;
    const unsigned char* buffer;
    int* counter = (int*)userContextCallback;
    MAP_HANDLE mapProperties;
    
    const char* messageId;
    const char* correlationId;
    
    // Message Properties
    if ((messageId = IoTHubMessage_GetMessageId(message)) == NULL) messageId = "<null>";
    if ((correlationId = IoTHubMessage_GetCorrelationId(message)) == NULL) correlationId = "<null>";
    
    // Message content 체크 - 수신된 메시지를 버퍼(바이트 배열)에 담는다.
    if (IoTHubMessage_GetByteArray(message, &buffer, &size) != IOTHUB_MESSAGE_OK) {
        printf("unable to IoTHubMessage_GetByteArray\r\n");
        return IOTHUBMESSAGE_ABANDONED;
    } else {
      (void)printf("Received Message [%d]\r\n From Hub, Message ID: %s\r\n", *counter, messageId);
      (void)printf("Correlation ID: %s\r\n Data: <<<%.*s>>> & Size=%d\r\n", correlationId, (int)size, buffer, (int)size);
      
      mapProperties = IoTHubMessage_Properties(message);
      if(mapProperties != NULL) {
        const char*const* keys;
        const char*const* values;
        size_t propertyCount = 0;
        // 맵 내의 키와 밸류를 배열로 추출.- Map_GetInternals
        if (Map_GetInternals(mapProperties, &keys, &values, &propertyCount) == MAP_OK) {
          if (propertyCount > 0) {
            size_t i;
            printf("Message Properties:\r\n");
            for (i = 0; i < propertyCount; i++) {
              (void)printf("\tKey: %s Value: %s\r\n", keys[i], values[i]);
            }
            (void)printf("\r\n");
          }
        }
      }
      // 여기에서 Key 와 Value 에 기반한 메시지 처리 -----
    }
    return IOTHUBMESSAGE_ACCEPTED;
}

static int DeviceMethodCallback(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* response_size, void* userContextCallback) {

    (void)userContextCallback;
    
    static int result = -1;
    static int powerValue;
    const unsigned char* buffer;

    //Direct Method 에 대한 실행 및 콜백.
    
    return result;
}

// IoT Hub 클라이언트 생성 및 연결.
void initIoTHubClient(void){

  // 플랫폼 초기화.
  if (platform_init() != 0) (void)printf("Failed to initialize platform.\r\n");
  // 직렬화 API 준비 : serializer 라이브러리를 사용하려면 IoT Hub로 보낼 데이터와 IoT Hub에서 받으려는 메시지를 지정하는 모델을 정의. 
  if (serializer_init(NULL) != SERIALIZER_OK) (void)printf("Failed on serializer_init\r\n");
  // 클라이언트 초기화
  iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol);
  if (iotHubClientHandle == NULL) (void)printf("Failed on IoTHubClient_LL_Create\r\n");
  
#ifdef SET_TRUSTED_CERT_IN_SAMPLES
  // 인증서 정보 추가.
  if (IoTHubClient_LL_SetOption(iotHubClientHandle, "TrustedCerts", certificates) != IOTHUB_CLIENT_OK) (void)printf("failure to set option \"TrustedCerts\"\r\n");
#endif // SET_TRUSTED_CERT_IN_SAMPLES
  
  // 송/수신용 데이터 모델 객체 생성.
  atmos = CREATE_MODEL_INSTANCE(RecursiveSoft, AtmosphericModel);
  if (atmos == NULL) (void)printf("Failed on CREATE_MODEL_INSTANCE\r\n");

  // 수신 메시지의 비동기 콜백 메서드 셋팅: ReceiveMessageCallback - 메시지ID, 메시지 내용, 메시지의 사용자 지정 속성.
  IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, ReceiveMessageCallback, atmos);

  // Direct Method Callback : 3rd param -> void* userContextCallback
  IoTHubClient_LL_SetDeviceMethodCallback(iotHubClientHandle, DeviceMethodCallback, 0);
}

void simplesample_mqtt_run(void) {
  // Ready Sensor Data
  atmos->t_DeviceID = DEVICE_ID;
  atmos->t_CO2 = i_CO2;
  atmos->t_tVOC = i_tVOC;
  atmos->t_pm1_0 = i_pm1_0;
  atmos->t_pm2_5 = i_pm2_5;
  atmos->t_pm10 = i_pm10;
  atmos->t_pm1_0MIN = i_pm1_0MIN;
  atmos->t_pm2_5MIN = i_pm2_5MIN;
  atmos->t_pm10MIN = i_pm10MIN;
  atmos->t_pm1_0MAX = i_pm1_0MAX;
  atmos->t_pm2_5MAX = i_pm2_5MAX;
  atmos->t_pm10MAX = i_pm10MAX;
  atmos->t_tempC = f_tempC;
  atmos->t_humiP = f_humiP;
  atmos->t_pressure = f_pressure;
  atmos->t_latitudeA = f_latitudeA;
  atmos->t_longitudeA = f_longitudeA;
  {
    unsigned char* destination;
    size_t destinationSize;
    if (SERIALIZE(&destination, &destinationSize,
      atmos->t_DeviceID,   atmos->t_CO2,      atmos->t_tVOC,    atmos->t_pm1_0,      atmos->t_pm2_5,    atmos->t_pm10,
      atmos->t_pm1_0MIN,   atmos->t_pm2_5MIN, atmos->t_pm10MIN, atmos->t_pm1_0MAX,   atmos->t_pm2_5MAX, atmos->t_pm10MAX,
      atmos->t_tempC,      atmos->t_humiP,    atmos->t_pressure,atmos->t_latitudeA,  atmos->t_longitudeA ) != CODEFIRST_OK)
      {
        (void)printf("Failed to serialize\r\n");
      } else {
        sendMessage(iotHubClientHandle, destination, destinationSize, atmos);
        free(destination);
      }
   }
}

// 아래 메시지가 수신되자 마자 처리할 수도 있지만, 일정 시간 간격을 두고 최종 수신된 메시지에 기반한 처리를 할 수도 있다.
void doWork(void) {
//  IOTHUB_CLIENT_STATUS status;
//  while ((IoTHubClient_LL_GetSendStatus(iotHubClientHandle, &status) == IOTHUB_CLIENT_OK) && (status == IOTHUB_CLIENT_SEND_STATUS_BUSY))
//  {
    IoTHubClient_LL_DoWork(iotHubClientHandle);
    ThreadAPI_Sleep(100);
//  }
}

void initClient(void) {
  initIoTHubClient();
}

void sample_run(void) {
  simplesample_mqtt_run();
}


/*
  IoTHubClient_CreateFromConnectionString <-> IoTHubClient_LL_Destroy(iotHubClientHandle);
  serializer_init(NULL); <-> serializer_deinit();
  platform_init()
 */
