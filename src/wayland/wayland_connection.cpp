#include "wayland/wayland_connection.h"
#include "godot_cpp/variant/utility_functions.hpp"
#include <cstring>
#include <poll.h>
#include <wayland-client-core.h>

WaylandConnection &WaylandConnection::instance() {
  static WaylandConnection instance;
  return instance;
}

WaylandConnection::WaylandConnection() {
  m_display = wl_display_connect(nullptr);
  if (!m_display) {
    godot::UtilityFunctions::printerr("Wayland failed to connect.");
    return;
  }

  m_registry = wl_display_get_registry(m_display);

  m_registry_listener.global = &WaylandConnection::handle_global;
  m_registry_listener.global_remove = &WaylandConnection::handle_global_remove;

  wl_registry_add_listener(m_registry, &m_registry_listener, this);
  wl_display_roundtrip(m_display);
}

WaylandConnection::~WaylandConnection() {
  if (m_display) {
    wl_display_disconnect(m_display);
    m_display = nullptr;
  }
}

bool WaylandConnection::is_valid() const { return m_display && m_registry; }

void WaylandConnection::roundtrip() const {
  if (m_display) {
    wl_display_roundtrip(m_display);
  }
}

void WaylandConnection::dispatch_pending() const {
  if (m_display) {
    wl_display_dispatch_pending(m_display);
  }
}

void WaylandConnection::pump_events() {
  if (!m_display) {
    static bool warned = false;
    if (!warned) {
      godot::UtilityFunctions::print("No display");
      warned = true;
    }
    return;
  }

  wl_display_dispatch_pending(m_display);

  while (wl_display_prepare_read(m_display) != 0) {
    wl_display_dispatch_pending(m_display);
  }

  wl_display_flush(m_display);

  int fd = wl_display_get_fd(m_display);
  struct pollfd pfd {
    .fd = fd,
    .events = POLLIN,
    .revents = 0,
  };

  int r = poll(&pfd, 1, 0);
  if (r > 0 && (pfd.revents & POLLIN)) {
    wl_display_read_events(m_display);
    wl_display_dispatch_pending(m_display);
  } else {
    wl_display_cancel_read(m_display);
  }
}

void WaylandConnection::register_global(const char *interface, GlobalHandler handler) {
  m_handlers.push_back({interface, std::move(handler)});

  if (!m_registry) {
    return;
  }

  for (const auto &g : m_globals) {
    if (g.interface == interface) {
      m_handlers.back().handler(m_registry, g.id, g.version);
    }
  }
}

void WaylandConnection::handle_global(void *data, wl_registry *registry, uint32_t id, const char *interface,
                                      uint32_t version) {
  auto *self = static_cast<WaylandConnection *>(data);

  self->m_globals.push_back({interface, id, version});

  for (auto &h : self->m_handlers) {
    if (std::strcmp(h.interface, interface) == 0) {
      h.handler(registry, id, version);
    }
  }
}

void WaylandConnection::handle_global_remove(void *, wl_registry *, uint32_t) {
  // Optional cleanup later
}
