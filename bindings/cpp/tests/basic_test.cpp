/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "opendal.hpp"
#include "gtest/gtest.h"
#include <ctime>
#include <optional>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

class OpendalTest : public ::testing::Test {
protected:
  opendal::Operator op;

  std::string scheme;
  std::unordered_map<std::string, std::string> config;

  // random number generator
  std::mt19937 rng;

  void SetUp() override {
    scheme = "memory";
    rng.seed(time(nullptr));

    op = opendal::Operator(scheme, config);
    EXPECT_TRUE(op.available());
  }
};

// Scenario: OpenDAL Blocking Operations
TEST_F(OpendalTest, BasicTest) {
  std::string file_path = "test";
  std::string file_path_copied = "test_copied";
  std::string file_path_renamed = "test_renamed";
  std::string dir_path = "test_dir/";
  std::vector<uint8_t> data = {1, 2, 3, 4, 5};

  // write
  op.write(file_path, data);

  // read
  auto res = op.read(file_path);
  EXPECT_EQ(res, data);

  // is_exist
  EXPECT_TRUE(op.is_exist(file_path));

  // create_dir
  op.create_dir(dir_path);
  EXPECT_TRUE(op.is_exist(dir_path));

  // copy
  op.copy(file_path, file_path_copied);
  EXPECT_TRUE(op.is_exist(file_path_copied));

  // rename
  op.rename(file_path_copied, file_path_renamed);
  EXPECT_TRUE(op.is_exist(file_path_renamed));

  // stat
  auto metadata = op.stat(file_path);
  EXPECT_EQ(metadata.type, opendal::EntryMode::FILE);
  EXPECT_EQ(metadata.content_length, data.size());

  // list
  auto list_file_path = dir_path + file_path;
  op.write(list_file_path, data);
  auto entries = op.list(dir_path);
  EXPECT_EQ(entries.size(), 1);
  EXPECT_EQ(entries[0].path, list_file_path);

  // remove
  op.remove(file_path_renamed);
  op.remove(dir_path);
  EXPECT_FALSE(op.is_exist(file_path_renamed));
}

TEST_F(OpendalTest, ReaderTest) {
  std::string file_path = "test";
  constexpr int size = 2000;
  std::vector<uint8_t> data(size);

  for (auto &d : data) {
    d = rng() % 256;
  }

  // write
  op.write(file_path, data);

  // reader
  auto reader = op.reader(file_path);
  uint8_t part_data[100];
  reader.seek(200, std::ios::cur);
  reader.read(part_data, 100);
  EXPECT_EQ(reader.seek(0, std::ios::cur), 300);
  for (int i = 0; i < 100; ++i) {
    EXPECT_EQ(part_data[i], data[200 + i]);
  }
  reader.seek(0, std::ios::beg);

  // stream
  opendal::ReaderStream stream(reader);

  auto read_fn = [&](std::size_t to_read, std::streampos expected_tellg) {
    std::vector<char> v(to_read);
    stream.read(v.data(), v.size());
    EXPECT_TRUE(!!stream);
    EXPECT_EQ(stream.tellg(), expected_tellg);
  };

  EXPECT_EQ(stream.tellg(), 0);
  read_fn(10, 10);
  read_fn(15, 25);
  read_fn(15, 40);
  stream.get();
  EXPECT_EQ(stream.tellg(), 41);
  read_fn(1000, 1041);

  stream.seekg(0, std::ios::beg);
  std::vector<uint8_t> reader_data(std::istreambuf_iterator<char>{stream}, {});
  EXPECT_EQ(reader_data, data);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
