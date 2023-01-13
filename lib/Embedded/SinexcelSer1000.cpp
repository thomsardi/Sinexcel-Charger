
#include <SinexcelSer1000.h>

SinexcelSer1000::SinexcelSer1000()
{
    _commandList.setStorage(_requestCommand);
    _protocolCode = 0x38; //SER1000 protocol code for upper controller
    _monitorGroup = 0x1C;
    _monitorSubAddress = 0;
    _msgType = 0x03;
    _errType = 0xF0;
}

/*
int SinexcelSer1000::sendRequest(int cmd, int32_t value)
{
    int status = -1;
    int isFrameUpdated, isDataUpdated;
    _groupNumber = 0x1F;
    _subAddress = 0;
    isFrameUpdated = updateFrameId(cmd);
    isDataUpdated = updateData(cmd, value);
    if(isFrameUpdated && isDataUpdated)
    {
        status = 1;
    }
    return status;
}

int SinexcelSer1000::sendRequest(int cmd, int32_t value, int groupNumber)
{
    int status = -1;
    int isFrameUpdated, isDataUpdated;
    _groupNumber = groupNumber;
    _subAddress = 0;
    isFrameUpdated = updateFrameId(cmd);
    isDataUpdated = updateData(cmd, value);
    if(isFrameUpdated && isDataUpdated)
    {
        status = 1;
    }
    return status;
}
*/

int SinexcelSer1000::run()
{
    int isFrameUpdated, isDataUpdated;
    int status = 0;
    // Serial.println(_commandList.size());
    if (_commandList.size() > 0)
    {
        RequestCommand rc = _commandList.at(0);
        this->_groupNumber = rc.groupNumber;
        this->_subAddress = rc.subAddress;
        isFrameUpdated = updateFrameId(rc.msgId);
        isDataUpdated = updateData(rc.msgId, rc.value);
        if(isFrameUpdated && isDataUpdated)
        {
            beginExtendedPacket(_frameId);
            write(_data, 8);
            status = endPacket();
        }
        _commandList.remove(0);
    }
    return status;
}

int SinexcelSer1000::sendRequest(int msgId, int32_t value, uint8_t groupNumber, uint8_t subAddress)
{
    RequestCommand rc;
    int status = -1;
    int _groupNumber, _subAddress;
    if(groupNumber > 30) //Broadcast Command
    {
        _groupNumber = 31; // 0x1f is used as global broadcast
        _subAddress = 0;
    }
    else
    {
        if(_subAddress > 63) //only 0 - 63 subaddresses are valid
        {
            return status;
        }
        _groupNumber = groupNumber;
        _subAddress = subAddress;
    }
    rc.msgId = msgId;
    rc.groupNumber = _groupNumber;
    rc.subAddress = _subAddress;
    rc.value = value;
    if (_commandList.size() < 16)
    {
        _commandList.push_back(rc);
        status = 1;
    }
    return status;
}

int SinexcelSer1000::isSendQueueEmpty()
{
    int queueSize = 0;
    if(_commandList.size() <= 0)
    {
        queueSize = 1;
    }
    return queueSize;
}

int SinexcelSer1000::getSendQueueSize()
{
    return _commandList.size();
}

