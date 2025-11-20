#include "workspace.h"
#include "ext-workspace-v1.h"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "wayland.h"
#include <wayland-client-core.h>

using namespace godot;

Workspace::Workspace() {
  auto wayland = Wayland::get_singleton();

  if (wayland->workspace_manager == nullptr) {
    UtilityFunctions::push_warning("Could not bind workspace protocol");
  }

  // const ext_workspace_manager_v1_listener workspace_manager_listener = {
  //     .workspace_group = workspace_group,
  //     .workspace = workspace,
  //     .done = workspace_done,
  //     .finished = workspace_finished};

  // ext_workspace_manager_v1_add_listener(wayland->workspace_manager,
  //                                       &workspace_manager_listener, this);
  // wl_display_roundtrip(wayland->display);
}

void Workspace::register_to_workspace_events() {
  auto wayland = Wayland::get_singleton();

  const ext_workspace_manager_v1_listener workspace_manager_listener = {
      .workspace_group = workspace_group,
      .workspace = workspace,
      .done = workspace_done,
      .finished = workspace_finished};

  ext_workspace_manager_v1_add_listener(wayland->workspace_manager,
                                        &workspace_manager_listener, this);

  ext_workspace_manager_v1_commit(wayland->workspace_manager);

  wl_display_roundtrip(wayland->display);
}

void Workspace::_bind_methods() {
  ClassDB::bind_method(D_METHOD("register_to_workspace_events"),
                       &Workspace::register_to_workspace_events);

  ADD_SIGNAL(MethodInfo("workspace_created"));
}

void Workspace::workspace_group(void *data, ext_workspace_manager_v1 *manager,
                                ext_workspace_group_handle_v1 *group_handle) {

  // Workspace group in Niri relates to workspaces belonging to specific monitor
  UtilityFunctions::print("Workspace group has been created");
};

void Workspace::workspace(void *data, ext_workspace_manager_v1 *manager,
                          ext_workspace_handle_v1 *workspace_handle) {

  auto *self = static_cast<Workspace *>(data);
  UtilityFunctions::print("New workspace created");
  // TODO: send actual data
  // auto workspace_data =
  // ext_workspace_handle_v1_get_user_data(workspace_handle);
  // wl_display_roundtrip(struct wl_display *display)

  UtilityFunctions::print("Emit signal");
  self->emit_signal("workspace_created");
}

void Workspace::workspace_done(void *data, ext_workspace_manager_v1 *manager) {
  UtilityFunctions::print("Workspace done");
}
void Workspace::workspace_finished(void *data,
                                   ext_workspace_manager_v1 *manager) {
  UtilityFunctions::print("Workspace finished");
}
