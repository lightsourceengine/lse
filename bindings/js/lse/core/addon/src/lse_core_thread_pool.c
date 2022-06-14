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

#include "napix.h"
#include <lse_memory.h>
#include <lse_node.h>
#include <assert.h>
#include <Yoga.h>

#define i_tag tasks
#define i_key lse_thread_pool_task_ptr
#define i_opt c_no_clone | c_no_cmp
#include <stc/cset.h>

struct lse_thread_pool {
  lse_env* env;
  napi_env js_env;
  int32_t ref_count;
  cset_tasks cancellable_tasks;
};

struct lse_thread_pool_task {
  lse_thread_pool* pool;
  napi_async_work async;
  void* user_data;
  lse_thread_pool_user_data_finalize user_data_finalize;
  lse_thread_pool_work_callback work;
  lse_thread_pool_complete_callback complete;
};

static bool thread_pool_is_connected(lse_thread_pool* pool);
static void thread_pool_ref(lse_thread_pool* pool);
static void thread_pool_unref(lse_thread_pool* pool);
static void thread_pool_destroy(lse_thread_pool* pool);

static lse_thread_pool_task* thread_pool_task_new(lse_thread_pool* pool);
static void thread_pool_task_free(lse_thread_pool_task* task);

// @private
static void work(napi_env env, void* data) {
  lse_thread_pool_task* task = data;

  task->work(task->user_data);
}

// @private
static void complete(napi_env env, napi_status status, void* data) {
  lse_thread_pool_task* task = data;

  if (thread_pool_is_connected(task->pool)) {
    cset_tasks_erase(&task->pool->cancellable_tasks, task);
  }

  if (status == napi_ok) {
    task->complete(task->pool->env, task->user_data);
  } else {
    // TODO: log info?
  }

  thread_pool_task_free(data);
}

// @private
static lse_thread_pool_task* queue(
    lse_thread_pool* pool,
    const char* resource_name,
    lse_thread_pool_work_callback thread_pool_work,
    lse_thread_pool_complete_callback thread_pool_complete,
    void* user_data,
    lse_thread_pool_user_data_finalize user_data_finalize) {
  if (!thread_pool_is_connected(pool)) {
    // TODO: log warning
    return NULL;
  }

  napi_async_work async_work;
  lse_thread_pool_task* task = thread_pool_task_new(pool);

  if (napi_create_async_work(
          pool->js_env, NULL, napix_create_string(pool->js_env, resource_name), &work, &complete, task, &async_work) !=
      napi_ok) {
    // TODO: log error
    thread_pool_task_free((lse_thread_pool_task*)task);
    return NULL;
  }

  task->async = async_work;
  task->user_data = user_data;
  task->user_data_finalize = user_data_finalize;
  task->work = thread_pool_work;
  task->complete = thread_pool_complete;

  if (napi_queue_async_work(pool->js_env, async_work) != napi_ok) {
    // TODO: log error
    thread_pool_task_free((lse_thread_pool_task*)task);
    return NULL;
  }

  return task;
}

// @private
static void cancel(lse_thread_pool* pool, lse_thread_pool_task* task) {
  napi_status status;

  if (!pool || !thread_pool_is_connected(pool) || !task) {
    return;
  }

  if (!cset_tasks_contains(&pool->cancellable_tasks, task)) {
    return;
  }

  status = napi_cancel_async_work(pool->js_env, task->async);

  if (status == napi_ok) {
    // task was cancelled before getting into the libuv pool. call complete with a cancelled to clean up task memory.
    complete(pool->js_env, napi_cancelled, task->user_data);
  } else if (status != napi_generic_failure) {
    // napi_generic_failure means the task is inflight. complete callback will get a cancelled status and the task
    // will be deleted.
    // TODO: handle other errors
    cset_tasks_erase(&pool->cancellable_tasks, task);
  }
}

// @private
static bool thread_pool_is_connected(lse_thread_pool* pool) {
  return pool && pool->env;
}

// @private
static void thread_pool_ref(lse_thread_pool* pool) {
  pool->ref_count++;
}

// @private
static void thread_pool_unref(lse_thread_pool* pool) {
  assert(pool->ref_count > 0);

  if (--pool->ref_count < 1) {
    assert(pool->env == NULL);
    assert(pool->js_env == NULL);
    cset_tasks_drop(&pool->cancellable_tasks);
    free(pool);
  }
}

// @private
static void thread_pool_destroy(lse_thread_pool* pool) {
  if (pool == NULL) {
    return;
  }

  c_foreach(it, cset_tasks, pool->cancellable_tasks) {
    cancel(pool, *it.ref);
  }
  cset_tasks_clear(&pool->cancellable_tasks);

  pool->js_env = NULL;
  lse_unref(pool->env);
  pool->env = NULL;

  thread_pool_unref(pool);
}

// @private
static lse_thread_pool_task* thread_pool_task_new(lse_thread_pool* pool) {
  lse_thread_pool_task* task = lse_calloc(1, sizeof(lse_thread_pool_task));

  task->pool = pool;
  thread_pool_ref(pool);

  return task;
}

// @private
static void thread_pool_task_free(lse_thread_pool_task* task) {
  if (task->user_data && task->user_data_finalize) {
    task->user_data_finalize(task->user_data);
  }

  if (thread_pool_is_connected(task->pool) && task->async) {
    napi_delete_async_work(task->pool->js_env, task->async);
  }

  thread_pool_unref(task->pool);
  free(task);
}

// @public
void lse_core_thread_pool_install(lse_env* env, napi_env js_env) {
  lse_thread_pool* pool = lse_calloc(1, sizeof(lse_thread_pool));

  pool->env = env;
  pool->js_env = js_env;
  pool->ref_count = 1;

  lse_ref(pool->env);

  lse_env_set_thread_pool(env, (lse_thread_pool*)pool, &queue, &cancel, &thread_pool_destroy);
}

// @public
void lse_core_register_yoga_listeners() {
  YGRegisterNodeLayoutEvent(&lse_node_on_yoga_layout_event);
}
