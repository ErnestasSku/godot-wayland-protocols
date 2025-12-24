#include "workspace/workspace_group_view_gd.h"

#include "godot_cpp/core/class_db.hpp"
#include "workspace/workspace_group.h"
#include "workspace/workspace_manager.h"

void WorkspaceGroupViewGD::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_id"), &WorkspaceGroupViewGD::get_id);
  ClassDB::bind_method(D_METHOD("get_capabilities"), &WorkspaceGroupViewGD::get_capabilities);
}

void WorkspaceGroupViewGD::_init_view(std::shared_ptr<WorkspaceManager> manager, int32_t group_id) {
  m_manager = std::move(manager);
  m_group_id = group_id;
}

int64_t WorkspaceGroupViewGD::get_id() const { return static_cast<int64_t>(m_group_id); }

int64_t WorkspaceGroupViewGD::get_capabilities() const {
  auto manager = m_manager.lock();
  if (!manager) {
    return 0;
  }

  const WorkspaceGroup *g = manager->get_group(m_group_id);
  if (!g) {
    return 0;
  }

  return static_cast<int64_t>(g->capabilities());
}
