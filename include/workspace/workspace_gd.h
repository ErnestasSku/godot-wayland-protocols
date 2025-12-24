#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "workspace/workspace_manager.h"
#include <godot_cpp/variant/array.hpp>
#include <memory>

using namespace godot;

class WorkspaceGD : public ::RefCounted {
  GDCLASS(WorkspaceGD, RefCounted)

private:
  static void _bind_methods();

  std::shared_ptr<WorkspaceManager> manager;

public:
  WorkspaceGD();
  ~WorkspaceGD();

  void init();

  Array get_workspaces() const;
  Array get_groups() const;

};
