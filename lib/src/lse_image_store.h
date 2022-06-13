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

#pragma once

#include "lse_types.h"

#define LSE_IMAGE_STORE_ASYNC true
#define LSE_IMAGE_STORE_SYNC false

lse_image* lse_image_store_acquire_image(lse_image_store* store, lse_string* uri, bool async);
lse_image* lse_image_store_release_image(lse_image_store* store, lse_image* image);

lse_image* lse_image_store_get_image(lse_image_store* store, const char* uri);

void lse_image_store_destroy(lse_image_store* store);
bool lse_image_store_is_destroyed(lse_image_store* store);

lse_status lse_image_store_attach(lse_image_store* store);
void lse_image_store_detach(lse_image_store* store);

void lse_image_store_add_observer(lse_image_store* store, void* observer, lse_image_event_callback callback);
void lse_image_store_remove_observer(lse_image_store* store, void* observer);


//
// cmap_images
//

typedef struct image_resource image_resource;

struct image_resource {
  lse_image* image;
  int32_t usages;
  lse_thread_pool_task* task;
};

static image_resource image_resource_init(lse_string* uri);
static void image_resource_drop(image_resource* resource);
