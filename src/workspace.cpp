#include "workspace.h"
#include "ext-workspace-v1.h"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "wayland.h"
#include <algorithm>
#include <cstdint>
#include <memory>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>

using namespace godot;

Workspace::Workspace() {
  auto wayland = Wayland::get_singleton();

  if (wayland->workspace_manager == nullptr) {
    UtilityFunctions::push_warning("Could not bind workspace protocol");
  }
}

void Workspace::_bind_methods() {
  ClassDB::bind_method(D_METHOD("register_to_workspace_events"),
                       &Workspace::register_to_workspace_events);

  ADD_SIGNAL(MethodInfo("workspace_created"));
}

void Workspace::workspace_group(void *data, ext_workspace_manager_v1 *manager,
                                ext_workspace_group_handle_v1 *group_handle) {

  auto self = static_cast<Workspace *>(data);

  auto workspace_group =
      std::make_unique<Workspace::WorkspaceGroupWrapper>(group_handle);

  self->groups.push_back(std::move(workspace_group));

  ext_workspace_group_handle_v1_listener group_listener = {
      .capabilities = workspace_group_capabilities,
      .output_enter = workspace_group_output_enter,
      .output_leave = workspace_group_output_leave,
      .workspace_enter = workspace_group_workspace_enter,
      .workspace_leave = workspace_group_workspace_leave,
      .removed = workspace_group_remove};

  ext_workspace_group_handle_v1_add_listener(group_handle, &group_listener,
                                             data);
  wl_display_roundtrip(Wayland::get_singleton()->display);
};

void Workspace::workspace(void *data, ext_workspace_manager_v1 *manager,
                          ext_workspace_handle_v1 *workspace_handle) {

  auto *self = static_cast<Workspace *>(data);
  auto workspace = std::make_unique<WorkspaceWrapper>(workspace_handle);
  self->workspaces.push_back(std::move(workspace));

  ext_workspace_handle_v1_listener workspace_listener{
      .id = workspace_id,
      .name = workspace_name,
      .coordinates = workspace_coordinates,
      .state = workspace_state,
      .capabilities = workspace_capabilities,
      .removed = workspace_removed};

  ext_workspace_handle_v1_add_listener(workspace_handle, &workspace_listener,
                                       data);
  wl_display_roundtrip(Wayland::get_singleton()->display);
}

void Workspace::workspace_done(void *data, ext_workspace_manager_v1 *manager) {
  UtilityFunctions::print("Workspace done");
  // This means the manager sent all of the relevant data. Now we process it?
  // Build out structures properly.

  auto self = static_cast<Workspace *>(data);

  UtilityFunctions::print("We have [groups]: ", self->groups.size());
  UtilityFunctions::print("We have [workspaces]: ",
                          (*self->groups[0]).workspaces.size());
  UtilityFunctions::print("We have [wl_outputs]",
                          (*self->groups[0]).outputs.size());
  UtilityFunctions::print("We have [capabilities]",
                          (*self->groups[0]).capabilities);
}

void Workspace::workspace_finished(void *data,
                                   ext_workspace_manager_v1 *manager) {
  // TODO: This meanes no more events will be sent
  // some flag should be set, which would now allow to call any methods from
  // godot since after finished, it would probably crash, as the managed is
  // destroyed
  UtilityFunctions::print("Workspace finished");
}

// Workspace group event listeners
void Workspace::workspace_group_capabilities(
    void *data, ext_workspace_group_handle_v1 *group_handle,
    uint32_t capabilities) {

  auto self = static_cast<Workspace *>(data);
  // TODO: This will be repeated multiple times. So move it to a class method.
  auto workspace_group = self->find_workspace_group_wrapper(group_handle);
  if (workspace_group == nullptr) {
    return;
  }

  (*workspace_group).capabilities =
      static_cast<ext_workspace_group_handle_v1_group_capabilities>(
          capabilities);
}

void Workspace::workspace_group_output_enter(
    void *data, struct ext_workspace_group_handle_v1 *group_handle,
    struct wl_output *output) {

  auto self = static_cast<Workspace *>(data);
  auto workspace_group = self->find_workspace_group_wrapper(group_handle);

  if (workspace_group == nullptr) {
    return;
  }

  (*workspace_group).outputs.push_back(output);
}

void Workspace::workspace_group_output_leave(
    void *data, struct ext_workspace_group_handle_v1 *group_handle,
    struct wl_output *output) {
  auto self = static_cast<Workspace *>(data);
  auto workspace_group = self->find_workspace_group_wrapper(group_handle);

  if (workspace_group == nullptr) {
    return;
  }

  (*workspace_group)
      .outputs.erase(std::find_if(
          (*workspace_group).outputs.begin(), (*workspace_group).outputs.end(),
          [output](const wl_output *elem) { return elem == output; }));
}

void Workspace::workspace_group_workspace_enter(
    void *data, struct ext_workspace_group_handle_v1 *group_handle,
    struct ext_workspace_handle_v1 *workspace) {

  auto self = static_cast<Workspace *>(data);
  auto workspace_group = self->find_workspace_group_wrapper(group_handle);
  if (workspace_group == nullptr) {
    return;
  }

  (*workspace_group).workspaces.push_back(workspace);
}

