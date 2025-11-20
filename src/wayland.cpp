#include "wayland.h"
#include "godot_cpp/variant/utility_functions.hpp"
#include "wlr-screencopy-unstable-v1.h"
#include "ext-workspace-v1.h"
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>

using namespace godot;

Wayland *Wayland::singleton = nullptr;

Wayland *Wayland::get_singleton() {
  if (singleton == nullptr) {
    UtilityFunctions::print("Initializing wayland singleton");
    singleton = new Wayland();
    singleton->init_wayland();
  }

  return singleton;
}

void Wayland::init_wayland() {
  display = wl_display_connect(nullptr);

  if (!display) {
    UtilityFunctions::printerr("Failed to connect to wayland display");
    return;
  }

  registry = wl_display_get_registry(display);
  const wl_registry_listener registry_listener = {
      .global = registry_global, .global_remove = registry_remove};

  wl_registry_add_listener(registry, &registry_listener, this);
  wl_display_roundtrip(display);
}

void Wayland::registry_global(void *data, wl_registry *wl_registry, uint32_t id,
                              const char *interface, uint32_t version) {
  auto *self = static_cast<Wayland *>(data);

  // Print interfaces for now.
  UtilityFunctions::print(interface);
}

void Wayland::registry_remove(void *data, struct wl_registry *wl_registry,
                              uint32_t id) {
  // Some kind of clean up should go here
}
