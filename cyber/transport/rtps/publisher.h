/******************************************************************************
 * Copyright 2024 The Apollo Authors. All Rights Reserved.
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
#ifndef CYBER_TRANSPORT_RTPS_PUBLISHER_H
#define CYBER_TRANSPORT_RTPS_PUBLISHER_H

#include <memory>
#include <string>

#include "fastdds/dds/domain/DomainParticipant.hpp"
#include "fastdds/dds/domain/DomainParticipantFactory.hpp"
#include "fastdds/dds/publisher/DataWriter.hpp"
#include "fastdds/dds/publisher/DataWriterListener.hpp"
#include "fastdds/dds/publisher/Publisher.hpp"
#include "fastdds/dds/topic/Topic.hpp"
#include "fastdds/dds/topic/TypeSupport.hpp"
#include "cyber/proto/qos_profile.pb.h"
#include "cyber/base/macros.h"
#include "cyber/common/log.h"
#include "cyber/transport/message/message_info.h"
#include "cyber/transport/qos/qos_filler.h"
#include "cyber/transport/rtps/underlay_message.h"

namespace apollo {
namespace cyber {
namespace transport {

class Publisher;
using PublisherPtr = std::shared_ptr<Publisher>;

// Publisher 发布者
class Publisher {
 public:
  Publisher(const std::string& channel_name, const proto::QosProfile& qos,
            eprosima::fastdds::dds::DomainParticipant* participant);
  virtual ~Publisher();

  // 初始化
  bool Init();
  // 发送消息
  bool Write(const UnderlayMessage& msg, bool is_topo_msg = false);
  bool Write(const UnderlayMessage& msg, const MessageInfo& msg_info,
             bool is_topo_msg = false);
  // 关闭
  void Shutdown();

 private:
  // 禁用拷贝构造函数和拷贝赋值运算符
  Publisher(const Publisher&) = delete;
  Publisher& operator=(const Publisher&) = delete;
  // 创建 Topic
  bool EnsureCreateTopic(const std::string& channel_name);

  std::string channel_name_;                                  // Channel 名称
  proto::QosProfile qos_;                                     // QoSProfile
  std::atomic<bool> shutdown_;                                // 关闭标志

  eprosima::fastdds::dds::DomainParticipant* participant_;    // Participant
  eprosima::fastdds::dds::Publisher* publisher_;              // Publisher
  eprosima::fastdds::dds::Topic* topic_;                      // Topic
  eprosima::fastdds::dds::DataWriter* writer_;                // DataWriter
};

}  // namespace transport
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_TRANSPORT_RTPS_PUBLISHER_H
