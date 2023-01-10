#include <Arduino.h>
#include <SinexcelSer1000.h>

SinexcelSer1000 sinexcelSer1000;

String commandLine;
bool isComplete = false;

const int numOfCharger = 3;

int32_t idFilter = 0xE000000; // only get CAN message that start with (0b0000 1110 0000 0000 0000 0000 0000 0000)
int32_t idMask = 0xE000000; // mask 

ChargerParam chargerParam[numOfCharger];

void onReceive(int packetSize) {
  // received a packet
  uint8_t data[8] = {0};
  Serial.print("Received ");

  if (sinexcelSer1000.packetExtended()) {
    Serial.print("extended ");
  }

  if (sinexcelSer1000.packetRtr()) {
    // Remote transmission request, packet contains no data
    Serial.print("RTR ");
  }

  Serial.print("packet with id 0x");
  int32_t packetId = sinexcelSer1000.packetId();
  Serial.print(packetId, HEX);

  if (sinexcelSer1000.packetRtr()) {
    Serial.print(" and requested length ");
    Serial.println(sinexcelSer1000.packetDlc());
  } else {
    Serial.print(" and length ");
    Serial.println(packetSize);
    
    size_t index = 0;
    while (sinexcelSer1000.available()) {
      data[index] = sinexcelSer1000.read();
      Serial.print(data[index], HEX);
      Serial.print(",");
      index++;
    }
    // only print packet data for non-RTR packets
    Serial.println();

    if((packetId & idMask) == idFilter)
    {
      Serial.println("ID Found");
      int messageId = (data[2] << 8) + data[3];
      switch (messageId)
      {
        case 0x0202:
          Serial.println("0x0202");
          break;
        case 0x0203:
          Serial.println("0x0203");
          break;
        case 0x0204:
          Serial.println("0x0204");
          break;
        case 0x0205:
          Serial.println("0x0205");
          break;
        case 0x0206:
          Serial.println("0x0206");
          break;
        case 0x020a:
          Serial.println("0x020A");
          break;
        default:
          break;
      }
    }
  }

  Serial.println();
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  if (!sinexcelSer1000.begin(125E3)) {
    Serial.println("Starting CAN failed!");
  }
  // sinexcelSer1000.filterExtended(idFilter, idMask);
  sinexcelSer1000.onReceive(onReceive);
}

void loop() {
  // put your main code here, to run repeatedly:
  int errorCode;

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

  if(isComplete)
  {
    if (commandLine == "voltage")
    {
      sinexcelSer1000.sendRequest(Module_Output_Voltage, 4500);
    }
    else if (commandLine == "current")
    {
      sinexcelSer1000.sendRequest(Module_Output_Current, 1024);
    }
    isComplete = false;
    commandLine = "";
  }
  else
  {
    sinexcelSer1000.sendRequest(Query_Single_Module_Info,0,0,1);
  }  
  Serial.print("Sending extended packet ... ");
  errorCode = sinexcelSer1000.run();
  Serial.println("done");
  switch (errorCode)
  {
    case 0: 
      Serial.print("Idle\r\n");
      break;
    case 1: 
      Serial.print("Buff sent!\r\n");
      break;
    case -1: 
      Serial.print("Get tx buff time out!\r\n");
      break;
    case -2: 
      Serial.print("Send msg timeout!\r\n");
      sinexcelSer1000.sleep();
      delay(50);
      sinexcelSer1000.wakeup();
      break;
    default: 
      Serial.print("Buff send failed!\r\n");
  }
  delay(100);
}