int SinexcelSer1000::updateFrameId(int msgId)
{
    switch (msgId)
    {
        case MessageIdRequest::Module_On_Off_32:
            _protocolCode = 0x38;
            _subAddress = 0;
            _monitorGroup = 0x1c;
            _monitorSubAddress = 0;
            buildFrameId();
            return 1;
            break;
        case MessageIdRequest::Module_On_Off_64:
            _protocolCode = 0x38;
            _subAddress = 0;
            _monitorGroup = 0x1c;
            _monitorSubAddress = 0;
            buildFrameId();
            return 1;
            break;
        case MessageIdRequest::Module_Online_Status_32:
            _protocolCode = 0x2C;
            _subAddress = _groupNumber;
            _monitorGroup = _groupNumber;
            _groupNumber = 0x1C;
            _monitorSubAddress = _subAddress;
            buildFrameId();
            return 1;
            break;
        case MessageIdRequest::Module_Online_Status_64:
            _protocolCode = 0x2C;
            _subAddress = _groupNumber;
            _monitorGroup = _groupNumber;
            _groupNumber = 0x1C;
            _monitorSubAddress = _subAddress;
            buildFrameId();
            return 1;
            break;    
        case MessageIdRequest::Module_Voltage_Mode:
            _protocolCode = 0x38;
            _monitorGroup = 0x1c;
            _monitorSubAddress = 0;
            buildFrameId();
            return 1;
            break;
        case MessageIdRequest::Module_Output_Voltage:
            _protocolCode = 0x38;
            _monitorGroup = 0x1c;
            _monitorSubAddress = 0;
            buildFrameId();
            return 1;
            break;
        case MessageIdRequest::Module_Output_Current:
            _protocolCode = 0x38;
            _monitorGroup = 0x1c;
            _monitorSubAddress = 0;
            buildFrameId();
            return 1;
            break;
        case MessageIdRequest::Module_Modify_Group:
            _protocolCode = 0x38;
            _monitorGroup = 0x1c;
            _monitorSubAddress = 0;
            buildFrameId();
            return 1;
            break;
        case MessageIdRequest::Query_Single_Module_Info:
            _protocolCode = 0x38;
            _monitorGroup = 0x1c;
            _monitorSubAddress = 0;
            buildFrameId();
            return 1;
            break;
    }
    _frameId = -1;
    return -1;
}

int SinexcelSer1000::buildFrameId()
{
    _frameId = (_protocolCode << 22) + (_groupNumber << 17) + (_subAddress << 11) + (_monitorGroup << 6) + _monitorSubAddress;
    return 1;
}

int SinexcelSer1000::updateData(int msgId, int32_t value)
{
    switch (msgId)
    {
        case MessageIdRequest::Module_On_Off_32:
            _msgType = 0x03;
            _errType = 0xF0;
            _msgId[0] = (msgId & 0xFF00) >> 8 ;
            _msgId[1] = msgId & 0x00FF;
            buildData(value);
            return 1;
            break;
        case MessageIdRequest::Module_On_Off_64:
            _msgType = 0x03;
            _errType = 0xF0;
            _msgId[0] = (msgId & 0xFF00) >> 8 ;
            _msgId[1] = msgId & 0x00FF;
            buildData(value);
            return 1;
            break;
        case MessageIdRequest::Module_Online_Status_32:
            _msgType = 0x50;
            _errType = 0xF0;
            _msgId[0] = (msgId & 0xFF00) >> 8 ;
            _msgId[1] = msgId & 0x00FF;
            buildData(value);
            return 1;
            break;
        case MessageIdRequest::Module_Online_Status_64:
            _msgType = 0x50;
            _errType = 0xF0;
            _msgId[0] = (msgId & 0xFF00) >> 8 ;
            _msgId[1] = msgId & 0x00FF;
            buildData(value);
            return 1;
            break;     
        case MessageIdRequest::Module_Voltage_Mode:
            _msgType = 0x03;
            _errType = 0xF0;
            _msgId[0] = (msgId & 0xFF00) >> 8;
            _msgId[1] = msgId & 0x00FF;
            buildData(value);
            return 1;
            break;
        case MessageIdRequest::Module_Output_Voltage:
            _msgType = 0x03;
            _errType = 0xF0;
            _msgId[0] = (msgId & 0xFF00) >> 8 ;
            _msgId[1] = msgId & 0x00FF;
            buildData(value);
            return 1;
            break;
        case MessageIdRequest::Module_Output_Current:
            _msgType = 0x03;
            _errType = 0xF0;
            _msgId[0] = (msgId & 0xFF00) >> 8 ;
            _msgId[1] = msgId & 0x00FF;
            buildData(value);
            return 1;
            break;
        case MessageIdRequest::Module_Modify_Group:
            _msgType = 0x03;
            _errType = 0xF0;
            _msgId[0] = (msgId & 0xFF00) >> 8 ;
            _msgId[1] = msgId & 0x00FF;
            buildData(value);
            return 1;
            break;
        case MessageIdRequest::Query_Single_Module_Info:
            _msgType = 0x50;
            _errType = 0xF0;
            _msgId[0] = (msgId & 0xFF00) >> 8 ;
            _msgId[1] = msgId & 0x00FF;
            buildData(0);
            return 1;
            break;
    }
    _msgType = 0;
    _msgId[0] = 0;
    _msgId[1] = 0;
    return -1;
}

