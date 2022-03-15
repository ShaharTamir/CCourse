#include <string.h>

#include <tester.h>
#include "../parser.h"

static void TestValidName(TestStatusType*);
static void TestIsLabel(TestStatusType*);
static void TestValidData(TestStatusType*);
static void TestValidString(TestStatusType*);

TestFunction tests[] = {TestValidName, TestIsLabel, TestValidData, TestValidString};

int main()
{
    TestStatusType status = {0, 0, 0};
    
    RunTests(tests, &status, sizeof(tests) / sizeof(TestFunction));
    PrintSummary(&status);

    return 0;
}

void TestValidName(TestStatusType *status)
{
    char valid_name[] = {"m1\0"};
    char invalid_name[] = {"r1_\0"};
    char invalid_name2[] = {"mov\0"};
    char invalid_name3[] = {"m1:\0"};

    PrintTestSubject("VALID LABEL NAME");
    PrepareForTest("check a valid name", status);
    CheckResult(ParserValidateName(valid_name), __LINE__, status);

    PrepareForTest("check invalid name with invalid char in name", status);
    CheckResult(!ParserValidateName(invalid_name), __LINE__, status);

    PrepareForTest("check invalid name with function name", status);
    CheckResult(!ParserValidateName(invalid_name2), __LINE__, status);

    PrepareForTest("check invalid name with label def ending", status);
    CheckResult(!ParserValidateName(invalid_name3), __LINE__, status);
}

void TestIsLabel(TestStatusType *status)
{
    char label[] = {"X:\0"};
    char label2[] = {"somthing123:\0"};
    char not_label[] = {"mov:\0"};

    PrintTestSubject("NEW LABEL DEF");
    PrepareForTest("check a valid label def", status);
    CheckResult(ParserIsNewLabel(label), __LINE__, status);

    PrepareForTest("check another valid label def with numbers in name", status);
    CheckResult(ParserIsNewLabel(label2), __LINE__, status);

    PrepareForTest("check a valid label def and verify name is invalid", status);
    CheckResult(ParserIsNewLabel(not_label) && !ParserValidateName(not_label), __LINE__, status);
}

void TestValidData(TestStatusType *status)
{
    char valid_single_data[] = {"   7    \0"};
    char valid_array[] = {"   -1,  +7  ,  2   ,+6\0"};
    char invalid_char_single_data[] = {"7:\0"};
    char invalid_char_single_data2[] = {"7- \0"};
    char invalid_char_single_data3[] = {"7 l \0"};
    char invalid_char_single_data4[] = {",7\0"};
    char invalid_char_single_data5[] = {"+ 7\0"};
    char invalid_array[] = {" +7, 11, -2,\0"};
    char invalid_array2[] = {" 1, 2, 3    ,   \0"};

    PrintTestSubject("VALIDATE DATA");
    PrepareForTest("check a valid single data", status);
    CheckResult(ParserIsValidData(valid_single_data, 0, strlen(valid_single_data)), __LINE__, status);

    PrepareForTest("check a valid array", status);
    CheckResult(ParserIsValidData(valid_array, 0, strlen(valid_array)), __LINE__, status);

    PrepareForTest("check invalid single data - invalid char after num no gaps", status);
    CheckResult(!ParserIsValidData(invalid_char_single_data, 0, strlen(invalid_char_single_data)), __LINE__, status);

    PrepareForTest("check invalid single data - invalid char straight after num gap only to end str", status);
    CheckResult(!ParserIsValidData(invalid_char_single_data2, 0, strlen(invalid_char_single_data2)), __LINE__, status);

    PrepareForTest("check invalid single data - invalid char after num - gaps", status);
    CheckResult(!ParserIsValidData(invalid_char_single_data3, 0, strlen(invalid_char_single_data3)), __LINE__, status);

    PrepareForTest("check invalid single data - invalid char before num", status);
    CheckResult(!ParserIsValidData(invalid_char_single_data4, 0, strlen(invalid_char_single_data4)), __LINE__, status);

    PrepareForTest("check invalid single data - invalid sign without following number", status);
    CheckResult(!ParserIsValidData(invalid_char_single_data5, 0, strlen(invalid_char_single_data5)), __LINE__, status);

    PrepareForTest("check invalid array - end with num-separator no gaps", status);
    CheckResult(!ParserIsValidData(invalid_array, 0, strlen(invalid_array)), __LINE__, status);

    PrepareForTest("check invalid array - end with num-separator gaps", status);
    CheckResult(!ParserIsValidData(invalid_array2, 0, strlen(invalid_array2)), __LINE__, status);
}

void TestValidString(TestStatusType *status)
{
    char valid_string[] = {" \"  7  \"  \"\0"};
    char valid_string2[] = {"\" -1,  +7  ,  2   ,+6 \"      \0"};
    char valid_string3[] = {"\"    \"\0"};
    char invalid_string[] = {"\"\0"};
    char invalid_string2[] = {"7 l \"\0"};
    char invalid_string3[] = {"\",7  l   \0"};
    char invalid_string4[] = {"1, 2, 3\0"};

    PrintTestSubject("VALIDATE DATA");
    PrepareForTest("valid string with \" in middle and last char is closing", status);
    CheckResult(ParserIsValidString(valid_string, 0, strlen(valid_string)), __LINE__, status);

    PrepareForTest("valid string closer and delim have gaps ", status);
    CheckResult(ParserIsValidString(valid_string2, 0, strlen(valid_string2)), __LINE__, status);

    PrepareForTest("valid string - empty. string closer is next to delim", status);
    CheckResult(ParserIsValidString(valid_string3, 0, strlen(valid_string3)), __LINE__, status);

    PrepareForTest("invalid string - only open, no other chars", status);
    CheckResult(!ParserIsValidString(invalid_string, 0, strlen(invalid_string)), __LINE__, status);

    PrepareForTest("invalid string - only close - other data before", status);
    CheckResult(!ParserIsValidString(invalid_string2, 0, strlen(invalid_string2)), __LINE__, status);

    PrepareForTest("invalid string - open, no close, with data in middle", status);
    CheckResult(!ParserIsValidString(invalid_string3, 0, strlen(invalid_string3)), __LINE__, status);

    PrepareForTest("invalid string - no string at all", status);
    CheckResult(!ParserIsValidString(invalid_string4, 0, strlen(invalid_string4)), __LINE__, status);
}