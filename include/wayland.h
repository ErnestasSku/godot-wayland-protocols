#ifndef WAYLAND_H
#define WAYLAND_H

#include <cstdint>
#include <string>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>

extern "C" {
#include "wlr-screencopy-unstable-v1.h"
#include "ext-workspace-v1.h"
}

class Wayland {
public:
  static Wayland *get_singleton(); 
  wl_display *display = nullptr;
  wl_registry *registry = nullptr;

  // Protocol specific handles
  ext_workspace_manager_v1 *workspace_manager = nullptr;
  zwlr_screencopy_manager_v1 *screencopy_manager = nullptr;
  
  std::string test;
  int test_counter = 0;

private:
  static Wayland *singleton;
  static void registry_global(void *data, wl_registry *wl_registry, uint32_t id,
                              const char *interface, uint32_t version);
  static void registry_remove(void *data, struct wl_registry *wl_registry,
                              uint32_t id);
  void init_wayland();
  bool initialized = false;
  friend class Workspace;
};

#endif
