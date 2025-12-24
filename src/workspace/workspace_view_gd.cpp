#include "workspace/workspace_view_gd.h"

#include "godot_cpp/core/class_db.hpp"
#include "workspace/workspace.h"
#include "workspace/workspace_manager.h"
#include <cstdint>
#include <wayland-util.h>

void WorkspaceViewGD::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_runtime_id"), &WorkspaceViewGD::get_runtime_id);
  ClassDB::bind_method(D_METHOD("get_id"), &WorkspaceViewGD::get_id);
  ClassDB::bind_method(D_METHOD("get_name"), &WorkspaceViewGD::get_name);
  ClassDB::bind_method(D_METHOD("get_state"), &WorkspaceViewGD::get_state);
  ClassDB::bind_method(D_METHOD("get_capabilities"), &WorkspaceViewGD::get_capabilities);
  ClassDB::bind_method(D_METHOD("get_group_id"), &WorkspaceViewGD::get_group_id);
  ClassDB::bind_method(D_METHOD("get_coordinates"), &WorkspaceViewGD::get_coordinates);
}

void WorkspaceViewGD::_init_view(std::shared_ptr<WorkspaceManager> manager, uint64_t runtime_id) {
  m_manager = std::move(manager);
  m_runtime_id = runtime_id;
}

int64_t WorkspaceViewGD::get_runtime_id() const { return static_cast<int64_t>(m_runtime_id); }

String WorkspaceViewGD::get_id() const {
  auto manager = m_manager.lock();
  if (!manager) {
    return {};
  }

  const Workspace *w = manager->get_workspace(m_runtime_id);
  if (!w) {
    return {};
  }

  return String(w->id().c_str());
}

String WorkspaceViewGD::get_name() const {
  auto manager = m_manager.lock();
  if (!manager) {
    return {};
  }

  const Workspace *w = manager->get_workspace(m_runtime_id);
  if (!w) {
    return {};
  }

  return String(w->name().c_str());
}

int64_t WorkspaceViewGD::get_state() const {
  auto manager = m_manager.lock();
  if (!manager) {
    return 0;
  }

  const Workspace *w = manager->get_workspace(m_runtime_id);
  if (!w) {
    return 0;
  }

  return static_cast<int64_t>(w->state());
}

int64_t WorkspaceViewGD::get_capabilities() const {
  auto manager = m_manager.lock();
  if (!manager) {
    return 0;
  }

  const Workspace *w = manager->get_workspace(m_runtime_id);
  if (!w) {
    return 0;
  }

  return static_cast<int64_t>(w->capabilities());
}

int64_t WorkspaceViewGD::get_group_id() const {
  auto manager = m_manager.lock();
  if (!manager) {
    return -1;
  }

  return static_cast<int64_t>(manager->get_group_id_for_workspace(m_runtime_id));
}

PackedInt32Array WorkspaceViewGD::get_coordinates() const {
  PackedInt32Array arr;

  auto manager = m_manager.lock();
  if (!manager) {
    return arr;
  }

  const Workspace *w = manager->get_workspace(m_runtime_id);
  if (!w) {
    return arr;
  }

  const wl_array *coords = w->coordinates();
  if (!coords || coords->size == 0) {
    return arr;
  }

  const size_t count = coords->size / sizeof(uint32_t);
  arr.resize(static_cast<int>(count));

  const uint32_t *data = static_cast<const uint32_t *>(coords->data);
  for (size_t i = 0; i < count; i++) {
    arr.set(static_cast<int>(i), static_cast<int32_t>(data[i]));
  }

  return arr;
}
