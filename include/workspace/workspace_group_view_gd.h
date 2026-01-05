#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/variant/string.hpp"
#include "workspace/workspace_group.h"
#include <memory>

using namespace godot;

class WorkspaceManager;

class WorkspaceGroupViewGD : public RefCounted {
  GDCLASS(WorkspaceGroupViewGD, RefCounted)

private:
  static void _bind_methods();

  std::weak_ptr<WorkspaceManager> m_manager;
  WorkspaceGroup *m_group;
  int32_t m_group_id = -1;

public:
  WorkspaceGroupViewGD() = default;

  void _init_view(WorkspaceGroup *group, int32_t group_id);
  void create_workspace(String workspace_name);
  void destroy();

  int64_t get_id() const;
  int64_t get_capabilities() const;
};
