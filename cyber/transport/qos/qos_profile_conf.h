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

#ifndef CYBER_TRANSPORT_QOS_QOS_PROFILE_CONF_H_
#define CYBER_TRANSPORT_QOS_QOS_PROFILE_CONF_H_

#include <cstdint>

#include "cyber/proto/qos_profile.pb.h"

namespace apollo {
namespace cyber {
namespace transport {

using cyber::proto::QosDurabilityPolicy;
using cyber::proto::QosHistoryPolicy;
using cyber::proto::QosProfile;
using cyber::proto::QosReliabilityPolicy;

// Qos 配置
class QosProfileConf {
 public:
  QosProfileConf();
  virtual ~QosProfileConf();

  // 创建 Qos 配置文件
  static QosProfile CreateQosProfile(const QosHistoryPolicy& history,
                                     uint32_t depth, uint32_t mps,
                                     const QosReliabilityPolicy& reliability,
                                     const QosDurabilityPolicy& durability);

  static const uint32_t QOS_HISTORY_DEPTH_SYSTEM_DEFAULT;  // Capacity of history
  static const uint32_t QOS_MPS_SYSTEM_DEFAULT;            // Messages per second

  static const QosProfile QOS_PROFILE_DEFAULT;           // 默认
  static const QosProfile QOS_PROFILE_SENSOR_DATA;       // 传感器数据
  static const QosProfile QOS_PROFILE_PARAMETERS;        // 参数
  static const QosProfile QOS_PROFILE_SERVICES_DEFAULT;  // 服务默认
  static const QosProfile QOS_PROFILE_PARAM_EVENT;       // 参数事件
  static const QosProfile QOS_PROFILE_SYSTEM_DEFAULT;    // 系统默认
  static const QosProfile QOS_PROFILE_TF_STATIC;         // TF静态
  static const QosProfile QOS_PROFILE_TOPO_CHANGE;       // 拓扑变化
};

}  // namespace transport
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_TRANSPORT_QOS_QOS_PROFILE_CONF_H_
