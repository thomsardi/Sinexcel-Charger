#include <JsonManagerCharger.h>

JsonManagerCharger::JsonManagerCharger()
{

}

/**
 * Build sinexcel data
 * 
 * @param[in]   dataCharger DataCharger struct
*/
String JsonManagerCharger::buildDataCharger(const DataCharger &dataCharger)
{
    String result;
    StaticJsonDocument<256> doc; // for single object
    
    doc["msg_count"] = dataCharger.msgCount;
    doc["group"] = dataCharger.groupNumber;
    doc["subaddress"] = dataCharger.subAddress;
    doc["voltage"] = dataCharger.outputVoltage;
    doc["current"] = dataCharger.outputCurrent;
    doc["module_off"] = dataCharger.moduleOff;
    doc["dc_operating_status"] = dataCharger.dcOperatingStatus;
    doc["ac_operating_status"] = dataCharger.acOperatingStatus;
    doc["dc_status_1"] = dataCharger.dcStatus_1;
    doc["dc_status_2"] = dataCharger.dcStatus_2;
    doc["ac_status_1"] = dataCharger.acStatus_1;
    doc["ac_status_2"] = dataCharger.acStatus_2;
    doc["ac_version"] = dataCharger.acVersionNumber;
    doc["dc_version"] = dataCharger.dcVersionNumber;
    serializeJson(doc, result);
    return result;
}

/**
 * Parse json post data charger and convert into ApiRequestCommand to send via CAN-bus
 * 
 * @param[in]   jsonInput   json formatted string
 * @param[in]   apiRequestCommand   ApiRequestCommand struct
*/
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

/**
 * Parse json post voltage data charger and convert into ApiRequestCommand to send via CAN-bus
 * 
 * @param[in]   jsonInput   json formatted string
 * @param[in]   apiRequestCommand   ApiRequestCommand struct
*/
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

/**
 * Parse json post current data charger and convert into ApiRequestCommand to send via CAN-bus
 * 
 * @param[in]   jsonInput   json formatted string
 * @param[in]   apiRequestCommand   ApiRequestCommand struct
*/
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

/**
 * Parse json post module on / off for first 1 - 32 address and convert into ApiRequestCommand to send via CAN-bus
 * 
 * @param[in]   jsonInput   json formatted string
 * @param[in]   apiRequestCommand   ApiRequestCommand struct
*/
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

/**
 * Parse json post module on / off for address 33 - 64 and convert into ApiRequestCommand to send via CAN-bus
 * 
 * @param[in]   jsonInput   json formatted string
 * @param[in]   apiRequestCommand   ApiRequestCommand struct
*/
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

/**
 * Parse json post module online for address 1 - 32 and convert into ApiRequestCommand to send via CAN-bus
 * @brief   to check the number of connected module
 * 
 * @param[in]   jsonInput   json formatted string
 * @param[in]   apiRequestCommand   ApiRequestCommand struct
*/
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

/**
 * Parse json post module online for address 33 - 64 and convert into ApiRequestCommand to send via CAN-bus
 * @brief   to check the number of connected module
 * 
 * @param[in]   jsonInput   json formatted string
 * @param[in]   apiRequestCommand   ApiRequestCommand struct
*/
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

/**
 * Get bit on certain position
 * 
 * @param[in]   pos     position of bit (bit 0 - 7)
 * @param[in]   data    data to evaluate
*/
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