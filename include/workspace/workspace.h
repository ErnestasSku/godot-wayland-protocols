#pragma once

#include "ext-workspace-v1.h"
#include "workspace/workspace_manager.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <wayland-util.h>

class Workspace {
public:
  using IdChangedCallback = std::function<void(std::string)>;
  using NameChangedCallback = std::function<void(std::string)>;
  using CoordinatesChangedCallback = std::function<void(wl_array *)>;
  using StateChangedCallback = std::function<void(uint32_t)>;
  using CapabilitiesChangedCallback = std::function<void(uint32_t)>;
  using RemovedCallback = std::function<void()>;

  explicit Workspace(ext_workspace_handle_v1 *handle, WorkspaceManager *manager, uint64_t runtime_id);
  ~Workspace();

  void on_id_changed(IdChangedCallback cb);
  void on_name_changed(NameChangedCallback cb);
  void on_coordinates_changed(CoordinatesChangedCallback cb);
  void on_state_changed(StateChangedCallback cb);
  void on_capabilities_changed(CapabilitiesChangedCallback cb);
  void on_removed(RemovedCallback cb);

  WorkspaceManager *manager() const { return m_manager; }
  ext_workspace_handle_v1 *handle() const { return m_handle; }
  uint64_t runtime_id() const { return m_runtime_id; }

  const std::string &id() const { return m_id; }
  const std::string &name() const { return m_name; }
  const wl_array *coordinates() const { return &m_coordinates; }
  uint32_t state() const { return m_state; }
  uint32_t capabilities() const { return m_capabilities; }

  void activate();

  void set_id_available_callback(std::function<void(Workspace &)> cb) { m_id_available_cb = std::move(cb); }
  void set_removed_callback(std::function<void(Workspace &)> cb) { m_removed_manager_cb = std::move(cb); }

private:
  static void handle_id(void *data, ext_workspace_handle_v1 *, const char *id);
  static void handle_name(void *data, ext_workspace_handle_v1 *, const char *name);
  static void handle_cooridinates(void *data, ext_workspace_handle_v1 *, wl_array *coordinates);
  static void handle_state(void *data, ext_workspace_handle_v1 *, uint32_t state);
  static void handle_capabilities(void *data, ext_workspace_handle_v1 *, uint32_t capabilities);
  static void handle_removed(void *data, ext_workspace_handle_v1 *);

  WorkspaceManager *m_manager;
  ext_workspace_handle_v1 *m_handle = nullptr;
  ext_workspace_handle_v1_listener m_listener = {};

  uint64_t m_runtime_id = 0;

  std::string m_id;
  std::string m_name;
  wl_array m_coordinates{};
  uint32_t m_state = 0;
  uint32_t m_capabilities = 0;
  std::function<void(Workspace &)> m_id_available_cb;
  std::function<void(Workspace &)> m_removed_manager_cb;

  IdChangedCallback m_id_changed_cb;
  NameChangedCallback m_name_changed_cb;
  CoordinatesChangedCallback m_coordinates_changed_cb;
  StateChangedCallback m_state_changed_cb;
  CapabilitiesChangedCallback m_capabilities_changed_cb;
  RemovedCallback m_removed_cb;
};
