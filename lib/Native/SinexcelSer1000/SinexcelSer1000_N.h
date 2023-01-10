#ifndef SINEXCEL_SER1000_H
#define SINEXCEL_SER1000_H

#ifndef Arduino_h
    #include <iostream>
#endif
#include <SinexcelSer1000DataDef.h>
#include <Vector.h>

class SinexcelSer1000 {
    public :
        SinexcelSer1000();
        int run();
        // int sendRequest(int cmd, int32_t value); //Broadcast command
        // int sendRequest(int cmd, int32_t value, int groupNumber); //Group Broadcast command
        int sendRequest(int cmd, int32_t value, uint8_t groupNumber = 31, uint8_t subAddress = 0); //Point to Point command
        int32_t getFrameId();
        int getData(int destination[], size_t arrSize);
    private :
        RequestCommand _requestCommand[16];
        Vector<RequestCommand> _commandList;
        int updateFrameId(int cmd);
        int buildFrameId();
        int updateData(int cmd, int32_t value);
        int buildData(int32_t value);
        int moduleRequestOnOff_32(int frameId, int buffer[], size_t bufferLength);
        int32_t _frameId = 0;
        uint8_t _protocolCode = 0;
        uint8_t _groupNumber = 0;
        uint8_t _subAddress = 0;
        uint8_t _monitorGroup = 0;
        uint8_t _monitorSubAddress = 0;
        int _msgType = 0;
        int _errType = 0;
        int _msgId[2] = {0};
        int _msgContent[4] = {0};
        int _data[8] = {0};
};

#endif