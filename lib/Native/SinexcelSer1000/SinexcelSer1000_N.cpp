
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
    int status = -1;
    if (_commandList.size() > 0)
    {
        RequestCommand rc = _commandList.at(0);
        this->_groupNumber = rc.groupNumber;
        this->_subAddress = rc.subAddress;
        isFrameUpdated = updateFrameId(rc.cmd);
        isDataUpdated = updateData(rc.cmd, rc.value);
        if(isFrameUpdated && isDataUpdated)
        {
            status = 1;
        }
        _commandList.remove(0);
    }
    return status;
}

int SinexcelSer1000::sendRequest(int cmd, int32_t value, uint8_t groupNumber, uint8_t subAddress)
{
    RequestCommand rc;
    int status = -1;
    int _groupNumber, _subAddress;
    if(groupNumber > 30) //Broadcast Command
    {
        _groupNumber = 0x1F;
        _subAddress = 0;
    }
    else
    {
        _groupNumber = groupNumber;
        _subAddress = subAddress;
    }
    rc.cmd = cmd;
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

int SinexcelSer1000::updateFrameId(int cmd)
{
    switch (cmd)
    {
        case Module_On_Off_32:
            buildFrameId();
            return 1;
            break;
        case Module_On_Off_64:
            buildFrameId();
            return 1;
            break;    
        case Module_Voltage_Mode:
            buildFrameId();
            return 1;
            break;
        case Module_Output_Voltage:
            buildFrameId();
            return 1;
            break;
        case Module_Output_Current:
            buildFrameId();
            return 1;
            break;
        case Module_Modify_Group:
            buildFrameId();
            return 1;
            break;
        case Query_Single_Module_Info:
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

int SinexcelSer1000::updateData(int cmd, int32_t value)
{
    switch (cmd)
    {
        case Module_On_Off_32:
            _msgType = 0x03;
            _errType = 0xF0;
            _msgId[0] = 0x02;
            _msgId[1] = 0x30;
            buildData(value);
            return 1;
            break;
        case Module_On_Off_64:
            _msgType = 0x03;
            _errType = 0xF0;
            _msgId[0] = 0x02;
            _msgId[1] = 0x32;
            buildData(value);
            return 1;
            break;    
        case Module_Voltage_Mode:
            _msgType = 0x03;
            _errType = 0xF0;
            _msgId[0] = 0x02;
            _msgId[1] = 0x33;
            buildData(value);
            return 1;
            break;
        case Module_Output_Voltage:
            _msgType = 0x03;
            _errType = 0xF0;
            _msgId[0] = 0x0;
            _msgId[1] = 0x21;
            buildData(value);
            return 1;
            break;
        case Module_Output_Current:
            _msgType = 0x03;
            _errType = 0xF0;
            _msgId[0] = 0x0;
            _msgId[1] = 0x22;
            buildData(value);
            return 1;
            break;
        case Module_Modify_Group:
            _msgType = 0x03;
            _errType = 0xF0;
            _msgId[0] = 0x0;
            _msgId[1] = 0x1A;
            buildData(value);
            return 1;
            break;
        case Query_Single_Module_Info:
            _msgType = 0x50;
            _errType = 0xF0;
            _msgId[0] = 0x02;
            _msgId[1] = 0x02;
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