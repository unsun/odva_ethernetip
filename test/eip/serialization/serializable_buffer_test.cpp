/**
Software License Agreement (proprietary)

\file      serializable_buffer_test.cpp
\authors   Kareem Shehata <kshehata@clearpathrobotics.com>
\copyright Copyright (c) 2015, Clearpath Robotics, Inc., All rights reserved.

Redistribution and use in source and binary forms, with or without modification, is not permitted without the
express permission of Clearpath Robotics.
*/

#include <gtest/gtest.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>

#include "eip/serialization/buffer_writer.h"
#include "eip/serialization/buffer_reader.h"
#include "eip/serialization/stream_reader.h"
#include "eip/serialization/serializable_buffer.h"

using boost::shared_ptr;
using boost::make_shared;
using namespace boost::asio;
using namespace boost::iostreams;
using namespace eip::serialization;

class SerializableBufferTest : public :: testing :: Test
{

};

TEST_F(SerializableBufferTest, test_serialize)
{
  unsigned char input[] = { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0x12, 0x34 };
  shared_ptr<Serializable> sb = make_shared<SerializableBuffer> (buffer(input));

  unsigned char d[8]; 
  BufferWriter writer(buffer(d));

  sb->serialize(writer);

  EXPECT_EQ(8, writer.getByteCount());
  EXPECT_EQ(0xFF, d[0]);
  EXPECT_EQ(0xFE, d[1]);
  EXPECT_EQ(0xFD, d[2]);
  EXPECT_EQ(0xFC, d[3]);
  EXPECT_EQ(0xFB, d[4]);
  EXPECT_EQ(0xFA, d[5]);
  EXPECT_EQ(0x12, d[6]);
  EXPECT_EQ(0x34, d[7]);
}

TEST_F(SerializableBufferTest, test_write_bytes_short_buffer)
{
  unsigned char input[] = { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0x12, 0x34 };
  shared_ptr<Serializable> sb = make_shared<SerializableBuffer> (buffer(input));

  unsigned char d[7]; 
  BufferWriter writer(buffer(d));

  ASSERT_THROW(sb->serialize(writer);, std::length_error);
}

TEST_F(SerializableBufferTest, test_deserialize)
{
  unsigned char input[] = { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0x12, 0x34 };
  BufferReader reader(buffer(input));

  unsigned char d[8];  
  shared_ptr<Serializable> sb = make_shared<SerializableBuffer> (buffer(d));

  sb->deserialize(reader);

  EXPECT_EQ(8, reader.getByteCount());
  EXPECT_EQ(0xFF, d[0]);
  EXPECT_EQ(0xFE, d[1]);
  EXPECT_EQ(0xFD, d[2]);
  EXPECT_EQ(0xFC, d[3]);
  EXPECT_EQ(0xFB, d[4]);
  EXPECT_EQ(0xFA, d[5]);
  EXPECT_EQ(0x12, d[6]);
  EXPECT_EQ(0x34, d[7]);
}

TEST_F(SerializableBufferTest, test_deserialize_fixed_length)
{
  unsigned char input[] = { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0x12, 0x34 };
  basic_array_source<char> sr((const char*)input, sizeof(input));
  shared_ptr<std::istream> isp = make_shared< stream< basic_array_source<char> > > (sr);
  StreamReader reader(isp);

  shared_ptr<SerializableBuffer> sb = make_shared<SerializableBuffer> ();
  shared_ptr<Serializable> s = sb;

  s->deserialize(reader, 4);
  EXPECT_EQ(4, reader.getByteCount());

  unsigned char* d = buffer_cast<unsigned char*>(sb->getData());
  EXPECT_EQ(0xFF, d[0]);
  EXPECT_EQ(0xFE, d[1]);
  EXPECT_EQ(0xFD, d[2]);
  EXPECT_EQ(0xFC, d[3]);
}

TEST_F(SerializableBufferTest, test_deserialize_no_copy)
{
  unsigned char input[] = { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0x12, 0x34 };
  BufferReader reader(buffer(input));

  shared_ptr<SerializableBuffer> sb = make_shared<SerializableBuffer> ();
  shared_ptr<Serializable> s = sb;
  s->deserialize(reader, 4);

  EXPECT_EQ(4, reader.getByteCount());
  EXPECT_EQ(4, buffer_size(sb->getData()));
  unsigned char* d = buffer_cast<unsigned char*>(sb->getData());
  EXPECT_EQ(input, d);
}

TEST_F(SerializableBufferTest, test_deserialize_short_buffer)
{
  unsigned char input[] = { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0x12 };
  BufferReader reader(buffer(input));

  unsigned char d[8];  
  shared_ptr<Serializable> sb = make_shared<SerializableBuffer> (buffer(d));

  ASSERT_THROW(sb->deserialize(reader), std::length_error);
}

TEST_F(SerializableBufferTest, test_deserialize_no_copy_short_buffer)
{
  unsigned char input[] = { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0x12, 0x34 };
  BufferReader reader(buffer(input));

  shared_ptr<Serializable> sb = make_shared<SerializableBuffer> ();
  ASSERT_THROW(sb->deserialize(reader, 9), std::length_error);
}