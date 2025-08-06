

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "validation.hpp"

TEST(ValidationTests, CmdChecksum)
{
    EXPECT_EQ(validation::cmd_checksum(0x00), 0xFF);
    EXPECT_EQ(validation::cmd_checksum(0xFF), 0x00);
    EXPECT_EQ(validation::cmd_checksum(0x0F), 0xF0);
    EXPECT_EQ(validation::cmd_checksum(0b01010101), 0b10101010);
}

TEST(ValidationTests, BytesChecksumNull)
{
    uint8_t *arr = nullptr;
    EXPECT_EQ(validation::bytes_checksum(arr, 10), -1);
}

TEST(ValidationTests, BytesChecksumZeroVal)
{
    uint8_t arr[] = {0xFF};
    EXPECT_EQ(validation::bytes_checksum(arr, 0), 0x00);
}

TEST(ValidationTests, BytesChecksumOneVal)
{
    uint8_t arr[] = {0x12};
    EXPECT_EQ(validation::bytes_checksum(arr, sizeof(arr)), 0x12);
}

TEST(ValidationTests, BytesChecksumManyVal)
{
    uint8_t arr[] = {0x12, 0x34, 0x56};
    EXPECT_EQ(validation::bytes_checksum(arr, sizeof(arr)), 0x70);
}

TEST(ValidationTests, RunningChecksumNull)
{
    uint8_t *ptr = nullptr;
    
    validation::running_checksum(ptr, 0xFF);
    SUCCEED();
}

TEST(ValidationTests, RunningChecksumNormal)
{
    uint8_t checksum = 0x00;
    
    validation::running_checksum(&checksum, 0xFF);
    EXPECT_EQ(checksum, 0xFF);
}

TEST(ValidationTests, RunningChecksumOpposites)
{
    uint8_t checksum = 0b01010101;
    
    validation::running_checksum(&checksum, 0b10101010);
    EXPECT_EQ(checksum, 0xFF);
}