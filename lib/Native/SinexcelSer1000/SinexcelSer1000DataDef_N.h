enum CommandType {
    Module_On_Off_32 = 1,
    Module_On_Off_64 = 2,
    Module_Voltage_Mode = 3,
    Module_Output_Voltage = 4,
    Module_Output_Current = 5,
    Module_Modify_Group = 6,
    Query_Single_Module_Info = 7
};

struct RequestCommand {
    int cmd;
    int32_t value;
    uint8_t groupNumber;
    uint8_t subAddress;
};