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

struct RequestCommand {
    int cmd;
    int32_t value;
    uint8_t groupNumber;
    uint8_t subAddress;
};

struct ChargerParam {
    int groupNumber;
    int subAddress;
    int dcOperatingStatus;
    int acOperatingStatus;
    int dcStatus_1;
    int dcStatus_2;
    int acStatus_1;
    int acStatus_2;
    int32_t outputVoltage;
    int32_t outputCurrent;
    int acVersionNumber;
    int dcVersionNumber;
};