int SinexcelSer1000::buildData(int32_t value)
{
    _data[0] = _msgType;
    _data[1] = _errType;
    _data[2] = _msgId[0];
    _data[3] = _msgId[1];
    _data[4] = (value & 0xFF000000) >> 24;
    _data[5] = (value & 0xFF0000) >> 16;
    _data[6] = (value & 0xFF00) >> 8;
    _data[7] = value & 0xFF;
    return 1;
}

int32_t SinexcelSer1000::getFrameId()
{
    return _frameId;
}

int SinexcelSer1000::getData(int destination[], size_t arrSize)
{
    int status = -1;
    if (arrSize >= 8)
    {
        for (size_t i = 0; i < arrSize; i++)
        {
            destination[i] = _data[i];
        }
        status = 1;
    }
    return status;
}

/**
 * [int] filterExtended
 * @param id lowest id range
 * @param mask mask value
 * @return
 * 
 * To determine the mask value :
 * 1. Write the list of id that need to pass the filter
 * 2. XNOR bitwise each of ID
 * 3. Bitwise Not the result of the step 2
 * 
 * Example 1:
 * List ID:
 * ID 1 : 0b1110000000000000011100000000
 * ID 2 : 0b1011000000000000011100000000
 * XNOR : 0b1010111111111111111111111111
 * MASK : 0b0101000000000000000000000000
 * 
 * This will get id range from 0b1110000000000000011100000000 - 0b1110001111111111111111111111
 * Example 2 :
 * List ID:
 * ID 1 : 0b1110000000000000011100000000
 * ...
 * ID X : 0b1110001111111111111111111111
 * XNOR : 0b1010110000000000000000000000
 * MASK : 0b0101001111111111111111111111
*/

int SinexcelSer1000::filterExtended(long id, long mask)
{
    // uint32_t temp;
    // temp = mask;
    id &= 0x1FFFFFFF;
    // temp = ~(temp & 0x1FFFFFFF);
    Serial.print("ID Filter : ");
    Serial.println(id, BIN);

    Serial.print("Mask Filter : ");
    Serial.println(mask, BIN);

    // mask = temp;

    modifyRegister(REG_MOD, 0x17, 0x01); // reset

    writeRegister(REG_ACRn(0), id >> 21);
    writeRegister(REG_ACRn(1), id >> 13);
    writeRegister(REG_ACRn(2), id >> 5);
    writeRegister(REG_ACRn(3), id << 5);

    writeRegister(REG_AMRn(0), mask >> 21);
    writeRegister(REG_AMRn(1), mask >> 13);
    writeRegister(REG_AMRn(2), mask >> 5);
    writeRegister(REG_AMRn(3), (mask << 5) | 0x1f);

    modifyRegister(REG_MOD, 0x17, 0x00); // normal

    return 1;
}

void SinexcelSer1000::modifyRegister(uint8_t address, uint8_t mask, uint8_t value)
{
  volatile uint32_t* reg = (volatile uint32_t*)(REG_BASE + address * 4);

  *reg = (*reg & ~mask) | value;
}

void SinexcelSer1000::writeRegister(uint8_t address, uint8_t value)
{
  volatile uint32_t* reg = (volatile uint32_t*)(REG_BASE + address * 4);

  *reg = value;
}