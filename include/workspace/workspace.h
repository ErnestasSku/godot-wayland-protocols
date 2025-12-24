#pragma once

#include "ext-workspace-v1.h"
#include <cstdint>
#include <functional>
#include <string>
#include <wayland-util.h>

class Workspace {
public:
  explicit Workspace(ext_workspace_handle_v1 *handle);
  ~Workspace();

  ext_workspace_handle_v1 *handle() const { return m_handle; }

  const std::string &id() const { return m_id; }
  const std::string &name() const { return m_name; }
  const wl_array *coordinates() const { return &m_coordinates; }
  uint32_t state() const { return m_state; }
  uint32_t capabilities() const { return m_capabilities; }

  void set_id_available_callback(std::function<void(Workspace &)> cb) { m_id_available_cb = std::move(cb); }
  void set_removed_callback(std::function<void(Workspace &)> cb) { m_removed_cb = std::move(cb); }

private:
  static void handle_id(void *data, ext_workspace_handle_v1 *, const char *id);
  static void handle_name(void *data, ext_workspace_handle_v1 *, const char *name);
  static void handle_cooridinates(void *data, ext_workspace_handle_v1 *, wl_array *coordinates);
  static void handle_state(void *data, ext_workspace_handle_v1 *, uint32_t state);
  static void handle_capabilities(void *data, ext_workspace_handle_v1 *, uint32_t capabilities);
  static void handle_removed(void *data, ext_workspace_handle_v1 *);

  ext_workspace_handle_v1 *m_handle = nullptr;
  ext_workspace_handle_v1_listener m_listener = {};

  std::string m_id;
  std::string m_name;
  wl_array m_coordinates{};
  uint32_t m_state = 0;
  uint32_t m_capabilities = 0;

  std::function<void(Workspace &)> m_id_available_cb;
  std::function<void(Workspace &)> m_removed_cb;
};
