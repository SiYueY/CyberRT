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

#ifndef CYBER_TRANSPORT_TRANSPORT_H_
#define CYBER_TRANSPORT_TRANSPORT_H_

#include <atomic>
#include <memory>
#include <string>

#include "cyber/proto/transport_conf.pb.h"

#include "cyber/common/macros.h"
#include "cyber/transport/dispatcher/intra_dispatcher.h"
#include "cyber/transport/dispatcher/rtps_dispatcher.h"
#include "cyber/transport/dispatcher/shm_dispatcher.h"
#include "cyber/transport/qos/qos_profile_conf.h"
#include "cyber/transport/receiver/hybrid_receiver.h"
#include "cyber/transport/receiver/intra_receiver.h"
#include "cyber/transport/receiver/receiver.h"
#include "cyber/transport/receiver/rtps_receiver.h"
#include "cyber/transport/receiver/shm_receiver.h"
#include "cyber/transport/rtps/participant.h"
#include "cyber/transport/shm/notifier_factory.h"
#include "cyber/transport/transmitter/hybrid_transmitter.h"
#include "cyber/transport/transmitter/intra_transmitter.h"
#include "cyber/transport/transmitter/rtps_transmitter.h"
#include "cyber/transport/transmitter/shm_transmitter.h"
#include "cyber/transport/transmitter/transmitter.h"

namespace apollo {
namespace cyber {
namespace transport {

using apollo::cyber::proto::OptionalMode;

// Transport 通信机制
class Transport {
 public:
  virtual ~Transport();

  // 关闭
  void Shutdown();

  // 创建发送器 Transmitter
  template <typename M>
  auto CreateTransmitter(const RoleAttributes& attr,
                         const OptionalMode& mode = OptionalMode::HYBRID) ->
      typename std::shared_ptr<Transmitter<M>>;

  // 创建接收器 Receiver
  template <typename M>
  auto CreateReceiver(const RoleAttributes& attr,
                      const typename Receiver<M>::MessageListener& msg_listener,
                      const OptionalMode& mode = OptionalMode::HYBRID) ->
      typename std::shared_ptr<Receiver<M>>;

  // 获取 Participant
  ParticipantPtr participant() const { return participant_; }

 private:
  // 创建 Participant
  void CreateParticipant();

  // 是否已经关闭
  std::atomic<bool> is_shutdown_ = {false};
  // 参与者 Participant
  ParticipantPtr participant_ = nullptr;
  // 通知器 Notifier
  NotifierPtr notifier_ = nullptr;
  // INTRA
  IntraDispatcherPtr intra_dispatcher_ = nullptr;
  // SHM
  ShmDispatcherPtr shm_dispatcher_ = nullptr;
  // RTPS
  RtpsDispatcherPtr rtps_dispatcher_ = nullptr;

  // 单例模式
  DECLARE_SINGLETON(Transport)
};

// 创建发送器 Transmitter
template <typename M>
auto Transport::CreateTransmitter(const RoleAttributes& attr,
                                  const OptionalMode& mode) ->
    typename std::shared_ptr<Transmitter<M>> {
  if (is_shutdown_.load()) {
    AINFO << "transport has been shut down.";
    return nullptr;
  }

  std::shared_ptr<Transmitter<M>> transmitter = nullptr;
  RoleAttributes modified_attr = attr;
  if (!modified_attr.has_qos_profile()) {
    modified_attr.mutable_qos_profile()->CopyFrom(
        QosProfileConf::QOS_PROFILE_DEFAULT);
  }

  switch (mode) {
    // INTRA
    case OptionalMode::INTRA:
      transmitter = std::make_shared<IntraTransmitter<M>>(modified_attr);
      break;
    // 共享内存
    case OptionalMode::SHM:
      transmitter = std::make_shared<ShmTransmitter<M>>(modified_attr);
      break;
    // RTPS
    case OptionalMode::RTPS:
      transmitter =
          std::make_shared<RtpsTransmitter<M>>(modified_attr, participant());
      break;
    // 默认: 混合模式
    default:
      transmitter =
          std::make_shared<HybridTransmitter<M>>(modified_attr, participant());
      break;
  }

  RETURN_VAL_IF_NULL(transmitter, nullptr);
  if (mode != OptionalMode::HYBRID) {
    transmitter->Enable();
  }
  return transmitter;
}

// 创建接收器 Receiver
template <typename M>
auto Transport::CreateReceiver(
    const RoleAttributes& attr,
    const typename Receiver<M>::MessageListener& msg_listener,
    const OptionalMode& mode) -> typename std::shared_ptr<Receiver<M>> {
  if (is_shutdown_.load()) {
    AINFO << "transport has been shut down.";
    return nullptr;
  }

  std::shared_ptr<Receiver<M>> receiver = nullptr;
  RoleAttributes modified_attr = attr;
  if (!modified_attr.has_qos_profile()) {
    modified_attr.mutable_qos_profile()->CopyFrom(
        QosProfileConf::QOS_PROFILE_DEFAULT);
  }

  switch (mode) {
    // INTRA
    case OptionalMode::INTRA:
      receiver =
          std::make_shared<IntraReceiver<M>>(modified_attr, msg_listener);
      break;
    // 共享内存
    case OptionalMode::SHM:
      receiver = std::make_shared<ShmReceiver<M>>(modified_attr, msg_listener);
      break;
    // RTPS
    case OptionalMode::RTPS:
      receiver = std::make_shared<RtpsReceiver<M>>(modified_attr, msg_listener);
      break;
    // 默认: 混合模式
    default:
      receiver = std::make_shared<HybridReceiver<M>>(
          modified_attr, msg_listener, participant());
      break;
  }

  RETURN_VAL_IF_NULL(receiver, nullptr);
  if (mode != OptionalMode::HYBRID) {
    receiver->Enable();
  }
  return receiver;
}

}  // namespace transport
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_TRANSPORT_TRANSPORT_H_
