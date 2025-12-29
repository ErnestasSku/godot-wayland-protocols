#include "workspace/workspace_manager.h"
#include "ext-workspace-v1.h"
#include "wayland/wayland_connection.h"
#include "workspace/workspace.h"
#include "workspace/workspace_group.h"
#include <algorithm>

WorkspaceManager::WorkspaceManager() {
  auto &connection = WaylandConnection::instance();

  if (!connection.is_valid()) {
    return;
  }

  connection.register_global(
      ext_workspace_manager_v1_interface.name, [this](wl_registry *registry, uint32_t id, uint32_t version) {
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

  m_groups_by_id.clear();
  m_workspaces_by_id.clear();
  m_workspaces_by_runtime_id.clear();
  m_workspaces_by_handle.clear();
  m_group_by_workspace_handle.clear();
  m_group_by_workspace_id.clear();
  m_group_by_workspace_runtime_id.clear();

  if (m_manager) {
    ext_workspace_manager_v1_destroy(m_manager);
    m_manager = nullptr;
  }
}

bool WorkspaceManager::is_available() const { return m_manager != nullptr; }

void WorkspaceManager::on_workspace_created(WorkspaceCreatedCallback cb) { m_workspace_created_cb = std::move(cb); }

void WorkspaceManager::on_group_created(GroupCreatedCallback cb) { m_group_created_cb = std::move(cb); }

const Workspace *WorkspaceManager::get_workspace(const std::string &id) const {
  auto it = m_workspaces_by_id.find(id);
  if (it == m_workspaces_by_id.end()) {
    return nullptr;
  }
  return it->second;
}

const Workspace *WorkspaceManager::get_workspace(uint64_t runtime_id) const {
  auto it = m_workspaces_by_runtime_id.find(runtime_id);
  if (it == m_workspaces_by_runtime_id.end()) {
    return nullptr;
  }
  return it->second;
}

const WorkspaceGroup *WorkspaceManager::get_group(int32_t group_id) const {
  auto it = m_groups_by_id.find(group_id);
  if (it == m_groups_by_id.end()) {
    return nullptr;
  }
  return it->second;
}

int32_t WorkspaceManager::get_group_id_for_workspace(const std::string &workspace_id) const {
  auto it = m_group_by_workspace_id.find(workspace_id);
  if (it == m_group_by_workspace_id.end()) {
    return -1;
  }
  return it->second;
}

int32_t WorkspaceManager::get_group_id_for_workspace(uint64_t runtime_id) const {
  auto it = m_group_by_workspace_runtime_id.find(runtime_id);
  if (it == m_group_by_workspace_runtime_id.end()) {
    return -1;
  }
  return it->second;
}

void WorkspaceManager::set_workspace_handle_group(ext_workspace_handle_v1 *workspace_handle, int32_t group_id) {
  if (!workspace_handle) {
    return;
  }

  m_group_by_workspace_handle[workspace_handle] = group_id;

  auto ws_it = m_workspaces_by_handle.find(workspace_handle);
  if (ws_it != m_workspaces_by_handle.end()) {
    m_group_by_workspace_runtime_id[ws_it->second->runtime_id()] = group_id;

    const std::string &id = ws_it->second->id();
    if (!id.empty()) {
      m_group_by_workspace_id[id] = group_id;
    }
  }
}

void WorkspaceManager::clear_workspace_handle_group(ext_workspace_handle_v1 *workspace_handle, int32_t group_id) {
  if (!workspace_handle) {
    return;
  }

  auto it = m_group_by_workspace_handle.find(workspace_handle);
  if (it != m_group_by_workspace_handle.end() && it->second == group_id) {
    m_group_by_workspace_handle.erase(it);
  }

  auto ws_it = m_workspaces_by_handle.find(workspace_handle);
  if (ws_it != m_workspaces_by_handle.end()) {
    auto rid_it = m_group_by_workspace_runtime_id.find(ws_it->second->runtime_id());
    if (rid_it != m_group_by_workspace_runtime_id.end() && rid_it->second == group_id) {
      m_group_by_workspace_runtime_id.erase(rid_it);
    }

    const std::string &id = ws_it->second->id();
    auto gid_it = m_group_by_workspace_id.find(id);
    if (!id.empty() && gid_it != m_group_by_workspace_id.end() && gid_it->second == group_id) {
      m_group_by_workspace_id.erase(gid_it);
    }
  }
}

void WorkspaceManager::on_workspace_id_available(Workspace &workspace) {
  const std::string &id = workspace.id();
  if (id.empty()) {
    return;
  }

  m_workspaces_by_id[id] = &workspace;

  auto g_it = m_group_by_workspace_handle.find(workspace.handle());
  if (g_it != m_group_by_workspace_handle.end()) {
    m_group_by_workspace_id[id] = g_it->second;
  }
}

void WorkspaceManager::remove_workspace(Workspace &workspace) {
  const std::string &id = workspace.id();
  if (!id.empty()) {
    m_workspaces_by_id.erase(id);
    m_group_by_workspace_id.erase(id);
  }

  m_workspaces_by_runtime_id.erase(workspace.runtime_id());
  m_group_by_workspace_runtime_id.erase(workspace.runtime_id());

  if (workspace.handle()) {
    m_group_by_workspace_handle.erase(workspace.handle());
    m_workspaces_by_handle.erase(workspace.handle());
  }

  m_workspaces.erase(std::remove_if(m_workspaces.begin(), m_workspaces.end(),
                                    [&workspace](const auto &p) { return p.get() == &workspace; }),
                     m_workspaces.end());
}

const std::vector<std::unique_ptr<WorkspaceGroup>> &WorkspaceManager::groups() const { return m_groups; }

const std::vector<std::unique_ptr<Workspace>> &WorkspaceManager::workspaces() const { return m_workspaces; }

void WorkspaceManager::handle_workspace_group(void *data, ext_workspace_manager_v1 *,
                                              ext_workspace_group_handle_v1 *handle) {
  auto *self = static_cast<WorkspaceManager *>(data);

  const int32_t group_id = self->m_next_group_id++;
  auto group = std::make_unique<WorkspaceGroup>(handle, self, group_id);
  WorkspaceGroup &ref = *group;

  self->m_groups.push_back(std::move(group));
  self->m_groups_by_id[group_id] = &ref;

  if (self->m_group_created_cb) {
    self->m_group_created_cb(ref);
  }
}

void WorkspaceManager::handle_workspace(void *data, ext_workspace_manager_v1 *, ext_workspace_handle_v1 *handle) {
  auto *self = static_cast<WorkspaceManager *>(data);

  const uint64_t runtime_id = self->m_next_runtime_workspace_id++;
  auto workspace = std::make_unique<Workspace>(handle, self, runtime_id);
  Workspace &ref = *workspace;

  self->m_workspaces_by_handle[handle] = &ref;
  self->m_workspaces_by_runtime_id[runtime_id] = &ref;
  ref.set_id_available_callback([self](Workspace &w) { self->on_workspace_id_available(w); });
  ref.set_removed_callback([self](Workspace &w) { self->remove_workspace(w); });

  auto g_it = self->m_group_by_workspace_handle.find(handle);
  if (g_it != self->m_group_by_workspace_handle.end()) {
    self->m_group_by_workspace_runtime_id[runtime_id] = g_it->second;
  }

  self->m_workspaces.push_back(std::move(workspace));

  if (self->m_workspace_created_cb) {
    self->m_workspace_created_cb(ref);
  }
}

void WorkspaceManager::handle_done(void *, ext_workspace_manager_v1 *) {
  // Snapshot complete
}

void WorkspaceManager::handle_finished(void *, ext_workspace_manager_v1 *) {
  // No more events
}

void WorkspaceManager::commit() {
  if (!m_manager) {
    return;
  }

  ext_workspace_manager_v1_commit(m_manager);
  WaylandConnection::instance().flush();
}
