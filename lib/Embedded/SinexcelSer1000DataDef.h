#ifndef SINEXCELSER1000_DATA_H
#define SINEXCELSER1000_DATA_H

namespace CommandType {
    enum CommandType {
        Module_On_Off_32 = 1,
        Module_On_Off_64 = 2,
        Module_Online_Status_32 = 3,
        Module_Online_Status_64 = 4,
        Module_Voltage_Mode = 5,
        Module_Output_Voltage = 6,
        Module_Output_Current = 7,
        Module_Modify_Group = 8,
        Query_Single_Module_Info = 9
    };
}


namespace MessageType {
    enum MessageType {
        Query_Controller = 0x50,
        Query_Module_Response = 0x41,
        Single_Setting_Controller = 0x03,
        Single_Setting_Module_Response = 0x43
    };
}

namespace MessageIdRequest {
    enum MessageIdRequest {
        Module_On_Off_32 = 0x0230,
        Module_On_Off_64 = 0x0232,
        Module_Online_Status_32 = 0xfffe,
        Module_Online_Status_64 = 0xffff,
        Module_Voltage_Mode = 0x0233,
        Module_Output_Voltage = 0x0021,
        Module_Output_Current = 0x0022,
        Module_Modify_Group = 0x001a,
        Query_Single_Module_Info = 0x0202,
    };
}

namespace MessageIdResponse {
    enum MessageIdResponse {
        Module_Operating_Status = 0x0202,
        Module_DC_Status = 0x0203,
        Module_AC_Status = 0x0204,
        Module_Output_Voltage = 0x0205,
        Module_Output_Current = 0x0206,
        Module_Version = 0x020a,
    };
}

namespace ErrorType {
    enum ErrorType {
        No_Error = 0xf0,
        Invalid_Node_Address = 0xf1,
        Invalid_Command = 0xf2,
        Data_Verification_Error = 0xf3,
        Address_Being_Identified = 0xf4
    };
}

struct RequestCommand {
    int msgId;
    int32_t value;
    uint8_t groupNumber;
    uint8_t subAddress;
};

struct ApiRequestCommand {
    int groupNumber = -1;
    int subAddress = -1;
    int32_t value = -1;
};

struct RawDataCharger {
    int msgId;
    int monitorGroup;
    int monitorSubAddress;
    int groupNumber;
    int subAddress;
    int32_t msgContent;
};

struct DataCharger {
    uint16_t msgCount;
    int monitorGroup = -1;
    int monitorSubAddress = -1;
    int groupNumber = -1;
    int subAddress = -1;
    int32_t current;
    int32_t voltage;
};

struct DCOperatingStatus {
    int chargerInitStatus;
    int pfcSoftStartInit_1;
    int pfcSoftStartInit_2;
    int pfcSoftStart;
    int dcdcSoftStart;
    int dcdcSoftStartWideningStage;
    int dcdcSoftStartFmStage;
    int moduleNormal;
    int moduleShutdown;
};

struct ACOperatingStatus {
    int init;
    int standby;
    int acdcSoftStart;
    int normal;
    int alarm;
    int monitorShutdown;
};

struct DCStatus_1 {
    int outputOvervoltage;
    int overTemperature;
    int hardwareFailure;
    int operatingMode;
    int fanFailure;
    int acLimitedState;
    int temperatureDerating;
    int powerLimitedState;
    int moduleOn;
    int ouputUndervoltage;
    int moduleAddressConflict;
    int currentImbalance;
    int canCommFail;
};

struct DCStatus_2 {
    int shortCircuit;
    int currentImbalance_20s;
    int alarmCapacity;
    int failureCapacity;
};

struct ACStatus_1 {
    int inputCurrentPhaseLoss;
    int inputPhaseLoss_A;
    int inputPhaseLoss_B;
    int inputPhaseLoss_C;
    int busOvervoltage;
    int busUndervoltage;
    int busVoltageImbalance;
    int busVoltageImbalanceOverLimit;
    int inputOverload;
    int inputOverloadOverlimit;
    int inputOvercurrent;
};

struct ACStatus_2 {
    int inputOvervoltage;
    int inputUndervoltage;
    int inputOverfrequency;
    int inputUnderfrequency;
    int voltageImbalance;
    int sidePhaseLockFailure;
    int voltageFastPowerDown;
    int abnormalInput;
    int busOvervoltageManyTimes;
    int wrongSoftwareVer;
    int sciCommError;
};

struct ChargerParam {
    int groupNumber;
    int subAddress;
    DCOperatingStatus dcOperatingStatus;
    ACOperatingStatus acOperatingStatus;
    DCStatus_1 dcStatus_1;
    DCStatus_2 dcStatus_2;
    ACStatus_1 acStatus_1;
    ACStatus_2 acStatus_2;
    int32_t outputVoltage;
    int32_t outputCurrent;
    int acVersionNumber;
    int dcVersionNumber;
};

#endif