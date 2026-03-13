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

#ifndef CYBER_TRANSPORT_SHM_CONDITION_NOTIFIER_H_
#define CYBER_TRANSPORT_SHM_CONDITION_NOTIFIER_H_

#include <sys/types.h>
#include <atomic>
#include <cstdint>

#include "cyber/common/macros.h"
#include "cyber/transport/shm/notifier_base.h"

namespace apollo {
namespace cyber {
namespace transport {

const uint32_t kBufLength = 4096;

// Condition Notifier: 条件变量通知器
class ConditionNotifier : public NotifierBase {

  // Indicator: 指示器
  struct Indicator {
    std::atomic<uint64_t> next_seq = {0};  // 下一序列号
    ReadableInfo infos[kBufLength];        // 通知信息
    uint64_t seqs[kBufLength] = {0};       // 序列号
  };

 public:
  virtual ~ConditionNotifier();

  // 关闭
  void Shutdown() override;
  // 通知
  bool Notify(const ReadableInfo& info) override;
  // 监听
  bool Listen(int timeout_ms, ReadableInfo* info) override;
  // 类型
  static const char* Type() { return "condition"; }

 private:
  bool Init();
  bool OpenOrCreate();
  bool OpenOnly();
  bool Remove();
  void Reset();

  key_t key_ = 0;                            // 共享内存 key
  void* managed_shm_ = nullptr;              // 管理共享内存
  size_t shm_size_ = 0;                      // 共享内存大小
  Indicator* indicator_ = nullptr;           // 指示器
  uint64_t next_seq_ = 0;                    // 下一序列号
  std::atomic<bool> is_shutdown_ = {false};  // 是否关闭

  // 单例模式
  DECLARE_SINGLETON(ConditionNotifier)
};

}  // namespace transport
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_TRANSPORT_SHM_CONDITION_NOTIFIER_H_
