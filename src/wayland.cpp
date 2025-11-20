#include "wayland.h"
#include "godot_cpp/variant/utility_functions.hpp"
#include "wlr-screencopy-unstable-v1.h"
#include "ext-workspace-v1.h"
#include <cstdint>
#include <cstring>
#include <functional>
#include <vector>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>

using namespace godot;

Wayland *Wayland::singleton = nullptr;

Wayland::Wayland() {
  init_registry_mapper();
  init_wayland();
}

Wayland *Wayland::get_singleton() {
  if (singleton == nullptr) {
    UtilityFunctions::print("Initializing wayland singleton");
    singleton = new Wayland();
  }

  return singleton;
}

void Wayland::add_output(wl_output* output) {
  outputs.push_back(output);
}

void Wayland::init_wayland() {
  if (initialized) {
    return;
  }
  
  display = wl_display_connect(nullptr);

  if (!display) {
    UtilityFunctions::printerr("Failed to connect to wayland display");
    return;
  }

  registry = wl_display_get_registry(display);
  const wl_registry_listener registry_listener = {
      .global = registry_global, .global_remove = registry_remove};

  initialized = true;
  wl_registry_add_listener(registry, &registry_listener, this);
  wl_display_roundtrip(display);
}

void Wayland::init_registry_mapper() {
  handlers = {
      {wl_output_interface.name,
       [this](uint32_t id, uint32_t version) {
         auto output = static_cast<wl_output *>(
             wl_registry_bind(registry, id, &wl_output_interface, version));

         this->add_output(output);
       }},
      {wl_shm_interface.name,
       [this](uint32_t id, uint32_t version) {
         shm = static_cast<wl_shm *>(
             wl_registry_bind(registry, id, &wl_shm_interface, version));
       }

      },

      {zwlr_screencopy_manager_v1_interface.name,
       [this](uint32_t id, uint32_t version) {
         screencopy_manager =
             static_cast<zwlr_screencopy_manager_v1 *>(wl_registry_bind(
                 registry, id, &zwlr_screencopy_manager_v1_interface, version));
       }

      },
      {ext_workspace_manager_v1_interface.name,
       [this](uint32_t id, uint32_t version) {
         workspace_manager =
             static_cast<ext_workspace_manager_v1 *>(wl_registry_bind(
                 registry, id, &ext_workspace_manager_v1_interface, version));
       }

      }

  };
}

void Wayland::registry_global(void *data, wl_registry *wl_registry, uint32_t id,
                              const char *interface, uint32_t version) {
  auto *self = static_cast<Wayland *>(data);

  for (auto &h : self->handlers) {
    if (strcmp(interface, h.name) == 0) {
      UtilityFunctions::print("Registering object: ", interface);
      h.handler(id, version);
      return;
    }
  }
}

void Wayland::registry_remove(void *data, struct wl_registry *wl_registry,
                              uint32_t id) {
  // Some kind of clean up should go here
}
