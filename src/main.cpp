#include <Arduino.h>
#include <SinexcelSer1000.h>
//Json Library
#include <ArduinoJson.h>
#include <AsyncJson.h>
//Wifi Library
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#include <JsonManagerCharger.h>

#define LAMINATE_ROOM 1

SinexcelSer1000 sinexcelSer1000;
JsonManagerCharger jsonManagerCharger;

const char *ssid = "RnD_Sundaya";
const char *password = "sundaya22";

#ifdef LAMINATE_ROOM
    IPAddress local_ip(192, 168, 8, 150);
    IPAddress gateway(192, 168, 8, 1);
#else
    // Set your Static IP address
    IPAddress local_ip(192, 168, 2, 150);
    IPAddress gateway(192, 168, 2, 1);
#endif


// Set your Static IP address
// Set your Gateway IP address

IPAddress subnet(255, 255, 255, 0);
// IPAddress primaryDNS(192, 168, 2, 1);        // optional
// IPAddress secondaryDNS(119, 18, 156, 10);       // optional
String hostName = "rms-battery1-rnd-room";
String serverName = "http://192.168.2.132/mydatabase/";

AsyncWebServer server(80);

String commandLine;
bool isComplete = false;

const int numOfCharger = 3;
const int sinexcelChargerReferenceCurrent = 20; // 20 A
const int sinexcelChargercurrentMultiplier = 1024;
int packetSize = 0;

SemaphoreHandle_t xCANReceived = NULL;
SemaphoreHandle_t xSinexcelObject = NULL;

// int32_t idFilter = 0xE000000; // only get CAN message that start with (0b0000 1110 0000 0000 0000 0000 0000 0000)
int32_t idFilter = 0xB000000;
// int32_t idMask = 0xAE00000;
int32_t idMask = 0x53FFFFF;
int32_t softwareIdFilter = 0xE000000;
int32_t softwareIdFilter2 = 0xB000000; 
int32_t softwareIdMask = 0xE000000; // mask 
int32_t softwareIdMask2 = 0xB000000;
// int32_t idMask = 0xA3FE000;

unsigned long previousTime;
int schedulerTime = 200; //100ms

bool isFirstRun = true;
uint msgCount;
int address = 1;

// int maxGroup = 16;
// int maxSubAddress = 32;
// int subModuleBytesSize = sizeof(DataCharger) * maxSubAddress;
// int dataChargerBytesSize = sizeof(DataCharger);
// DataCharger *dataCharger = (DataCharger*)malloc(dataChargerBytesSize*maxGroup*maxSubAddress);
// DataCharger *baseAddr = dataCharger;


DataCharger dataCharger[16][32];

// void declareDataCharger()
// {
//   DataCharger *pDc;
//   pDc = baseAddr + (0*subModuleBytesSize) + (1*dataChargerBytesSize); //group 0, subaddress 1
//   pDc->msgCount = 100;
//   pDc->outputVoltage = 5000;
//   pDc->outputCurrent = 2000;
//   pDc = baseAddr + (0*subModuleBytesSize) + (2*dataChargerBytesSize); //group 0, subaddress 1
//   pDc->msgCount = 200;
//   pDc->outputVoltage = 10000;
//   pDc->outputCurrent = 4000;
// }


int getBit(int pos, int data)
{
  if (pos > 32 & pos < 0)
  {
    return -1;
  }
  int temp = data >> pos;
  temp = temp & 0x01;
  return temp;
}

void onReceive(int _packetSize) 
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(xCANReceived, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  packetSize = _packetSize;
}

