#include <Arduino.h>
#include <SinexcelSer1000.h>

SinexcelSer1000 sinexcelSer1000;

String commandLine;
bool isComplete = false;

const int numOfCharger = 3;
int packetSize = 0;

SemaphoreHandle_t xCANReceived = NULL;
SemaphoreHandle_t xSinexcelObject = NULL;

// int32_t idFilter = 0xE000000; // only get CAN message that start with (0b0000 1110 0000 0000 0000 0000 0000 0000)
int32_t idFilter = 0xB000000;
// int32_t idMask = 0xAE00000;
int32_t idMask = 0x53FFFFF;
int32_t softwareIdFilter = 0xE000000;
int32_t softwareIdMask = 0xE000000; // mask 
// int32_t idMask = 0xA3FE000;

unsigned long previousTime;
int schedulerTime = 100; //100ms

bool isFirstRun = true;
uint msgCount;
int address;

ChargerParam chargerParam[numOfCharger];
RawDataCharger rawDataChargerStorage[64];
Vector<RawDataCharger> rawDataChargerList(rawDataChargerStorage);

DataCharger dataCharger[8][63];

void onReceive(int _packetSize) 
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(xCANReceived, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  packetSize = _packetSize;
}

void test()
{
  uint8_t data[8] = {0};
  Serial.print("Received ");
  // Serial.print("packet with id 0x");
  int32_t packetId = sinexcelSer1000.packetId();
  // Serial.print(packetId, HEX);
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

    if((packetId & softwareIdMask) == softwareIdFilter)
    {
      // Serial.println("ID Found");
      int32_t voltage;
      int32_t current;
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


              break;
            case MessageIdResponse::Module_DC_Status:
              // Serial.println("0x0203");

              break;
            case MessageIdResponse::Module_AC_Status:
              // Serial.println("0x0204");

              break;
            case MessageIdResponse::Module_Output_Voltage:

              // Serial.println("0x0205");
              // outputVoltage = static_cast<float>(msgContent / 10);
              // voltage = (msgContent / 10);
              dataCharger[groupNumber][subAddress].msgCount = msgCount;
              dataCharger[groupNumber][subAddress].groupNumber = groupNumber;
              dataCharger[groupNumber][subAddress].subAddress = subAddress;
              dataCharger[groupNumber][subAddress].monitorGroup = monitorGroup;
              dataCharger[groupNumber][subAddress].monitorSubAddress = monitorSubAddress;
              dataCharger[groupNumber][subAddress].voltage = msgContent;
              // Serial.println("Monitor Group : " + String(monitorGroup));
              // Serial.println("Monitor Subaddress : " + String(monitorSubAddress));
              // Serial.println("Group : " + String(groupNumber));
              // Serial.println("Subaddress : " + String(subAddress));
              // Serial.println("Output Voltage : " + String(voltage) + " V");
              break;
            case MessageIdResponse::Module_Output_Current:

              // current = (msgContent / 100);
              dataCharger[groupNumber][subAddress].msgCount = msgCount;
              dataCharger[groupNumber][subAddress].groupNumber = groupNumber;
              dataCharger[groupNumber][subAddress].subAddress = subAddress;
              dataCharger[groupNumber][subAddress].monitorGroup = monitorGroup;
              dataCharger[groupNumber][subAddress].monitorSubAddress = monitorSubAddress;
              dataCharger[groupNumber][subAddress].current = msgContent;
              // Serial.println("Monitor Group : " + String(monitorGroup));
              // Serial.println("Monitor Subaddress : " + String(monitorSubAddress));
              // Serial.println("Group : " + String(groupNumber));
              // Serial.println("Subaddress : " + String(subAddress));
              // Serial.println("Output Current : " + String(current) + " A");
              break;
            case MessageIdResponse::Module_Version:

              // Serial.println("0x020A");
              break;
          }
          break;
      } 
    }
  }
  // Serial.println();    
}


void test2()
{ 
  float voltage,current;
  for (size_t i = 0; i < 8; i++)
  {
    for (size_t j = 0; j < 63; j++)
    {
      if (dataCharger[i][j].groupNumber < 0)
      {
        continue;
      }
      else
      {
        voltage = dataCharger[i][j].voltage / 10;
        current = dataCharger[i][j].current / 100;
        Serial.println("Message Count : " + String(dataCharger[i][j].msgCount));
        Serial.println("Monitor Group : " + String(dataCharger[i][j].monitorGroup));
        Serial.println("Monitor Subaddress : " + String(dataCharger[i][j].monitorSubAddress));
        Serial.println("Group : " + String(dataCharger[i][j].groupNumber));
        Serial.println("Subaddress : " + String(dataCharger[i][j].subAddress));
        Serial.println("Output Voltage : " + String(voltage) + " V");
        Serial.println("Output Current : " + String(current) + " A");
      }
      
    }
    
  }
  
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

  if(address > 1)
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
    else if (commandLine == "module32")
    {
      sinexcelSer1000.sendRequest(MessageIdRequest::Module_On_Off_32, 0xFFFFFFFF);
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
        test();
        xSemaphoreGive(xSinexcelObject);
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
    
  }
}

void setup() {
  // put your setup code here, to run once:
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
  previousTime = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(xSemaphoreTake(xSinexcelObject, 1000) == pdTRUE)
  {
    // Serial.println("Success get sinexcel obj");  
    test2();
    xSemaphoreGive(xSinexcelObject);
  }
  else
  {
    // Serial.println("failed to get sinexcel obj");
  }
  delay(100);
}