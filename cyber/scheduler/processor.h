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

#ifndef CYBER_SCHEDULER_PROCESSOR_H_
#define CYBER_SCHEDULER_PROCESSOR_H_

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "cyber/proto/scheduler_conf.pb.h"

#include "cyber/croutine/croutine.h"
#include "cyber/scheduler/processor_context.h"

namespace apollo {
namespace cyber {
namespace scheduler {

using croutine::CRoutine;

// 快照
struct Snapshot {
  std::atomic<uint64_t> execute_start_time = {0};  // 开始执行时间
  std::atomic<pid_t> processor_id = {0};           // 进程 ID
  std::string routine_name;                        // 协程名称
};

// 进程
class Processor {
 public:
  Processor();
  virtual ~Processor();

  // 运行
  void Run();
  // 停止
  void Stop();
  // 绑定上下文
  void BindContext(const std::shared_ptr<ProcessorContext>& context);
  // 线程
  std::thread* Thread() { return &thread_; }
  // 线程 ID
  std::atomic<pid_t>& Tid();

  // 进程快照
  std::shared_ptr<Snapshot> ProcSnapshot() { return snap_shot_; }

 private:
  std::shared_ptr<ProcessorContext> context_;                           // 上下文

  std::condition_variable cv_ctx_;                                      // 条件变量
  std::once_flag thread_flag_;                                          // 线程标志
  std::mutex mtx_ctx_;                                                  // 互斥锁
  std::thread thread_;                                                  // 线程

  std::atomic<pid_t> tid_{-1};                                          // 线程 ID
  std::atomic<bool> running_{false};                                    // 运行标志

  std::shared_ptr<Snapshot> snap_shot_ = std::make_shared<Snapshot>();  // 快照
};

}  // namespace scheduler
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_SCHEDULER_PROCESSOR_H_