void Workspace::workspace_group_workspace_leave(
    void *data, struct ext_workspace_group_handle_v1 *group_handle,
    struct ext_workspace_handle_v1 *workspace) {

  auto self = static_cast<Workspace *>(data);
  auto workspace_group = self->find_workspace_group_wrapper(group_handle);
  if (workspace_group == nullptr) {
    return;
  }

  (*workspace_group)
      .workspaces.erase(
          std::find_if((*workspace_group).workspaces.begin(),
                       (*workspace_group).workspaces.end(),
                       [workspace](const ext_workspace_handle_v1 *elem) {
                         return elem == workspace;
                       }));
}

void Workspace::workspace_group_remove(
    void *data, struct ext_workspace_group_handle_v1 *group_handle) {
  auto self = static_cast<Workspace *>(data);
  auto workspace_group = std::find_if(
      self->groups.begin(), self->groups.end(),
      [group_handle](const std::unique_ptr<WorkspaceGroupWrapper> &elem) {
        return elem->handle == group_handle;
      });

  if (workspace_group == self->groups.end()) {
    return;
  }

  self->groups.erase(workspace_group);
}

// Workspace evebt listeners
void Workspace::workspace_id(
    void *data, struct ext_workspace_handle_v1 *ext_workspace_handle_v1,
    const char *id) {
  auto self = static_cast<Workspace *>(data);
  auto workspace = self->find_workspace_wrapper(ext_workspace_handle_v1);

  self->find_workspace_wrapper(ext_workspace_handle_v1);

  if (workspace != nullptr) {
    (*workspace).id = id;
  }
}

void Workspace::workspace_name(
    void *data, struct ext_workspace_handle_v1 *ext_workspace_handle_v1,
    const char *name) {
  auto self = static_cast<Workspace *>(data);
  auto workspace = self->find_workspace_wrapper(ext_workspace_handle_v1);
  if (workspace != nullptr) {
    (*workspace).name = name;
  }
}

void Workspace::workspace_coordinates(
    void *data, struct ext_workspace_handle_v1 *ext_workspace_handle_v1,
    struct wl_array *coordinates) {
  UtilityFunctions::print("Coords: ", coordinates);
  auto self = static_cast<Workspace *>(data);
  auto workspace = self->find_workspace_wrapper(ext_workspace_handle_v1);

  if (workspace != nullptr) {
    (*workspace).coordinates = coordinates;
  }
}

void Workspace::workspace_state(
    void *data, struct ext_workspace_handle_v1 *ext_workspace_handle_v1,
    uint32_t state) {

  auto self = static_cast<Workspace *>(data);
  auto workspace = self->find_workspace_wrapper(ext_workspace_handle_v1);

  if (workspace != nullptr) {
    (*workspace).state = static_cast<ext_workspace_handle_v1_state>(state);
  }
}

void Workspace::workspace_capabilities(
    void *data, struct ext_workspace_handle_v1 *ext_workspace_handle_v1,
    uint32_t capabilities) {
  auto self = static_cast<Workspace *>(data);
  auto workspace = self->find_workspace_wrapper(ext_workspace_handle_v1);

  if (workspace != nullptr) {
    (*workspace).capabilities =
        static_cast<ext_workspace_handle_v1_workspace_capabilities>(
            capabilities);
  }
}
void Workspace::workspace_removed(
    void *data, struct ext_workspace_handle_v1 *ext_workspace_handle_v1) {

  auto self = static_cast<Workspace *>(data);

  self->workspaces.erase(
      std::find_if(self->workspaces.begin(), self->workspaces.end(),
                   [ext_workspace_handle_v1](auto &elem) {
                     return elem->handle == ext_workspace_handle_v1;
                   }));
}

// Utility methods

Workspace::WorkspaceGroupWrapper *
Workspace::find_workspace_group_wrapper(ext_workspace_group_handle_v1 *handle) {
  auto workspace_group = std::find_if(
      groups.begin(), groups.end(),
      [handle](const std::unique_ptr<WorkspaceGroupWrapper> &elem) {
        return elem->handle == handle;
      });

  if (workspace_group == groups.end()) {
    return nullptr;
  } else {
    return workspace_group->get();
  }
}

Workspace::WorkspaceWrapper *
Workspace::find_workspace_wrapper(ext_workspace_handle_v1 *handle) {
  auto workspace =
      std::find_if(workspaces.begin(), workspaces.end(),
                   [handle](auto &elem) { return elem->handle == handle; });
  if (workspace == workspaces.end()) {
    return nullptr;
  } else {
    return workspace->get();
  }
}

// Godot methods
void Workspace::register_to_workspace_events() {
  auto wayland = Wayland::get_singleton();

  const ext_workspace_manager_v1_listener workspace_manager_listener = {
      .workspace_group = workspace_group,
      .workspace = workspace,
      .done = workspace_done,
      .finished = workspace_finished};

  ext_workspace_manager_v1_add_listener(wayland->workspace_manager,
                                        &workspace_manager_listener, this);
  wl_display_roundtrip(wayland->display);
}
