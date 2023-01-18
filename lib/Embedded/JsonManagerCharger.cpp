#include <JsonManagerCharger.h>

JsonManagerCharger::JsonManagerCharger()
{

}

String JsonManagerCharger::buildDataCharger(const DataCharger &dataCharger)
{
    String result;
    StaticJsonDocument<128> doc; // for 8 object
    
    doc["msgcount"] = dataCharger.msgCount;
    doc["group"] = dataCharger.groupNumber;
    doc["subaddress"] = dataCharger.subAddress;
    doc["voltage"] = dataCharger.voltage;
    doc["current"] = dataCharger.current;
    serializeJson(doc, result);
    return result;
}

int JsonManagerCharger::jsonParserDataCharger(const char* jsonInput, ApiRequestCommand &apiRequestCommand)
{
    ApiRequestCommand api;
    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, jsonInput);
    if (error) 
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return -1;
    }

    if(!doc.containsKey("group") || !doc.containsKey("subaddress"))
    {
        return -1;
    }
    
    int groupNumber = doc["group"];
    int subAddress = doc["subaddress"];

    if (groupNumber < 0 || groupNumber > 30 || subAddress < 1 || subAddress > 63)
    {
        return -1;
    }
    
    apiRequestCommand.groupNumber = groupNumber;
    apiRequestCommand.subAddress = subAddress;
    apiRequestCommand.value = 0;
    return 1;
}

int JsonManagerCharger::jsonParserVoltage(const char* jsonInput, ApiRequestCommand &apiRequestCommand)
{
    ApiRequestCommand api;
    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, jsonInput);
    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return -1;
    }

    if(!doc.containsKey("group") || !doc.containsKey("subaddress") || !doc.containsKey("voltage"))
    {
        return -1;
    }
    
    int groupNumber = doc["group"];
    int subAddress = doc["subaddress"];
    int32_t value = doc["voltage"];

    if (groupNumber < 0 || groupNumber > 31 || subAddress < 0 || subAddress > 63)
    {
        return -1;
    }
    
    apiRequestCommand.groupNumber = groupNumber;
    apiRequestCommand.subAddress = subAddress;
    apiRequestCommand.value = value;
    return 1;
}

int JsonManagerCharger::jsonParserCurrent(const char* jsonInput, ApiRequestCommand &apiRequestCommand)
{
    ApiRequestCommand api;
    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, jsonInput);
    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return -1;
    }

    if(!doc.containsKey("group") || !doc.containsKey("subaddress") || !doc.containsKey("current"))
    {
        return -1;
    }
    
    int groupNumber = doc["group"];
    int subAddress = doc["subaddress"];
    int32_t value = doc["current"];

    if (groupNumber < 0 || groupNumber > 31 || subAddress < 0 || subAddress > 63)
    {
        return -1;
    }
    
    apiRequestCommand.groupNumber = groupNumber;
    apiRequestCommand.subAddress = subAddress;
    apiRequestCommand.value = value;
    return 1;
}

int JsonManagerCharger::jsonParserModuleOnOff_32(const char* jsonInput, ApiRequestCommand &apiRequestCommand)
{
    ApiRequestCommand api;
    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, jsonInput);
    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return -1;
    }

    if(!doc.containsKey("group") || !doc.containsKey("value"))
    {
        return -1;
    }
    
    int groupNumber = doc["group"];
    int32_t value = doc["value"];

    if (groupNumber < 0 || groupNumber > 30)
    {
        return -1;
    }
    
    apiRequestCommand.groupNumber = groupNumber;
    apiRequestCommand.subAddress = 0;
    apiRequestCommand.value = value;
    return 1;
}

int JsonManagerCharger::jsonParserModuleOnOff_64(const char* jsonInput, ApiRequestCommand &apiRequestCommand)
{
    ApiRequestCommand api;
    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, jsonInput);
    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return -1;
    }

    if(!doc.containsKey("group") || !doc.containsKey("value"))
    {
        return -1;
    }
    
    int groupNumber = doc["group"];
    int32_t value = doc["value"];

    if (groupNumber < 0 || groupNumber > 30)
    {
        return -1;
    }
    
    apiRequestCommand.groupNumber = groupNumber;
    apiRequestCommand.subAddress = 0;
    apiRequestCommand.value = value;
    return 1;
}


int JsonManagerCharger::jsonParserModuleStatusOnline_32(const char* jsonInput, ApiRequestCommand &apiRequestCommand)
{
    ApiRequestCommand api;
    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, jsonInput);
    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return -1;
    }

    if(!doc.containsKey("group"))
    {
        return -1;
    }
    
    int groupNumber = doc["group"];

    if (groupNumber < 0 || groupNumber > 30)
    {
        return -1;
    }
    
    apiRequestCommand.groupNumber = groupNumber;
    apiRequestCommand.subAddress = 0;
    apiRequestCommand.value = 0;
    return 1;
}
int JsonManagerCharger::jsonParserModuleStatusOnline_64(const char* jsonInput, ApiRequestCommand &apiRequestCommand)
{
    ApiRequestCommand api;
    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, jsonInput);
    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return -1;
    }

    if(!doc.containsKey("group"))
    {
        return -1;
    }
    
    int groupNumber = doc["group"];

    if (groupNumber < 0 || groupNumber > 30)
    {
        return -1;
    }
    
    apiRequestCommand.groupNumber = groupNumber;
    apiRequestCommand.subAddress = 0;
    apiRequestCommand.value = 0;
    return 1;
}

int JsonManagerCharger::getBit(int pos, int data)
{
  if (pos > 7 & pos < 0)
  {
    return -1;
  }
  int temp = data >> pos;
  temp = temp & 0x01;
  return temp;
}