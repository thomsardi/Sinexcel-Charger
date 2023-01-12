#ifndef SINEXCEL_SER1000_H
#define SINEXCEL_SER1000_H

#ifndef Arduino_h
    #include <iostream>
#endif
#include <SinexcelSer1000DataDef.h>
#include <CAN.h>
#include <Vector.h>

#define REG_BASE                   0x3ff6b000

#define REG_MOD                    0x00
#define REG_CMR                    0x01
#define REG_SR                     0x02
#define REG_IR                     0x03
#define REG_IER                    0x04

#define REG_BTR0                   0x06
#define REG_BTR1                   0x07
#define REG_OCR                    0x08

#define REG_ALC                    0x0b
#define REG_ECC                    0x0c
#define REG_EWLR                   0x0d
#define REG_RXERR                  0x0e
#define REG_TXERR                  0x0f
#define REG_SFF                    0x10
#define REG_EFF                    0x10
#define REG_ACRn(n)                (0x10 + n)
#define REG_AMRn(n)                (0x14 + n)

#define REG_CDR                    0x1F

class SinexcelSer1000 : public ESP32SJA1000Class{
    public :
        SinexcelSer1000();
        int run();
        // int sendRequest(int cmd, int32_t value); //Broadcast command
        // int sendRequest(int cmd, int32_t value, int groupNumber); //Group Broadcast command
        int sendRequest(int msgId, int32_t value, uint8_t groupNumber = 31, uint8_t subAddress = 0); //Point to Point command
        int32_t getFrameId();
        int getData(int destination[], size_t arrSize);

        using CANControllerClass::filterExtended;
        virtual int filterExtended(long id, long mask);

    private :
        ESP32SJA1000Class _esp32sja1000class;
        RequestCommand _requestCommand[16];
        Vector<RequestCommand> _commandList;
        int updateFrameId(int msgId);
        int buildFrameId();
        int updateData(int msgId, int32_t value);
        int buildData(int32_t value);
        int moduleRequestOnOff_32(int frameId, int buffer[], size_t bufferLength);
        void modifyRegister(uint8_t address, uint8_t mask, uint8_t value);
        void writeRegister(uint8_t address, uint8_t value);
        int32_t _frameId = 0;
        uint8_t _protocolCode = 0;
        uint8_t _groupNumber = 0;
        uint8_t _subAddress = 0;
        uint8_t _monitorGroup = 0;
        uint8_t _monitorSubAddress = 0;
        uint8_t _msgType = 0;
        uint8_t _errType = 0;
        uint8_t _msgId[2] = {0};
        uint8_t _msgContent[4] = {0};
        uint8_t _data[8] = {0};
};

#endif