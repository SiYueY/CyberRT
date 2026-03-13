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

#ifndef CYBER_SCHEDULER_POLICY_CLASSIC_CONTEXT_H_
#define CYBER_SCHEDULER_POLICY_CLASSIC_CONTEXT_H_

#include <array>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "cyber/base/atomic_rw_lock.h"
#include "cyber/croutine/croutine.h"
#include "cyber/scheduler/common/cv_wrapper.h"
#include "cyber/scheduler/common/mutex_wrapper.h"
#include "cyber/scheduler/processor_context.h"

namespace apollo {
namespace cyber {
namespace scheduler {

// 最大优先级
static constexpr uint32_t MAX_PRIO = 20;

// 默认 Group 名称
#define DEFAULT_GROUP_NAME "default_grp"

using CROUTINE_QUEUE = std::vector<std::shared_ptr<CRoutine>>;       // 协程队列
using MULTI_PRIO_QUEUE = std::array<CROUTINE_QUEUE, MAX_PRIO>;       // 多优先级队列
using CR_GROUP = std::unordered_map<std::string, MULTI_PRIO_QUEUE>;  // 协程组
using LOCK_QUEUE = std::array<base::AtomicRWLock, MAX_PRIO>;         // 锁队列
using RQ_LOCK_GROUP = std::unordered_map<std::string, LOCK_QUEUE>;   // 读写锁组

using GRP_WQ_MUTEX = std::unordered_map<std::string, MutexWrapper>;  // 互斥锁组
using GRP_WQ_CV = std::unordered_map<std::string, CvWrapper>;        // 条件变量组
using NOTIFY_GRP = std::unordered_map<std::string, int>;             // 通知组

// Classic Context
class ClassicContext : public ProcessorContext {
 public:
  ClassicContext();
  explicit ClassicContext(const std::string &group_name);

  // 下一协程
  std::shared_ptr<CRoutine> NextRoutine() override;
  // 等待
  void Wait() override;
  // 关闭
  void Shutdown() override;

  // 通知
  static void Notify(const std::string &group_name);
  // 删除协程
  static bool RemoveCRoutine(const std::shared_ptr<CRoutine> &cr);

  alignas(CACHELINE_SIZE) static CR_GROUP cr_group_;
  alignas(CACHELINE_SIZE) static RQ_LOCK_GROUP rq_locks_;
  alignas(CACHELINE_SIZE) static GRP_WQ_CV cv_wq_;
  alignas(CACHELINE_SIZE) static GRP_WQ_MUTEX mtx_wq_;
  alignas(CACHELINE_SIZE) static NOTIFY_GRP notify_grp_;

 private:
  // 初始化 Group
  void InitGroup(const std::string &group_name);

  std::chrono::steady_clock::time_point wake_time_;  // 唤醒时间
  bool need_sleep_ = false;                          // 是否需要休眠

  MULTI_PRIO_QUEUE *multi_pri_rq_ = nullptr;         // 多优先级队列
  LOCK_QUEUE *lq_ = nullptr;                         // 锁队列
  MutexWrapper *mtx_wrapper_ = nullptr;              // 互斥锁包装器
  CvWrapper *cw_ = nullptr;                          // 条件变量包装器

  std::string current_grp;                           // 当前 Group 名称
};

}  // namespace scheduler
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_SCHEDULER_POLICY_CLASSIC_CONTEXT_H_
