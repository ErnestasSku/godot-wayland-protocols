#include "workspace/workspace_gd.h"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/object.hpp"
#include "workspace/workspace.h"
#include "workspace/workspace_group.h"
#include "workspace/workspace_manager.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <memory>

using namespace ::godot;

WorkspaceGD::WorkspaceGD() = default;

WorkspaceGD::~WorkspaceGD() = default;

void WorkspaceGD::_bind_methods() {
  ADD_SIGNAL(MethodInfo("workspace_created", PropertyInfo(Variant::STRING, "id")));
  ADD_SIGNAL(MethodInfo("group_created", PropertyInfo(Variant::INT, "capabilities")));

  ClassDB::bind_method(D_METHOD("init"), &WorkspaceGD::init);
}

void WorkspaceGD::init() {
  manager = std::make_unique<WorkspaceManager>();

  manager->on_workspace_created([this](Workspace &w) { call_deferred("workspace_created", w.id().c_str()); });

  manager->on_group_created([this](WorkspaceGroup &g) { call_deferred("group_created", int(g.capabilities())); });
}
