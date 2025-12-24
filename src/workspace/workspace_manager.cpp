#include "workspace/workspace_manager.h"
#include "godot_cpp/variant/utility_functions.hpp"
#include "wayland/wayland_connection.h"
#include "workspace/workspace.h"
#include "workspace/workspace_group.h"
#include <algorithm>

WorkspaceManager::WorkspaceManager() {
  auto &connection = WaylandConnection::instance();

  if (!connection.is_valid()) {
    return;
  }

  connection.register_global(ext_workspace_manager_v1_interface.name,
                             [this](wl_registry *registry, uint32_t id, uint32_t version) {
                               const uint32_t bind_version =
                                   std::min<uint32_t>(version, static_cast<uint32_t>(ext_workspace_manager_v1_interface.version));
                               m_manager = static_cast<ext_workspace_manager_v1 *>(
                                   wl_registry_bind(registry, id, &ext_workspace_manager_v1_interface, bind_version));

                               m_listener.workspace_group = handle_workspace_group;
                               m_listener.workspace = handle_workspace;
                               m_listener.done = handle_done;
                               m_listener.finished = handle_finished;

                               ext_workspace_manager_v1_add_listener(m_manager, &m_listener, this);
                             });
}

WorkspaceManager::~WorkspaceManager() {
  m_workspaces.clear();
  m_groups.clear();

  if (m_manager) {
    ext_workspace_manager_v1_destroy(m_manager);
    m_manager = nullptr;
  }
}

bool WorkspaceManager::is_available() const { return m_manager != nullptr; }

void WorkspaceManager::on_workspace_created(WorkspaceCreatedCallback cb) { m_workspace_created_cb = std::move(cb); }

void WorkspaceManager::on_group_created(GroupCreatedCallback cb) { m_group_created_cb = std::move(cb); }

const std::vector<std::unique_ptr<WorkspaceGroup>> &WorkspaceManager::groups() const { return m_groups; }

const std::vector<std::unique_ptr<Workspace>> &WorkspaceManager::workspaces() const { return m_workspaces; }

void WorkspaceManager::handle_workspace_group(void *data, ext_workspace_manager_v1 *,
                                              ext_workspace_group_handle_v1 *handle) {
  auto *self = static_cast<WorkspaceManager *>(data);

  auto group = std::make_unique<WorkspaceGroup>(handle);
  WorkspaceGroup &ref = *group;

  self->m_groups.push_back(std::move(group));

  if (self->m_group_created_cb) {
    self->m_group_created_cb(ref);
  }
}

void WorkspaceManager::handle_workspace(void *data, ext_workspace_manager_v1 *, ext_workspace_handle_v1 *handle) {
  auto *self = static_cast<WorkspaceManager *>(data);

  godot::UtilityFunctions::print("WORKSPACE CRATED");

  auto workspace = std::make_unique<Workspace>(handle);
  Workspace &ref = *workspace;

  self->m_workspaces.push_back(std::move(workspace));

  if (self->m_workspace_created_cb) {
    self->m_workspace_created_cb(ref);
  }
}

void WorkspaceManager::handle_done(void *, ext_workspace_manager_v1 *) {
  // Snapshot complete
  godot::UtilityFunctions::print("handle done");
}

void WorkspaceManager::handle_finished(void *, ext_workspace_manager_v1 *) {
  // No more events
  godot::UtilityFunctions::print("handle finished");
}