void canPacketProcess()
{
  uint8_t data[8] = {0};
  // Serial.print("Received ");
  // Serial.print("packet with id 0x");
  int32_t packetId = sinexcelSer1000.packetId();
  // Serial.println(packetId, HEX);
  if (sinexcelSer1000.packetRtr()) {
    // Serial.print(" and requested length ");
    // Serial.println(sinexcelSer1000.packetDlc());
  } 
  else 
  {
    // Serial.print(" and length ");
    // Serial.println(packetSize);
    msgCount++;
    size_t index = 0;
    while (sinexcelSer1000.available()) {
      data[index] = sinexcelSer1000.read();
      // Serial.print(data[index], HEX);
      // Serial.print(",");
      index++;
    }

    // only print packet data for non-RTR packets
    // Serial.println();

    if((packetId & softwareIdMask) == softwareIdFilter || (packetId & softwareIdMask2) == softwareIdFilter2)
    {
      // Serial.println("ID Found");
      // DataCharger *pDc;

      int32_t voltage;
      int32_t current;
      int value;
      int groupNumber = (packetId & 0b11111000000) >> 6;
      int subAddress = (packetId & 0b111111);
      int monitorGroup = (packetId & 0b1111100000000000000000) >> 17;
      int monitorSubAddress = (packetId & 0b11111100000000000) >> 11;
      int msgType = data[0];
      int msgId = (data[2] << 8) + data[3];
      // Serial.print("Message ID : 0x");
      // Serial.println(msgId, HEX);
      int32_t msgContent = (data[4] << 24) + (data[5] << 16) + (data[6] << 8) + (data[7]);
      // Serial.print("Message Content : 0x");
      // Serial.println(msgContent,HEX);
      switch (msgType)
      {
        case MessageType::Query_Module_Response:
          // Serial.println("Module Response");
          switch (msgId)
          {
            case MessageIdResponse::Module_Operating_Status:
              // Serial.println("0x0202");
              dataCharger[groupNumber][subAddress].dcOperatingStatus = data[4] << 8 + data[5];
              dataCharger[groupNumber][subAddress].acOperatingStatus = data[6] << 8 + data[7];

              break;
            case MessageIdResponse::Module_DC_Status:
              // Serial.println("0x0203");
              dataCharger[groupNumber][subAddress].dcStatus_2 = data[4] << 8 + data[5];
              dataCharger[groupNumber][subAddress].dcStatus_1 = data[6] << 8 + data[7];
              dataCharger[groupNumber][subAddress].moduleOff = getBit(9, msgContent);
              break;
            case MessageIdResponse::Module_AC_Status:
              // Serial.println("0x0204");
              dataCharger[groupNumber][subAddress].acStatus_2 = data[4] << 8 + data[5];
              dataCharger[groupNumber][subAddress].acStatus_1 = data[6] << 8 + data[7];
              break;
            case MessageIdResponse::Module_Output_Voltage:

              // Serial.println("0x0205");
              // outputVoltage = static_cast<float>(msgContent / 10);
              // voltage = (msgContent / 10);
              
              // pDc = baseAddr + (groupNumber*subModuleBytesSize) + (dataChargerBytesSize*subAddress);
              // pDc->msgCount = msgCount;
              // pDc->groupNumber = groupNumber;
              // pDc->subAddress = subAddress;
              // pDc->monitorGroup = monitorGroup;
              // pDc->monitorSubAddress = monitorSubAddress;
              // pDc->outputVoltage = msgContent;
              
              
              // Serial.println("Monitor Group : " + String(*pDc->monitorGroup));
              // Serial.println("Monitor Subaddress : " + String(*(pDc->monitorSubAddress)));
              // Serial.println("Group : " + String(*(pDc->groupNumber)));
              // Serial.println("Subaddress : " + String(*(pDc->subAddress)));
              // Serial.println("Output Voltage : " + String(*(pDc->outputVoltage)) + " V");

              dataCharger[groupNumber][subAddress].msgCount = msgCount;
              dataCharger[groupNumber][subAddress].groupNumber = groupNumber;
              dataCharger[groupNumber][subAddress].subAddress = subAddress;
              dataCharger[groupNumber][subAddress].monitorGroup = monitorGroup;
              dataCharger[groupNumber][subAddress].monitorSubAddress = monitorSubAddress;
              dataCharger[groupNumber][subAddress].outputVoltage = msgContent;
              
              
              // Serial.println("Monitor Group : " + String(monitorGroup));
              // Serial.println("Monitor Subaddress : " + String(monitorSubAddress));
              // Serial.println("Group : " + String(groupNumber));
              // Serial.println("Subaddress : " + String(subAddress));
              // Serial.println("Output Voltage : " + String(voltage) + " V");
              break;
            case MessageIdResponse::Module_Output_Current:

              // current = (msgContent / 100);
              
              // pDc = baseAddr + (groupNumber*subModuleBytesSize) + (dataChargerBytesSize*subAddress);
              // pDc->msgCount = msgCount;
              // pDc->groupNumber = groupNumber;
              // pDc->subAddress = subAddress;
              // pDc->monitorGroup = monitorGroup;
              // pDc->monitorSubAddress = monitorSubAddress;
              // pDc->outputVoltage = msgContent;
              
              // Serial.println("Monitor Group : " + String(pDc->monitorGroup));
              // Serial.println("Monitor Subaddress : " + String(pDc->monitorSubAddress));
              // Serial.println("Group : " + String(pDc->groupNumber));
              // Serial.println("Subaddress : " + String(pDc->subAddress));
              // Serial.println("Output Current : " + String(pDc->outputCurrent) + " A");
              
              
              dataCharger[groupNumber][subAddress].msgCount = msgCount;
              dataCharger[groupNumber][subAddress].groupNumber = groupNumber;
              dataCharger[groupNumber][subAddress].subAddress = subAddress;
              dataCharger[groupNumber][subAddress].monitorGroup = monitorGroup;
              dataCharger[groupNumber][subAddress].monitorSubAddress = monitorSubAddress;
              dataCharger[groupNumber][subAddress].outputCurrent = msgContent;
              
              
              // Serial.println("Monitor Group : " + String(monitorGroup));
              // Serial.println("Monitor Subaddress : " + String(monitorSubAddress));
              // Serial.println("Group : " + String(groupNumber));
              // Serial.println("Subaddress : " + String(subAddress));
              // Serial.println("Output Current : " + String(current) + " A");
              break;
            case MessageIdResponse::Module_Version:

              // Serial.println("0x020A");
              dataCharger[groupNumber][subAddress].acVersionNumber = data[4] << 8 + data[5];
              dataCharger[groupNumber][subAddress].dcVersionNumber = data[6] << 8 + data[7];
              break;
          }
          break;
        case MessageType::Query_Controller:
            // Serial.println("Query Module Online");
            switch (msgId)
            {
              case MessageIdRequest::Module_Online_Status_32 :
                // Serial.println("Module Online 32 Case");
                // for (size_t i = 0; i < 32; i++)
                // {
                //   if (getBit(i, msgContent))
                //   {
                //     Serial.println("Found Module in Group " + String(groupNumber) + " Subaddress " + String(i));
                //   }
                  
                // }
                
                break;
              case MessageIdRequest::Module_Online_Status_64 :
                // Serial.println("Module Online 64 Case");
                // for (size_t i = 0; i < 32; i++)
                // {
                //   if (getBit(i, msgContent))
                //   {
                //     Serial.println("Found Module in Group " + String(groupNumber) + " Subaddress " + String(i+31));
                //   }
                  
                // }
                break;  
            }
          break;
      } 
    }
  }
  // Serial.println();    
}


