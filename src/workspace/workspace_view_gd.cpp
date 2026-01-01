#include "workspace/workspace_view_gd.h"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/core/binder_common.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "workspace/workspace.h"
#include "workspace/workspace_manager.h"
#include <cstdint>
#include <wayland-util.h>

VARIANT_ENUM_CAST(WorkspaceViewGD::StateFlags);
VARIANT_ENUM_CAST(WorkspaceViewGD::CapabilitiesFlags);

void WorkspaceViewGD::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_runtime_id"), &WorkspaceViewGD::get_runtime_id);
  ClassDB::bind_method(D_METHOD("get_id"), &WorkspaceViewGD::get_id);
  ClassDB::bind_method(D_METHOD("get_name"), &WorkspaceViewGD::get_name);
  ClassDB::bind_method(D_METHOD("get_state"), &WorkspaceViewGD::get_state);
  ClassDB::bind_method(D_METHOD("get_capabilities"), &WorkspaceViewGD::get_capabilities);
  ClassDB::bind_method(D_METHOD("get_group_id"), &WorkspaceViewGD::get_group_id);
  ClassDB::bind_method(D_METHOD("get_coordinates"), &WorkspaceViewGD::get_coordinates);

  ClassDB::bind_method(D_METHOD("destroy"), &WorkspaceViewGD::destroy);
  ClassDB::bind_method(D_METHOD("activate"), &WorkspaceViewGD::activate);
  ClassDB::bind_method(D_METHOD("deactivate"), &WorkspaceViewGD::deactivate);
  ClassDB::bind_method(D_METHOD("assign", "group_id"), &WorkspaceViewGD::assign);
  ClassDB::bind_method(D_METHOD("remove"), &WorkspaceViewGD::remove);

  ADD_PROPERTY(PropertyInfo(Variant::INT, "state", godot::PROPERTY_HINT_FLAGS, "Active,Urgent,Hidden"), "",
               "get_state");
  BIND_ENUM_CONSTANT(ACTIVE);
  BIND_ENUM_CONSTANT(URGENT);
  BIND_ENUM_CONSTANT(HIDDEN);

  ADD_PROPERTY(PropertyInfo(Variant::INT, "capabilities", godot::PROPERTY_HINT_FLAGS), "", "get_capabilities");
  BIND_ENUM_CONSTANT(ACTIVATE);
  BIND_ENUM_CONSTANT(DEACTIVATE);
  BIND_ENUM_CONSTANT(REMOVE);
  BIND_CONSTANT(ASSIGN);

  ADD_SIGNAL(MethodInfo("id_changed", PropertyInfo(Variant::STRING, "id")));
  ADD_SIGNAL(MethodInfo("name_changed", PropertyInfo(Variant::STRING, "name")));
  ADD_SIGNAL(MethodInfo("coordinates_changed", PropertyInfo(Variant::PACKED_INT32_ARRAY, "coordinates")));
  ADD_SIGNAL(MethodInfo("workspace_state_changed",
                        PropertyInfo(Variant::INT, "state", PROPERTY_HINT_FLAGS, "WorkspaceState")));
  ADD_SIGNAL(MethodInfo("capabilities_changed", PropertyInfo(Variant::INT, "capabilities")));
  ADD_SIGNAL(MethodInfo("workspace_removed"));
}

void WorkspaceViewGD::_init_view(std::shared_ptr<WorkspaceManager> manager, Workspace *workspace, uint64_t runtime_id) {
  m_manager = std::move(manager);
  m_workspace = workspace;
  m_runtime_id = runtime_id;

  workspace->on_id_changed([this](std::string id) { call_deferred("emit_signal", "id_changed", id.c_str()); });
  workspace->on_name_changed([this](std::string name) { call_deferred("emit_signal", "name_changed", name.c_str()); });
  workspace->on_coordinates_changed([this](wl_array *coordinates) {
    PackedInt32Array arr;
    const size_t count = coordinates->size / sizeof(uint32_t);
    arr.resize(static_cast<int>(count));

    const uint32_t *data = static_cast<const uint32_t *>(coordinates->data);
    for (size_t i = 0; i < count; i++) {
      arr.set(static_cast<int>(i), static_cast<int32_t>(data[i]));
    }

    call_deferred("emit_signal", "coordinates_changed", arr);
  });
  workspace->on_state_changed(
      [this](uint32_t state) { call_deferred("emit_signal", "workspace_state_changed", state); });
  workspace->on_capabilities_changed(
      [this](uint32_t capabilities) { call_deferred("emit_signal", "capabilities_changed", capabilities); });
  workspace->on_removed([this]() { call_deferred("emit_signal", "workspace_removed"); });
}

int64_t WorkspaceViewGD::get_runtime_id() const { return static_cast<int64_t>(m_runtime_id); }

String WorkspaceViewGD::get_id() const {
  if (!m_workspace) {
    return {};
  }

  return String(m_workspace->id().c_str());
}

String WorkspaceViewGD::get_name() const {
  if (!m_workspace) {
    return {};
  }

  return String(m_workspace->name().c_str());
}

int64_t WorkspaceViewGD::get_state() const {
  if (!m_workspace) {
    return 0;
  }

  return static_cast<int64_t>(m_workspace->state());
}

int64_t WorkspaceViewGD::get_capabilities() const {
  if (!m_workspace) {
    return 0;
  }

  return static_cast<int64_t>(m_workspace->capabilities());
}

// TODO: This should be removed.
int64_t WorkspaceViewGD::get_group_id() const {
  auto manager = m_manager.lock();
  if (!manager) {
    return -1;
  }

  return static_cast<int64_t>(manager->get_group_id_for_workspace(m_runtime_id));
}

PackedInt32Array WorkspaceViewGD::get_coordinates() const {
  PackedInt32Array arr;

  if (!m_workspace) {
    return arr;
  }

  const wl_array *coords = m_workspace->coordinates();
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

void WorkspaceViewGD::destroy() const {
  if (!m_workspace) {
    return;
  }

  m_workspace->destroy();
}

void WorkspaceViewGD::activate() const {
  if (!m_workspace) {
    return;
  }

  m_workspace->activate();
}

void WorkspaceViewGD::deactivate() const {
  if (!m_workspace) {
    return;
  }

  m_workspace->deactivate();
}

void WorkspaceViewGD::assign(int32_t group_id) const {
  if (!m_workspace) {
    return;
  }

  m_workspace->assign(group_id);
}

void WorkspaceViewGD::remove() const {
  if (!m_workspace) {
    return;
  }

  m_workspace->remove();
}
