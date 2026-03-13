/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef CYBER_TRANSPORT_SHM_SEGMENT_H_
#define CYBER_TRANSPORT_SHM_SEGMENT_H_

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "cyber/transport/shm/block.h"
#include "cyber/transport/shm/shm_conf.h"
#include "cyber/transport/shm/state.h"

namespace apollo {
namespace cyber {
namespace transport {

class Segment;
using SegmentPtr = std::shared_ptr<Segment>;

struct WritableBlock {
  uint32_t index = 0;
  Block* block = nullptr;
  uint8_t* buf = nullptr;
};
using ReadableBlock = WritableBlock;

// Segment: 段
class Segment {
 public:
  explicit Segment(uint64_t channel_id);
  virtual ~Segment() {}

  // 获取 Block 以写入数据
  bool AcquireBlockToWrite(std::size_t msg_size, WritableBlock* writable_block);
  // 释放已写入的 Block
  void ReleaseWrittenBlock(const WritableBlock& writable_block);

  // 获取 Arena Block 以写入数据
  bool AcquireArenaBlockToWrite(
    std::size_t msg_size, WritableBlock* writable_block);
  // 释放已写入的 Arena Block
  void ReleaseArenaWrittenBlock(const WritableBlock& writable_block);

  // 获取 Block 以读取数据
  bool AcquireBlockToRead(ReadableBlock* readable_block);
  // 释放已读取的 Block
  void ReleaseReadBlock(const ReadableBlock& readable_block);

  // 获取 Arena Block 以读取数据
  bool AcquireArenaBlockToRead(ReadableBlock* readable_block);
  // 释放已读取的 Arena Block
  void ReleaseArenaReadBlock(const ReadableBlock& readable_block);

  bool InitOnly(uint64_t message_size);
  void* GetManagedShm();
  bool LockBlockForWriteByIndex(uint64_t block_index);
  bool ReleaseBlockForWriteByIndex(uint64_t block_index);
  bool LockBlockForReadByIndex(uint64_t block_index);
  bool ReleaseBlockForReadByIndex(uint64_t block_index);

  bool LockArenaBlockForWriteByIndex(uint64_t block_index);
  bool ReleaseArenaBlockForWriteByIndex(uint64_t block_index);
  bool LockArenaBlockForReadByIndex(uint64_t block_index);
  bool ReleaseArenaBlockForReadByIndex(uint64_t block_index);

 protected:
  virtual bool Destroy();
  virtual void Reset() = 0;
  virtual bool Remove() = 0;
  virtual bool OpenOnly() = 0;
  virtual bool OpenOrCreate() = 0;

  bool init_;                                                     // 是否已经初始化
  ShmConf conf_;                                                  // 共享内存配置
  uint64_t channel_id_;                                           // Channle ID

  State* state_;                                                  // 状态
  Block* blocks_;                                                 // Block 块
  Block* arena_blocks_;                                           // Arena Block 块
  void* managed_shm_;                                             // 管理的共享内存
  std::mutex block_buf_lock_;                                     // Block 块缓冲区互斥锁
  std::mutex arena_block_buf_lock_;                               // Arena Block 块缓冲区互斥锁
  std::unordered_map<uint32_t, uint8_t*> block_buf_addrs_;        // Block 块缓冲区地址
  std::unordered_map<uint32_t, uint8_t*> arena_block_buf_addrs_;  // Arena Block 块缓冲区地址

 private:
  bool Remap();
  bool Recreate(const uint64_t& msg_size);
  uint32_t GetNextWritableBlockIndex();
  uint32_t GetNextArenaWritableBlockIndex();
};

}  // namespace transport
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_TRANSPORT_SHM_SEGMENT_H_
