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

#define _cxl_concat(a, b) a##b
#define _cxl_paste(a, b) _cxl_concat(a, b)

#define _cxl_self _cxl_paste(i_subject, _observers)
#define _cxl_self_struct _cxl_self
#define _cxl_observer_vec _cxl_paste(cvec_, _cxl_self)
#define _cxl_observer_vec_member(NAME) _cxl_paste(_cxl_observer_vec, NAME)
#define _cxl_member(NAME) _cxl_paste(_cxl_self, NAME)
#define _cxl_event _cxl_paste(i_subject, _event)
#define _cxl_event_struct _cxl_paste(_cxl_event, _)
#define _cxl_callback _cxl_paste(_cxl_event, _callback)
#define _cxl_observer _cxl_paste(_cxl_event, _observer_)

#ifdef i_forward

#include <lse.h>
#include <stc/forward.h>

//
// types
//

typedef struct _cxl_observer {
  void* observer;
  _cxl_callback observer_callback;
} _cxl_observer;

forward_cvec(_cxl_observer_vec, _cxl_observer);

typedef struct _cxl_self_struct _cxl_self;

struct _cxl_self_struct {
  _cxl_observer_vec observers;
};

//
// public api
//

_cxl_self_struct _cxl_member(_init)();
void _cxl_member(_drop)(_cxl_self* self);

// remove all entries from this observer collection
void _cxl_member(_clear)(_cxl_self* self);
// add an observer callback using observer address
void _cxl_member(_add)(_cxl_self* self, void* observer, _cxl_callback observer_callback);
// remove observer by observer address
void _cxl_member(_remove)(_cxl_self* self, void* observer);
// dispatch an event to all registered observers. safe to add/remove observers in callbacks.
void _cxl_member(_dispatch_event)(_cxl_self* self, const _cxl_event* event);

//
// test only
//

size_t _cxl_member(_size)(_cxl_self* self);

#endif // i_forward

#ifdef i_implementation

#include <assert.h>

#define i_val _cxl_observer
#define i_tag _cxl_self
#define i_opt c_no_clone | c_no_cmp | c_is_fwd
#include <stc/cvec.h>

_cxl_self_struct _cxl_member(_init)() {
  return (_cxl_self_struct){ .observers = _cxl_observer_vec_member(_init)() };
}

void _cxl_member(_drop)(_cxl_self* self) {
  if (self) {
    _cxl_observer_vec_member(_drop)(&self->observers);
  }
}

void _cxl_member(_clear)(_cxl_self* self) {
  _cxl_observer_vec_member(_clear)(&self->observers);
}

void _cxl_member(_add)(_cxl_self* self, void* observer, _cxl_callback observer_callback) {
  assert(observer != NULL);
  assert(observer_callback != NULL);

  if (observer == NULL || observer_callback == NULL) {
    return;
  }

  _cxl_observer_vec_member(
      _push_back)(&self->observers, (_cxl_observer){ .observer = observer, .observer_callback = observer_callback });
}

void _cxl_member(_remove)(_cxl_self* self, void* observer) {
  c_foreach(i, _cxl_observer_vec, self->observers) {
    if (i.ref->observer == observer) {
      i.ref->observer = NULL;
      i.ref->observer_callback = NULL;
    }
  }
}

void _cxl_member(_dispatch_event)(_cxl_self* self, const _cxl_event* event) {
  bool needs_compact = false;

  c_foreach(i, _cxl_observer_vec, self->observers) {
    if (i.ref->observer) {
      i.ref->observer_callback(event, i.ref->observer);
    } else {
      needs_compact = true;
    }
  }

  if (needs_compact) {
    size_t n = 0;
    size_t len = _cxl_observer_vec_member(_size)(self->observers);
    _cxl_observer_vec_member(_iter) it;

    while (n < len) {
      // NOTE: no mutable at(), so get index manually
      it.ref = self->observers.data + n;

      if (it.ref->observer == NULL) {
        _cxl_observer_vec_member(_erase_at)(&self->observers, it);
        len--;
      } else {
        n++;
      }
    }
  }
}

size_t _cxl_member(_size)(_cxl_self* self) {
  size_t result = 0;

  c_foreach(i, _cxl_observer_vec, self->observers) {
    if (i.ref->observer != NULL) {
      result++;
    }
  }

  return result;
}

#endif // i_implementation

#undef _cxl_concat
#undef _cxl_paste

#undef _cxl_self
#undef _cxl_self_struct
#undef _cxl_observer_vec
#undef _cxl_observer_vec_member
#undef _cxl_member
#undef _cxl_event
#undef _cxl_event_struct
#undef _cxl_callback
#undef _cxl_observer

#undef i_forward
#undef i_implementation
#undef i_subject
