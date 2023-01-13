#ifndef JSON_MANAGER_CHARGER_H
#define JSON_MANAGER_CHARGER_H

#define JSON_MANAGER_CHARGER_DEBUG

#ifdef JSON_MANAGER_CHARGER_DEBUG
    #define LOG_PRINT(x)    Serial.print(x)
    #define LOG_PRINTLN(x)  Serial.println(x)
#else
    #define LOG_PRINT(x)
    #define LOG_PRINTLN(x)
#endif

#include <ArduinoJson.h>
#include <SinexcelSer1000DataDef.h>

class JsonManagerCharger {
    public :
        JsonManagerCharger();
        //GET method (read the data)
        String buildDataCharger(const DataCharger &dataCharger);

        // POST method (write the data)
        int jsonParserVoltage(const char* jsonInput, ApiRequestCommand &apiRequestCommand); // parse json data from voltage url
        int jsonParserCurrent(const char* jsonInput, ApiRequestCommand &apiRequestCommand); // parse json data from current url
        int jsonParserDataCharger(const char* jsonInput, ApiRequestCommand &apiRequestCommand); // parse json data from data-charger url
        int jsonParserModuleOnOff_32(const char* jsonInput, ApiRequestCommand &apiRequestCommand); // parse json data from module-on-off-32 url
        int jsonParserModuleOnOff_64(const char* jsonInput, ApiRequestCommand &apiRequestCommand); // parse json data from module-on-off-64 url
        private :
        int getBit(int pos, int data);
};


#endif