void mainRoutine()
{ 
  // float voltage,current;
  // for (size_t i = 0; i < 8; i++)
  // {
  //   for (size_t j = 0; j < 63; j++)
  //   {
  //     if (dataCharger[i][j].groupNumber < 0)
  //     {
  //       continue;
  //     }
  //     else
  //     {
  //       voltage = dataCharger[i][j].voltage / 10;
  //       current = dataCharger[i][j].current / 100;
  //       Serial.println("Message Count : " + String(dataCharger[i][j].msgCount));
  //       Serial.println("Monitor Group : " + String(dataCharger[i][j].monitorGroup));
  //       Serial.println("Monitor Subaddress : " + String(dataCharger[i][j].monitorSubAddress));
  //       Serial.println("Group : " + String(dataCharger[i][j].groupNumber));
  //       Serial.println("Subaddress : " + String(dataCharger[i][j].subAddress));
  //       Serial.println("Output Voltage : " + String(voltage) + " V");
  //       Serial.println("Output Current : " + String(current) + " A");
  //     }
      
  //   }
    
  // }
  
  // DataCharger *pDc;
  // pDc = baseAddr + 0*subModuleBytesSize + 1*dataChargerBytesSize;
  // Serial.println(pDc->msgCount);
  // Serial.println(pDc->outputVoltage);
  // Serial.println(pDc->outputCurrent);

  // pDc = baseAddr + 1*subModuleBytesSize + 1*dataChargerBytesSize;
  // Serial.println(pDc->msgCount);
  // Serial.println(pDc->outputVoltage);
  // Serial.println(pDc->outputCurrent);

  if (Serial.available())
  {
    char inByte = Serial.read();
    if(inByte != '\n')
    {      
      commandLine += inByte;
    }
    else
    {
      isComplete = true;
    }
  }

  if(address > 3)
  {
    address = 1;
  }
  
  if(isComplete)
  {
    if (commandLine == "voltage")
    {
      sinexcelSer1000.sendRequest(MessageIdRequest::Module_Output_Voltage, 4500);
    }
    else if (commandLine == "current")
    {
      sinexcelSer1000.sendRequest(MessageIdRequest::Module_Output_Current, 1024);
    }
    else if (commandLine == "module32on")
    {
      sinexcelSer1000.sendRequest(MessageIdRequest::Module_On_Off_32, 0xFFFFFFFF);
    }
    else if (commandLine == "module32off")
    {
      sinexcelSer1000.sendRequest(MessageIdRequest::Module_On_Off_32, 0);
    }
    isComplete = false;
    commandLine = "";
  }
  else
  {
    if(((millis() - previousTime) > schedulerTime) || isFirstRun)
    {
      if(sinexcelSer1000.isSendQueueEmpty())
      {
        sinexcelSer1000.sendRequest(MessageIdRequest::Query_Single_Module_Info,0,0,address);
        address++;
        previousTime = millis();
      }
      isFirstRun = false;
    }  
  }  
  // Serial.print("Sending extended packet ... ");
  int errorCode = sinexcelSer1000.run();
  // Serial.println("done");
  switch (errorCode)
  {
    case 0: 
      // Serial.print("Idle\r\n");
      break;
    case 1: 
      // Serial.print("Buff sent!\r\n");
      break;
    case -1: 
      // Serial.print("Get tx buff time out!\r\n");
      break;
    case -2: 
      // Serial.print("Send msg timeout!\r\n");
      sinexcelSer1000.sleep();
      delay(50);
      sinexcelSer1000.wakeup();
      break;
    default: 
      Serial.print("Buff send failed!\r\n");
  }
}

