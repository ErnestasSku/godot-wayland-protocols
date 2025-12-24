#pragma once

#include <wayland-client-core.h>
#include <wayland-client-protocol.h>

#include <functional>
#include <string>
#include <vector>

class WaylandConnection {
public:
  using GlobalHandler = std::function<void(wl_registry *, uint32_t id, uint32_t version)>;

  static WaylandConnection &instance();

  WaylandConnection(const WaylandConnection &) = delete;
  WaylandConnection &operator=(const WaylandConnection &) = delete;

  wl_display *display() const { return m_display; }
  wl_registry *registry() const { return m_registry; }

  bool is_valid() const;

  void roundtrip() const;
  void dispatch_pending() const;
  void pump_events();

  void register_global(const char *interface, GlobalHandler handler);

private:
  WaylandConnection();
  ~WaylandConnection();

  static void handle_global(void *data, wl_registry *registry, uint32_t id, const char *interface, uint32_t version);

  static void handle_global_remove(void *data, wl_registry *registry, uint32_t id);

private:
  wl_display *m_display = nullptr;
  wl_registry *m_registry = nullptr;

  wl_registry_listener m_registry_listener{};

  struct HandlerEntry {
    const char *interface;
    GlobalHandler handler;
  };

  struct GlobalEntry {
    std::string interface;
    uint32_t id = 0;
    uint32_t version = 0;
  };

  std::vector<HandlerEntry> m_handlers;
  std::vector<GlobalEntry> m_globals;
};
