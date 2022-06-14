/*
 * Copyright (c) 2022 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

#include "lse_core.h"

// XXX: target name has a '-', which is an illegal C identifier
#ifdef NODE_GYP_MODULE_NAME
#undef NODE_GYP_MODULE_NAME
#endif
#define NODE_GYP_MODULE_NAME lse_core

NAPI_MODULE_INIT() {
  // TODO: set log level from environment
  lse_core_register_yoga_listeners();

  // TODO: move these bindings to bindings.json5
  lse_core_env_init(env, exports);
  lse_core_style_init(env, exports);
  lse_core_window_init(env, exports);

  lse_core_exports(env, exports);

  napi_add_env_cleanup_hook(env, &lse_core_class_cleanup_hook, env);
  napi_add_env_cleanup_hook(env, &lse_core_env_cleanup_hook, env);

  return exports;
}
