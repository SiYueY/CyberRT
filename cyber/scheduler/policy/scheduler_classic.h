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

#ifndef CYBER_SCHEDULER_POLICY_SCHEDULER_CLASSIC_H_
#define CYBER_SCHEDULER_POLICY_SCHEDULER_CLASSIC_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "cyber/croutine/croutine.h"
#include "cyber/proto/classic_conf.pb.h"
#include "cyber/scheduler/scheduler.h"

namespace apollo {
namespace cyber {
namespace scheduler {

using apollo::cyber::croutine::CRoutine;
using apollo::cyber::proto::ClassicConf;
using apollo::cyber::proto::ClassicTask;

// Scheduler Classic
class SchedulerClassic : public Scheduler {
 public:
  // 删除 CRoutine
  bool RemoveCRoutine(uint64_t crid) override;
  // 删除 Task
  bool RemoveTask(const std::string& name) override;
  // 分发 Task
  bool DispatchTask(const std::shared_ptr<CRoutine>&) override;

 private:
  friend Scheduler* Instance();
  SchedulerClassic();

  // 创建 Processor
  void CreateProcessor();
  // 通知 Processor
  bool NotifyProcessor(uint64_t crid) override;

  std::unordered_map<std::string, ClassicTask> cr_confs_;  // CRoutine 配置

  ClassicConf classic_conf_;  // Classic 配置
};

}  // namespace scheduler
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_SCHEDULER_POLICY_SCHEDULER_CLASSIC_H_
