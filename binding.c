#include <assert.h>
#include <bare.h>
#include <js.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <windows.h>

// Matches `FIELDS` in `index.js`, which flattens each input to this many
// numbers.
#define BARE_USER32_INPUT_FIELDS 6

static js_value_t *
bare_user32__point(js_env_t *env, POINT point) {
  int err;

  js_value_t *result;
  err = js_create_object(env, &result);
  assert(err == 0);

  js_value_t *val;

  err = js_create_int32(env, point.x, &val);
  assert(err == 0);

  err = js_set_named_property(env, result, "x", val);
  assert(err == 0);

  err = js_create_int32(env, point.y, &val);
  assert(err == 0);

  err = js_set_named_property(env, result, "y", val);
  assert(err == 0);

  return result;
}

static js_value_t *
bare_user32__null(js_env_t *env) {
  int err;

  js_value_t *result;
  err = js_get_null(env, &result);
  assert(err == 0);

  return result;
}

static js_value_t *
bare_user32__window(js_env_t *env, HWND window) {
  int err;

  if (window == NULL) return bare_user32__null(env);

  js_value_t *result;
  err = js_create_bigint_uint64(env, (uint64_t) (uintptr_t) window, &result);
  assert(err == 0);

  return result;
}

static int
bare_user32__read_window(js_env_t *env, js_value_t *value, HWND *result) {
  int err;

  js_value_type_t type;
  err = js_typeof(env, value, &type);
  assert(err == 0);

  if (type != js_bigint) {
    err = js_throw_type_error(env, NULL, "Expected a window handle");
    assert(err == 0);

    return -1;
  }

  uint64_t handle;
  err = js_get_value_bigint_uint64(env, value, &handle, NULL);
  assert(err == 0);

  *result = (HWND) (uintptr_t) handle;

  return 0;
}

static js_value_t *
bare_user32__map_point(js_env_t *env, js_callback_info_t *info, BOOL (*map)(HWND, LPPOINT)) {
  int err;

  size_t argc = 3;
  js_value_t *argv[3];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 3);

  HWND window;
  if (bare_user32__read_window(env, argv[0], &window) < 0) return NULL;

  int32_t x;
  err = js_get_value_int32(env, argv[1], &x);
  assert(err == 0);

  int32_t y;
  err = js_get_value_int32(env, argv[2], &y);
  assert(err == 0);

  POINT point = {x, y};

  if (!map(window, &point)) return bare_user32__null(env);

  return bare_user32__point(env, point);
}

static js_value_t *
bare_user32_client_to_screen(js_env_t *env, js_callback_info_t *info) {
  return bare_user32__map_point(env, info, ClientToScreen);
}

static js_value_t *
bare_user32_screen_to_client(js_env_t *env, js_callback_info_t *info) {
  return bare_user32__map_point(env, info, ScreenToClient);
}

static js_value_t *
bare_user32_get_cursor_pos(js_env_t *env, js_callback_info_t *info) {
  POINT point;

  if (!GetCursorPos(&point)) return bare_user32__null(env);

  return bare_user32__point(env, point);
}

static js_value_t *
bare_user32_get_desktop_window(js_env_t *env, js_callback_info_t *info) {
  int err;

  js_value_t *result;
  err = js_create_bigint_uint64(env, (uint64_t) (uintptr_t) GetDesktopWindow(), &result);
  assert(err == 0);

  return result;
}

static js_value_t *
bare_user32_window_from_point(js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 2;
  js_value_t *argv[2];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 2);

  int32_t x;
  err = js_get_value_int32(env, argv[0], &x);
  assert(err == 0);

  int32_t y;
  err = js_get_value_int32(env, argv[1], &y);
  assert(err == 0);

  POINT point = {x, y};

  return bare_user32__window(env, WindowFromPoint(point));
}

static js_value_t *
bare_user32_get_ancestor(js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 2;
  js_value_t *argv[2];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 2);

  HWND window;
  if (bare_user32__read_window(env, argv[0], &window) < 0) return NULL;

  uint32_t flags;
  err = js_get_value_uint32(env, argv[1], &flags);
  assert(err == 0);

  return bare_user32__window(env, GetAncestor(window, flags));
}

static js_value_t *
bare_user32_get_foreground_window(js_env_t *env, js_callback_info_t *info) {
  return bare_user32__window(env, GetForegroundWindow());
}

static js_value_t *
bare_user32_set_foreground_window(js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 1;
  js_value_t *argv[1];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 1);

  HWND window;
  if (bare_user32__read_window(env, argv[0], &window) < 0) return NULL;

  js_value_t *result;
  err = js_get_boolean(env, SetForegroundWindow(window) != 0, &result);
  assert(err == 0);

  return result;
}

static js_value_t *
bare_user32_get_capture(js_env_t *env, js_callback_info_t *info) {
  return bare_user32__window(env, GetCapture());
}

static js_value_t *
bare_user32_release_capture(js_env_t *env, js_callback_info_t *info) {
  int err;

  js_value_t *result;
  err = js_get_boolean(env, ReleaseCapture() != 0, &result);
  assert(err == 0);

  return result;
}

static js_value_t *
bare_user32_get_system_metrics(js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 1;
  js_value_t *argv[1];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 1);

  int32_t index;
  err = js_get_value_int32(env, argv[0], &index);
  assert(err == 0);

  js_value_t *result;
  err = js_create_int32(env, GetSystemMetrics(index), &result);
  assert(err == 0);

  return result;
}