void readCANPacket(void *parameter)
{
  while(1)
  {
    if (xSemaphoreTake(xCANReceived, 1000) == pdTRUE)
    {
      if (xSemaphoreTake(xSinexcelObject, 1000) == pdTRUE)
      {
        // Serial.println("CAN Read Success");
        canPacketProcess();
        xSemaphoreGive(xSinexcelObject);
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
    
  }
}

void setup() {
  // put your setup code here, to run once:
  // declareDataCharger();
  xCANReceived = xSemaphoreCreateBinary();
  xSinexcelObject = xSemaphoreCreateBinary();

  if( xSinexcelObject == NULL )
  {
      /* There was insufficient FreeRTOS heap available for the semaphore to
      be created successfully. */
  }
  else
  {
      /* The semaphore can now be used. Its handle is stored in the
      xSemahore variable.  Calling xSemaphoreTake() on the semaphore here
      will fail until the semaphore has first been given. */
      xSemaphoreGive(xSinexcelObject);
  }

  xTaskCreatePinnedToCore(
    readCANPacket,
    "readCANPacket",
    2048,
    NULL,
    tskIDLE_PRIORITY,
    NULL,
    1
  );
  

  Serial.begin(115200);
  if (!sinexcelSer1000.begin(125E3)) {
    Serial.println("Starting CAN failed!");
  }
  sinexcelSer1000.filterExtended(idFilter, idMask);
  sinexcelSer1000.onReceive(onReceive);

  WiFi.disconnect(true);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.mode(WIFI_MODE_NULL);
  delay(100);
  WiFi.setHostname(hostName.c_str());
  WiFi.mode(WIFI_STA);
  if (!WiFi.config(local_ip, gateway, subnet))
  {
      Serial.println("STA Failed to configure");
  }
  WiFi.begin(ssid, password);

  Serial.println("Connecting..");
  int timeout = 0;
  while(timeout < 10)
  {
      if (WiFi.status() != WL_CONNECTED)
      {
          // Serial2.print(".");
          Serial.print(".");
          delay(100);
          timeout++;
      }
      else
      {
          Serial.println();
          break;
      }
  }
  delay(100); //wait a bit to stabilize voltage and current
  if (!MDNS.begin(hostName.c_str())) {             // Start the mDNS responder for esp8266.local
      Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");
  delay(100);
  // Wire.begin(I2C_SDA, I2C_SCL);
  // lcd.init();
  // lcd.backlight();    
  // lcd.clear();
  if (timeout < 10)
  {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Subnet Mask: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("Gateway IP: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("DNS 1: ");
    Serial.println(WiFi.dnsIP(0));
    Serial.print("DNS 2: ");
    Serial.println(WiFi.dnsIP(1));
    Serial.print("Hostname: ");
    Serial.println(WiFi.getHostname());
  }
  else
  {
    Serial.println("WiFi Not Connected");
  }

  AsyncCallbackJsonWebHandler *setDataChargerHandler = new AsyncCallbackJsonWebHandler("/get-data-charger", [](AsyncWebServerRequest *request, JsonVariant &json)
  {
    String response = R"(
    {
    "status" : :status:
    }
    )";
    String input = json.as<String>();
    ApiRequestCommand api;
    DataCharger *pDc;
    int status = jsonManagerCharger.jsonParserDataCharger(input.c_str(), api);
    if (status)
    {
      // pDc = baseAddr + (api.groupNumber*maxSubAddress) + (api.subAddress*subModuleBytesSize);
      // response = jsonManagerCharger.buildDataCharger(*pDc);
      response = jsonManagerCharger.buildDataCharger(dataCharger[api.groupNumber][api.subAddress]);
    }
    else
    {
      DataCharger dc;
      response = jsonManagerCharger.buildDataCharger(dc);
      // int fail = -1;
      // response.replace(":status:", String(fail));
    }
    request->send(200, "application/json", response); });

  AsyncCallbackJsonWebHandler *setVoltageHandler = new AsyncCallbackJsonWebHandler("/set-voltage", [](AsyncWebServerRequest *request, JsonVariant &json)
  {
    String response = R"(
    {
    "status" : :status:
    }
    )";
    String input = json.as<String>();
    ApiRequestCommand api;
    int status = jsonManagerCharger.jsonParserVoltage(input.c_str(), api);
    if (status)
    {
      int32_t dcVoltage = api.value / 100;
      // Serial.println(dcVoltage);
      sinexcelSer1000.sendRequest(MessageIdRequest::Module_Output_Voltage, dcVoltage, api.groupNumber, api.subAddress);
    }
    response.replace(":status:", String(status));
    request->send(200, "application/json", response); });

  AsyncCallbackJsonWebHandler *setCurrentHandler = new AsyncCallbackJsonWebHandler("/set-current", [](AsyncWebServerRequest *request, JsonVariant &json)
  {
    String response = R"(
    {
    "status" : :status:
    }
    )";
    String input = json.as<String>();
    ApiRequestCommand api;
    int status = jsonManagerCharger.jsonParserCurrent(input.c_str(), api);
    if (status)
    {
      int32_t dcCurrent = ((api.value * sinexcelChargercurrentMultiplier) / (1000*sinexcelChargerReferenceCurrent));
      // Serial.println(dcCurrent);
      sinexcelSer1000.sendRequest(MessageIdRequest::Module_Output_Current, dcCurrent, api.groupNumber, api.subAddress);
    }
    response.replace(":status:", String(status));
    request->send(200, "application/json", response); });

  AsyncCallbackJsonWebHandler *setModule32Handler = new AsyncCallbackJsonWebHandler("/set-module-32", [](AsyncWebServerRequest *request, JsonVariant &json)
  {
    String response = R"(
    {
    "status" : :status:
    }
    )";
    String input = json.as<String>();
    ApiRequestCommand api;
    int status = jsonManagerCharger.jsonParserModuleOnOff_32(input.c_str(), api);
    if (status)
    {
      sinexcelSer1000.sendRequest(MessageIdRequest::Module_On_Off_32, api.value, api.groupNumber, api.subAddress);
    }
    response.replace(":status:", String(status));
    request->send(200, "application/json", response); });
  
  AsyncCallbackJsonWebHandler *setModule64Handler = new AsyncCallbackJsonWebHandler("/set-module-64", [](AsyncWebServerRequest *request, JsonVariant &json)
  {
    String response = R"(
    {
    "status" : :status:
    }
    )";
    String input = json.as<String>();
    ApiRequestCommand api;
    int status = jsonManagerCharger.jsonParserModuleOnOff_64(input.c_str(), api);
    if (status)
    {
      sinexcelSer1000.sendRequest(MessageIdRequest::Module_On_Off_64, api.value, api.groupNumber, api.subAddress);
    }
    response.replace(":status:", String(status));
    request->send(200, "application/json", response); });

  AsyncCallbackJsonWebHandler *setModuleOnlineStatus32Handler = new AsyncCallbackJsonWebHandler("/scan-module-32", [](AsyncWebServerRequest *request, JsonVariant &json)
  {
    String response = R"(
    {
    "status" : :status:
    }
    )";
    String input = json.as<String>();
    ApiRequestCommand api;
    int status = jsonManagerCharger.jsonParserModuleStatusOnline_32(input.c_str(), api);
    if (status)
    {
      sinexcelSer1000.sendRequest(MessageIdRequest::Module_Online_Status_32, api.value, api.groupNumber, api.subAddress);
    }
    response.replace(":status:", String(status));
    request->send(200, "application/json", response); });

  AsyncCallbackJsonWebHandler *setModuleOnlineStatus64Handler = new AsyncCallbackJsonWebHandler("/scan-module-64", [](AsyncWebServerRequest *request, JsonVariant &json)
  {
    String response = R"(
    {
    "status" : :status:
    }
    )";
    String input = json.as<String>();
    ApiRequestCommand api;
    int status = jsonManagerCharger.jsonParserModuleStatusOnline_64(input.c_str(), api);
    if (status)
    {
      sinexcelSer1000.sendRequest(MessageIdRequest::Module_Online_Status_64, api.value, api.groupNumber, api.subAddress);
    }
    response.replace(":status:", String(status));
    request->send(200, "application/json", response); });

  server.addHandler(setDataChargerHandler);
  server.addHandler(setVoltageHandler);
  server.addHandler(setCurrentHandler);
  server.addHandler(setModule32Handler);
  server.addHandler(setModule64Handler);
  server.addHandler(setModuleOnlineStatus32Handler);
  server.addHandler(setModuleOnlineStatus64Handler);
  server.begin();
  Serial.println("HTTP server started");
  previousTime = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(xSemaphoreTake(xSinexcelObject, 1000) == pdTRUE)
  {
    // Serial.println("Success get sinexcel obj");  
    mainRoutine();
    xSemaphoreGive(xSinexcelObject);
  }
  else
  {
    // Serial.println("failed to get sinexcel obj");
  }
  delay(50);
}