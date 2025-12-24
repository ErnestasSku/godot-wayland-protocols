#include "workspace/workspace_gd.h"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/object.hpp"
#include "workspace/workspace.h"
#include "workspace/workspace_group.h"
#include "workspace/workspace_group_view_gd.h"
#include "workspace/workspace_manager.h"
#include "workspace/workspace_view_gd.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <memory>

using namespace ::godot;

WorkspaceGD::WorkspaceGD() = default;

WorkspaceGD::~WorkspaceGD() = default;

void WorkspaceGD::_bind_methods() {
  ADD_SIGNAL(MethodInfo("workspace_added",
                        PropertyInfo(Variant::OBJECT, "workspace", PROPERTY_HINT_RESOURCE_TYPE, "WorkspaceViewGD")));
  ADD_SIGNAL(MethodInfo("group_added",
                        PropertyInfo(Variant::OBJECT, "group", PROPERTY_HINT_RESOURCE_TYPE, "WorkspaceGroupViewGD")));

  ClassDB::bind_method(D_METHOD("init"), &WorkspaceGD::init);
  ClassDB::bind_method(D_METHOD("get_workspaces"), &WorkspaceGD::get_workspaces);
  ClassDB::bind_method(D_METHOD("get_groups"), &WorkspaceGD::get_groups);
}

void WorkspaceGD::init() {
  manager = std::make_shared<WorkspaceManager>();

  manager->on_workspace_created([this](Workspace &w) {
    Ref<WorkspaceViewGD> view;
    view.instantiate();
    view->_init_view(manager, w.runtime_id());
    call_deferred("emit_signal", "workspace_added", view);
  });

  manager->on_group_created([this](WorkspaceGroup &g) {
    Ref<WorkspaceGroupViewGD> view;
    view.instantiate();
    view->_init_view(manager, g.id());
    call_deferred("emit_signal", "group_added", view);
  });
}

Array WorkspaceGD::get_workspaces() const {
  Array arr;
  if (!manager) {
    return arr;
  }

  for (const auto &w : manager->workspaces()) {
    if (!w) {
      continue;
    }

    Ref<WorkspaceViewGD> view;
    view.instantiate();
    view->_init_view(manager, w->runtime_id());
    arr.append(view);
  }

  return arr;
}

Array WorkspaceGD::get_groups() const {
  Array arr;
  if (!manager) {
    return arr;
  }

  for (const auto &g : manager->groups()) {
    if (!g) {
      continue;
    }

    Ref<WorkspaceGroupViewGD> view;
    view.instantiate();
    view->_init_view(manager, g->id());
    arr.append(view);
  }

  return arr;
}
