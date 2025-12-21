#include <gtest/gtest.h>
#include <iostream>
#include <thread>
#include <vector>

#include "double_buffer.h"
#include "logger.h"

using namespace MinimalAudioEngine;

TEST(RealTime_DoubleBuffer_Test, CreateDoubleBuffer)
{
  DoubleBuffer<int> double_buffer(8);

  std::vector<int> read_buffer = double_buffer.get_read_buffer();
  std::vector<int> write_buffer = double_buffer.get_write_buffer();

  EXPECT_NE(&read_buffer, &write_buffer) << "Read and write buffers should be distinct instances";

  EXPECT_FALSE(double_buffer.is_read_ready()) << "Read buffer should not be ready initially";
}

TEST(RealTime_DoubleBuffer_Test, ProducerConsumerWriteRead)
{
  const int capacity = 4;
  DoubleBuffer<int> double_buffer(capacity);

  EXPECT_FALSE(double_buffer.is_read_ready()) << "Read buffer should not be ready initially";

  // Producer writes data to the write buffer
  std::vector<int>& write_buffer = double_buffer.get_write_buffer();
  for (int i = 0; i < capacity; ++i)
  {
    write_buffer[i] = i + 1; // Fill with sample data
  }
  double_buffer.publish(); // Publish the write buffer

  EXPECT_TRUE(double_buffer.is_read_ready()) << "Read buffer should be ready after publish";

  // Consumer reads data from the read buffer
  std::vector<int>& read_buffer = double_buffer.get_read_buffer();
  for (int i = 0; i < capacity; ++i)
  {
    EXPECT_EQ(read_buffer[i], i + 1) << "Data mismatch at index " << i;
  }

  EXPECT_FALSE(double_buffer.is_read_ready()) << "Read buffer should not be ready after being read";
}

TEST(RealTime_DoubleBuffer_Test, MultiplePublishReadCycles)
{
  const int capacity = 4;
  DoubleBuffer<int> double_buffer(capacity);

  for (int cycle = 0; cycle < 5; ++cycle)
  {
    // Producer writes data to the write buffer
    std::vector<int>& write_buffer = double_buffer.get_write_buffer();
    for (int i = 0; i < capacity; ++i)
    {
      write_buffer[i] = cycle * 10 + i; // Fill with sample data
    }
    double_buffer.publish(); // Publish the write buffer

    EXPECT_TRUE(double_buffer.is_read_ready()) << "Read buffer should be ready after publish in cycle " << cycle;

    // Consumer reads data from the read buffer
    std::vector<int>& read_buffer = double_buffer.get_read_buffer();
    for (int i = 0; i < capacity; ++i)
    {
      EXPECT_EQ(read_buffer[i], cycle * 10 + i) << "Data mismatch at index " << i << " in cycle " << cycle;
    }

    EXPECT_FALSE(double_buffer.is_read_ready()) << "Read buffer should not be ready after being read in cycle " << cycle;
  }
}

TEST(RealTime_DoubleBuffer_Test, ReadWithoutPublish)
{
  const int capacity = 4;
  DoubleBuffer<int> double_buffer(capacity);

  // Producer writes data to the write buffer
  std::vector<int> &write_buffer = double_buffer.get_write_buffer();
  for (int i = 0; i < capacity; ++i)
  {
    write_buffer[i] = i + 1; // Fill with sample data
  }

  EXPECT_FALSE(double_buffer.is_read_ready()) << "Read buffer should not be ready";

  // Attempt to read without any publish
  std::vector<int>& read_buffer = double_buffer.get_read_buffer();
  for (int i = 0; i < capacity; ++i)
  {
    EXPECT_EQ(read_buffer[i], 0) << "Expected default value at index " << i << " when reading without publish";
  }

  EXPECT_FALSE(double_buffer.is_read_ready()) << "Read buffer should not be ready after reading without publish";
}
