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

#ifndef CYBER_BASE_SIGNAL_H_
#define CYBER_BASE_SIGNAL_H_

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <mutex>

namespace apollo {
namespace cyber {
namespace base {

template <typename... Args>
class Slot;

template <typename... Args>
class Connection;

// Signal 信号
template <typename... Args>
class Signal {
 public:
  using Callback = std::function<void(Args...)>;  // 回调函数
  using SlotPtr = std::shared_ptr<Slot<Args...>>; // 槽指针
  using SlotList = std::list<SlotPtr>;            // 槽列表
  using ConnectionType = Connection<Args...>;     // 连接类型

  Signal() {}
  virtual ~Signal() { DisconnectAllSlots(); }

  // 重载 () 运算符
  void operator()(Args... args) {
    SlotList local;
    {
      std::lock_guard<std::mutex> lock(mutex_);
      for (auto& slot : slots_) {
        local.emplace_back(slot);
      }
    }

    if (!local.empty()) {
      for (auto& slot : local) {
        (*slot)(args...);
      }
    }

    ClearDisconnectedSlots();
  }

  // 连接
  ConnectionType Connect(const Callback& cb) {
    auto slot = std::make_shared<Slot<Args...>>(cb);
    {
      std::lock_guard<std::mutex> lock(mutex_);
      slots_.emplace_back(slot);
    }

    return ConnectionType(slot, this);
  }

  // 断开连接
  bool Disconnect(const ConnectionType& conn) {
    bool find = false;
    {
      std::lock_guard<std::mutex> lock(mutex_);
      for (auto& slot : slots_) {
        if (conn.HasSlot(slot)) {
          find = true;
          slot->Disconnect();
        }
      }
    }

    if (find) {
      ClearDisconnectedSlots();
    }
    return find;
  }

  // 断开所有连接
  void DisconnectAllSlots() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& slot : slots_) {
      slot->Disconnect();
    }
    slots_.clear();
  }

 private:
  // 禁用拷贝构造函数和拷贝赋值运算符
  Signal(const Signal&) = delete;
  Signal& operator=(const Signal&) = delete;

  // 清除断开连接的槽
  void ClearDisconnectedSlots() {
    std::lock_guard<std::mutex> lock(mutex_);
    slots_.erase(
        std::remove_if(slots_.begin(), slots_.end(),
                       [](const SlotPtr& slot) { return !slot->connected(); }),
        slots_.end());
  }

  // 信号槽列表
  SlotList slots_;
  // 互斥锁
  std::mutex mutex_;
};

// Connection 连接
template <typename... Args>
class Connection {
 public:
  using SlotPtr = std::shared_ptr<Slot<Args...>>;  // 槽指针
  using SignalPtr = Signal<Args...>*;              // 信号指针

  Connection() : slot_(nullptr), signal_(nullptr) {}
  Connection(const SlotPtr& slot, const SignalPtr& signal)
      : slot_(slot), signal_(signal) {}
  virtual ~Connection() {
    slot_ = nullptr;
    signal_ = nullptr;
  }

  // 重载 = 运算符
  Connection& operator=(const Connection& another) {
    if (this != &another) {
      this->slot_ = another.slot_;
      this->signal_ = another.signal_;
    }
    return *this;
  }

  // 槽是否为 slot
  bool HasSlot(const SlotPtr& slot) const {
    if (slot != nullptr && slot_ != nullptr) {
      return slot_.get() == slot.get();
    }
    return false;
  }

  // 是否连接
  bool IsConnected() const {
    if (slot_) {
      return slot_->connected();
    }
    return false;
  }

  // 断开连接
  bool Disconnect() {
    if (signal_ && slot_) {
      return signal_->Disconnect(*this);
    }
    return false;
  }

 private:
  SlotPtr slot_;      // 槽指针
  SignalPtr signal_;  // 信号指针
};

// Slot 槽
template <typename... Args>
class Slot {
 public:
  using Callback = std::function<void(Args...)>;  // 回调函数

  // 拷贝构造函数
  Slot(const Slot& another)
      : cb_(another.cb_), connected_(another.connected_) {}
  // 构造函数
  explicit Slot(const Callback& cb, bool connected = true)
      : cb_(cb), connected_(connected) {}
  virtual ~Slot() {}

  // 重载 () 运算符
  void operator()(Args... args) {
    if (connected_ && cb_) {
      cb_(args...);
    }
  }

  // 断开连接
  void Disconnect() { connected_ = false; }
  // 是否连接
  bool connected() const { return connected_; }

 private:
  Callback cb_;            // 回调函数
  bool connected_ = true;  // 是否连接
};

}  // namespace base
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_BASE_SIGNAL_H_