static js_value_t *
bare_user32_send_input(js_env_t *env, js_callback_info_t *info) {
  int err;

  size_t argc = 1;
  js_value_t *argv[1];

  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  assert(argc == 1);

  int32_t *fields;
  size_t len;
  err = js_get_typedarray_info(env, argv[0], NULL, (void **) &fields, &len, NULL, NULL);
  assert(err == 0);

  UINT count = (UINT) (len / BARE_USER32_INPUT_FIELDS);

  INPUT *inputs = calloc(count, sizeof(INPUT));

  for (UINT i = 0; i < count; i++) {
    int32_t *input = &fields[i * BARE_USER32_INPUT_FIELDS];

    inputs[i].type = input[0];

    if (input[0] == INPUT_MOUSE) {
      inputs[i].mi.dx = input[1];
      inputs[i].mi.dy = input[2];
      inputs[i].mi.mouseData = (DWORD) input[3];
      inputs[i].mi.dwFlags = (DWORD) input[4];
      inputs[i].mi.time = (DWORD) input[5];
    } else {
      inputs[i].ki.wVk = (WORD) input[1];
      inputs[i].ki.wScan = (WORD) input[2];
      inputs[i].ki.dwFlags = (DWORD) input[3];
      inputs[i].ki.time = (DWORD) input[4];
    }
  }

  UINT sent = SendInput(count, inputs, sizeof(INPUT));

  free(inputs);

  js_value_t *result;
  err = js_create_uint32(env, sent, &result);
  assert(err == 0);

  return result;
}

static js_value_t *
bare_user32_exports(js_env_t *env, js_value_t *exports) {
  int err;

#define V(name, fn) \
  { \
    js_value_t *val; \
    err = js_create_function(env, name, -1, fn, NULL, &val); \
    assert(err == 0); \
    err = js_set_named_property(env, exports, name, val); \
    assert(err == 0); \
  }

  V("clientToScreen", bare_user32_client_to_screen)
  V("screenToClient", bare_user32_screen_to_client)
  V("getCursorPos", bare_user32_get_cursor_pos)
  V("getDesktopWindow", bare_user32_get_desktop_window)
  V("windowFromPoint", bare_user32_window_from_point)
  V("getAncestor", bare_user32_get_ancestor)
  V("getForegroundWindow", bare_user32_get_foreground_window)
  V("setForegroundWindow", bare_user32_set_foreground_window)
  V("getCapture", bare_user32_get_capture)
  V("releaseCapture", bare_user32_release_capture)
  V("getSystemMetrics", bare_user32_get_system_metrics)
  V("sendInput", bare_user32_send_input)
#undef V

#define V(name, n) \
  { \
    js_value_t *val; \
    err = js_create_uint32(env, n, &val); \
    assert(err == 0); \
    err = js_set_named_property(env, exports, name, val); \
    assert(err == 0); \
  }

  V("SM_CXSCREEN", SM_CXSCREEN)
  V("SM_CYSCREEN", SM_CYSCREEN)
  V("SM_XVIRTUALSCREEN", SM_XVIRTUALSCREEN)
  V("SM_YVIRTUALSCREEN", SM_YVIRTUALSCREEN)
  V("SM_CXVIRTUALSCREEN", SM_CXVIRTUALSCREEN)
  V("SM_CYVIRTUALSCREEN", SM_CYVIRTUALSCREEN)

  V("GA_PARENT", GA_PARENT)
  V("GA_ROOT", GA_ROOT)
  V("GA_ROOTOWNER", GA_ROOTOWNER)

  V("INPUT_MOUSE", INPUT_MOUSE)
  V("INPUT_KEYBOARD", INPUT_KEYBOARD)

  V("MOUSEEVENTF_MOVE", MOUSEEVENTF_MOVE)
  V("MOUSEEVENTF_LEFTDOWN", MOUSEEVENTF_LEFTDOWN)
  V("MOUSEEVENTF_LEFTUP", MOUSEEVENTF_LEFTUP)
  V("MOUSEEVENTF_RIGHTDOWN", MOUSEEVENTF_RIGHTDOWN)
  V("MOUSEEVENTF_RIGHTUP", MOUSEEVENTF_RIGHTUP)
  V("MOUSEEVENTF_MIDDLEDOWN", MOUSEEVENTF_MIDDLEDOWN)
  V("MOUSEEVENTF_MIDDLEUP", MOUSEEVENTF_MIDDLEUP)
  V("MOUSEEVENTF_WHEEL", MOUSEEVENTF_WHEEL)
  V("MOUSEEVENTF_ABSOLUTE", MOUSEEVENTF_ABSOLUTE)
  V("MOUSEEVENTF_VIRTUALDESK", MOUSEEVENTF_VIRTUALDESK)

  V("KEYEVENTF_EXTENDEDKEY", KEYEVENTF_EXTENDEDKEY)
  V("KEYEVENTF_KEYUP", KEYEVENTF_KEYUP)
  V("KEYEVENTF_UNICODE", KEYEVENTF_UNICODE)
  V("KEYEVENTF_SCANCODE", KEYEVENTF_SCANCODE)
#undef V

  return exports;
}

BARE_MODULE(bare_user32, bare_user32_exports)
