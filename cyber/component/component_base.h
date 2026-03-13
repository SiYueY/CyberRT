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

#ifndef CYBER_COMPONENT_COMPONENT_BASE_H_
#define CYBER_COMPONENT_COMPONENT_BASE_H_

#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include "gflags/gflags.h"

#include "cyber/proto/component_conf.pb.h"

#include "cyber/class_loader/class_loader.h"
#include "cyber/common/environment.h"
#include "cyber/common/file.h"
#include "cyber/node/node.h"
#include "cyber/scheduler/scheduler.h"

namespace apollo {
namespace cyber {

using apollo::cyber::proto::ComponentConfig;
using apollo::cyber::proto::TimerComponentConfig;

// 基础组件
class ComponentBase : public std::enable_shared_from_this<ComponentBase> {
 public:
  template <typename M>
  using Reader = cyber::Reader<M>;

  virtual ~ComponentBase() {}

  // 初始化[普通组件]
  virtual bool Initialize(const ComponentConfig& config) { return false; }
  // 初始化[定时组件]
  virtual bool Initialize(const TimerComponentConfig& config) { return false; }
  
  // 关闭
  virtual void Shutdown() {
    if (is_shutdown_.exchange(true)) {
      return;
    }

    Clear();
    for (auto& reader : readers_) {
      reader->Shutdown();
    }
    scheduler::Instance()->RemoveTask(node_->Name());
  }

  // 获取 Proto 格式配置
  template <typename T>
  bool GetProtoConfig(T* config) const {
    return common::GetProtoFromFile(config_file_path_, config);
  }

 protected:
  virtual bool Init() = 0;
  virtual void Clear() { return; }

  // 配置文件路径
  const std::string& ConfigFilePath() const { return config_file_path_; }

  // 加载配置文件[普通组件]
  void LoadConfigFiles(const ComponentConfig& config) {
    // 获取配置文件路径
    if (!config.config_file_path().empty()) {
      if (!common::GetFilePathWithEnv(config.config_file_path(),
                                      "APOLLO_CONF_PATH", &config_file_path_)) {
        AERROR << "conf file [" << config.config_file_path() << "] not found!";
        config_file_path_ = config.config_file_path();
      } else {
        AINFO << "use config file: " << config_file_path_;
      }
    }

    // 设置 Flag 文件路径
    if (!config.flag_file_path().empty()) {
      std::string flag_file_path = config.flag_file_path();
      if (!common::GetFilePathWithEnv(config.flag_file_path(),
                                      "APOLLO_FLAG_PATH", &flag_file_path)) {
        AERROR << "flag file [" << config.flag_file_path() << "] not found!";
      } else {
        AINFO << "use flag file: " << flag_file_path;
      }
      google::SetCommandLineOption("flagfile", flag_file_path.c_str());
    }
  }
  
  // 加载配置文件[定时组件]
  void LoadConfigFiles(const TimerComponentConfig& config) {
    if (!config.config_file_path().empty()) {
      if (!common::GetFilePathWithEnv(config.config_file_path(),
                                      "APOLLO_CONF_PATH", &config_file_path_)) {
        AERROR << "conf file [" << config.config_file_path() << "] not found!";
        config_file_path_ = config.config_file_path();
      } else {
        AINFO << "use config file: " << config_file_path_;
      }
    }

    if (!config.flag_file_path().empty()) {
      std::string flag_file_path = config.flag_file_path();
      if (!common::GetFilePathWithEnv(config.flag_file_path(),
                                      "APOLLO_FLAG_PATH", &flag_file_path)) {
        AERROR << "flag file [" << config.flag_file_path() << "] not found!";
      } else {
        AINFO << "use flag file: " << flag_file_path;
      }
      google::SetCommandLineOption("flagfile", flag_file_path.c_str());
    }
  }

  // 是否关闭
  std::atomic<bool> is_shutdown_ = {false};
  // 配置文件路径
  std::string config_file_path_ = "";
  // Node
  std::shared_ptr<Node> node_ = nullptr;
  // Readers
  std::vector<std::shared_ptr<ReaderBase>> readers_;
};

}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_COMPONENT_COMPONENT_BASE_H_
