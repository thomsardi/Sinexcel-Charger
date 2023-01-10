#include <unity.h>
#include <iostream>
#include <string.h>
#include <SinexcelSer1000.h>

using namespace std;

SinexcelSer1000 sinexcelSer1000;

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}

void print()
{
    cout << "Hello world" << endl;
}

void print2()
{
    cout << "Hello World 2" << endl;
}

void printLambda (void (*function)())
{
    function();
}

void printMethod(void(*function)())
{
    function();
}

void test_can_global_broadcast()
{
    int32_t frameId;
    int data[8] = {0};
    int expectedData[8] = {0x03, 0xF0, 0x00, 0x21, 0x00, 0x00, 0x11, 0x94};
    int32_t expectedFrameId = 0xE3E0700;
    if(sinexcelSer1000.sendRequest(Module_Output_Voltage, 4500))
    {
        cout << "Success Send Request" << endl;
    }
    sinexcelSer1000.run();
    frameId = sinexcelSer1000.getFrameId();
    cout << "Frame ID : ";
    cout << frameId << endl;
    sinexcelSer1000.getData(data, 8);
    for (size_t i = 0; i < 8; i++)
    {
        cout << data[i] << endl;
    }
    TEST_ASSERT_EQUAL_INT_ARRAY(expectedData, data, 8);
    TEST_ASSERT_EQUAL(expectedFrameId, frameId);
}

void test_can_group_broadcast()
{
    int32_t frameId;
    int data[8] = {0};
    int expectedData[8] = {0x03, 0xF0, 0x00, 0x21, 0x00, 0x00, 0x11, 0x94};
    int32_t expectedFrameId = 0xE020700;
    if(sinexcelSer1000.sendRequest(Module_Output_Voltage, 4500, 1))
    {
        cout << "Success Send Request" << endl;
    }
    sinexcelSer1000.run();
    frameId = sinexcelSer1000.getFrameId();
    cout << "Frame ID : ";
    cout << frameId << endl;
    sinexcelSer1000.getData(data, 8);
    for (size_t i = 0; i < 8; i++)
    {
        cout << data[i] << endl;
    }
    TEST_ASSERT_EQUAL_INT_ARRAY(expectedData, data, 8);
    TEST_ASSERT_EQUAL(expectedFrameId, frameId);
}

void test_can_point_to_point()
{
    int32_t frameId;
    int data[8] = {0};
    int expectedData[8] = {0x03, 0xF0, 0x00, 0x21, 0x00, 0x00, 0x11, 0x94};
    int32_t expectedFrameId = 0xE021700;
    if(sinexcelSer1000.sendRequest(Module_Output_Voltage, 4500, 1, 2))
    {
        cout << "Success Send Request" << endl;
    }
    sinexcelSer1000.run();
    frameId = sinexcelSer1000.getFrameId();
    cout << "Frame ID : ";
    cout << frameId << endl;
    sinexcelSer1000.getData(data, 8);
    for (size_t i = 0; i < 8; i++)
    {
        cout << data[i] << endl;
    }
    TEST_ASSERT_EQUAL_INT_ARRAY(expectedData, data, 8);
    TEST_ASSERT_EQUAL(expectedFrameId, frameId);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    // print();
    RUN_TEST(test_can_global_broadcast);
    RUN_TEST(test_can_group_broadcast);
    RUN_TEST(test_can_point_to_point);
    UNITY_END();
    return 0;
}