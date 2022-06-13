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

#include "lse_render_queue.h"

#define i_val lse_render_command
#define i_tag render_commands
#define i_opt c_no_clone | c_no_cmp | c_is_fwd
#include "stc/cvec.h"

lse_render_queue lse_render_queue_init() {
  return (lse_render_queue){ .commands = cvec_render_commands_with_capacity(8) };
}

void lse_render_queue_drop(lse_render_queue* q) {
  cvec_render_commands_drop(&q->commands);
}

void lse_render_queue_push(lse_render_queue* q, lse_render_command* command) {
  cvec_render_commands_push_back(&q->commands, *command);
}

void lse_render_queue_clear(lse_render_queue* q) {
  cvec_render_commands_clear(&q->commands);
}

size_t lse_render_queue_size(lse_render_queue* q) {
  return cvec_rep_(&q->commands)->size;
}

lse_render_command* lse_render_queue_get(lse_render_queue* q, size_t i) {
  assert(i < cvec_rep_(&q->commands)->size);
  return q->commands.data + i;
}
