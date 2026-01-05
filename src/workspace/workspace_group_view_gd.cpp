#include "workspace/workspace_group_view_gd.h"

#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/string.hpp"
#include "workspace/workspace_group.h"
#include "workspace/workspace_manager.h"
#include <cstdint>

void WorkspaceGroupViewGD::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_id"), &WorkspaceGroupViewGD::get_id);
  ClassDB::bind_method(D_METHOD("get_capabilities"), &WorkspaceGroupViewGD::get_capabilities);

  ClassDB::bind_method(D_METHOD("create_workspace", "workspace_name"), &WorkspaceGroupViewGD::create_workspace);
  ClassDB::bind_method(D_METHOD("destroy"), &WorkspaceGroupViewGD::destroy);
}

void WorkspaceGroupViewGD::_init_view(WorkspaceGroup *group, int32_t group_id) {
  m_group = group;
  m_group_id = group_id;
}

int64_t WorkspaceGroupViewGD::get_id() const { return static_cast<int64_t>(m_group_id); }

int64_t WorkspaceGroupViewGD::get_capabilities() const {
  if (!m_group) {
    return 0;
  }

  return static_cast<int64_t>(m_group->capabilities());
}

void WorkspaceGroupViewGD::create_workspace(String workspace_name) {}

void WorkspaceGroupViewGD::destroy() {}
