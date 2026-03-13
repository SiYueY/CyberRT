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

#ifndef CYBER_CLASS_LOADER_CLASS_LOADER_REGISTER_MACRO_H_
#define CYBER_CLASS_LOADER_CLASS_LOADER_REGISTER_MACRO_H_

#include "cyber/class_loader/utility/class_loader_utility.h"

// 注册类: 创建唯一的代理类和实例，并使用基类和派生类名称注册
// Derived : 派生类
// Base    : 基类
// UniqueID: 唯一标识符，用于避免重复注册
#define CLASS_LOADER_REGISTER_CLASS_INTERNAL(Derived, Base, UniqueID)     \
  namespace {                                                             \
  struct ProxyType##UniqueID {                                            \
    ProxyType##UniqueID() {                                               \
      apollo::cyber::class_loader::utility::RegisterClass<Derived, Base>( \
          #Derived, #Base);                                               \
    }                                                                     \
  };                                                                      \
  static ProxyType##UniqueID g_register_class_##UniqueID;                 \
  }

#define CLASS_LOADER_REGISTER_CLASS_INTERNAL_1(Derived, Base, UniqueID) \
  CLASS_LOADER_REGISTER_CLASS_INTERNAL(Derived, Base, UniqueID)

// 注册类加载器
#define CLASS_LOADER_REGISTER_CLASS(Derived, Base) \
  CLASS_LOADER_REGISTER_CLASS_INTERNAL_1(Derived, Base, __COUNTER__)

#endif  // CYBER_CLASS_LOADER_CLASS_LOADER_REGISTER_MACRO_H_
