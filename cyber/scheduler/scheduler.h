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

#ifndef CYBER_SCHEDULER_SCHEDULER_H_
#define CYBER_SCHEDULER_SCHEDULER_H_

#include <unistd.h>

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "cyber/proto/choreography_conf.pb.h"

#include "cyber/base/atomic_hash_map.h"
#include "cyber/base/atomic_rw_lock.h"
#include "cyber/common/log.h"
#include "cyber/common/macros.h"
#include "cyber/common/types.h"
#include "cyber/croutine/croutine.h"
#include "cyber/croutine/routine_factory.h"
#include "cyber/scheduler/common/mutex_wrapper.h"
#include "cyber/scheduler/common/pin_thread.h"

namespace apollo {
namespace cyber {
namespace scheduler {

using apollo::cyber::base::AtomicHashMap;
using apollo::cyber::base::AtomicRWLock;
using apollo::cyber::base::ReadLockGuard;
using apollo::cyber::croutine::CRoutine;
using apollo::cyber::croutine::RoutineFactory;
using apollo::cyber::data::DataVisitorBase;
using apollo::cyber::proto::InnerThread;

class Processor;
class ProcessorContext;

// 调度器
class Scheduler {
 public:
  virtual ~Scheduler() {}
  static Scheduler* Instance();

  // 创建 Task
  bool CreateTask(const RoutineFactory& factory, const std::string& name);
  bool CreateTask(std::function<void()>&& func, const std::string& name,
                  std::shared_ptr<DataVisitorBase> visitor = nullptr);
  // 通知 Task 完成
  bool NotifyTask(uint64_t crid);

  // 关闭
  void Shutdown();
  // Task Pool 大小
  uint32_t TaskPoolSize() { return task_pool_size_; }

  // 删除 Task
  virtual bool RemoveTask(const std::string& name) = 0;

  // 进程级别资源控制
  void ProcessLevelResourceControl();
  // 设置内部线程属性
  void SetInnerThreadAttr(const std::string& name, std::thread* thr);

  // 分发 Task
  virtual bool DispatchTask(const std::shared_ptr<CRoutine>&) = 0;
  // 通知 Processor 完成 Task
  virtual bool NotifyProcessor(uint64_t crid) = 0;
  // 删除 CRoutine
  virtual bool RemoveCRoutine(uint64_t crid) = 0;

  // 监控调度状态
  void CheckSchedStatus();

  // 设置内部线程配置
  void SetInnerThreadConfs(
      const std::unordered_map<std::string, InnerThread>& confs) {
    inner_thr_confs_ = confs;
  }

 protected:
  Scheduler() : stop_(false) {}

  AtomicRWLock id_cr_lock_;                                        // 读写锁
  AtomicHashMap<uint64_t, MutexWrapper*> id_map_mutex_;            // 映射关系
  std::mutex cr_wl_mtx_;                                           // 互斥锁

  std::unordered_map<uint64_t, std::shared_ptr<CRoutine>> id_cr_;  // 任务映射
  std::vector<std::shared_ptr<ProcessorContext>> pctxs_;           // 进程上下文
  std::vector<std::shared_ptr<Processor>> processors_;             // 进程

  std::unordered_map<std::string, InnerThread> inner_thr_confs_;   // 内部线程配置

  std::string process_level_cpuset_;                               // 进程级别 CpuSet
  uint32_t proc_num_ = 0;                                          // 进程数量
  uint32_t task_pool_size_ = 0;                                    // Task Pool 大小
  std::atomic<bool> stop_;                                         // 停止标志
};

}  // namespace scheduler
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_SCHEDULER_SCHEDULER_H_
