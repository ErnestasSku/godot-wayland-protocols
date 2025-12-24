#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "workspace/workspace_group.h"
#include "workspace/workspace_manager.h"
#include <memory>

using namespace godot;

class WorkspaceGD : public ::RefCounted {
  GDCLASS(WorkspaceGD, RefCounted)

private:
  static void _bind_methods();

  std::unique_ptr<WorkspaceManager> manager;

public:
  WorkspaceGD();
  ~WorkspaceGD();

  void init();

  // GDScript signals
  void workspace_created(Workspace &workspace);
  void group_crated(WorkspaceGroup &group);
};
