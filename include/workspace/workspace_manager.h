#pragma once

#include "ext-workspace-v1.h"
#include "wayland/wayland_connection.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class WorkspaceGroup;
class Workspace;

class WorkspaceManager {
public:
  using WorkspaceCreatedCallback = std::function<void(Workspace &)>;
  using GroupCreatedCallback = std::function<void(WorkspaceGroup &)>;

  explicit WorkspaceManager();
  ~WorkspaceManager();

  WorkspaceManager &operator=(const WorkspaceManager &) = delete;

  bool is_available() const;
  // const WaylandConnection &connection() const { return m_connection; }

  void on_workspace_created(WorkspaceCreatedCallback cb);
  void on_group_created(GroupCreatedCallback cb);

  const Workspace *get_workspace(const std::string &id) const;
  const Workspace *get_workspace(uint64_t runtime_id) const;
  const WorkspaceGroup *get_group(int32_t group_id) const;

  int32_t get_group_id_for_workspace(const std::string &workspace_id) const;
  int32_t get_group_id_for_workspace(uint64_t runtime_id) const;

  const std::vector<std::unique_ptr<WorkspaceGroup>> &groups() const;
  const std::vector<std::unique_ptr<Workspace>> &workspaces() const;

private:
  void set_workspace_handle_group(ext_workspace_handle_v1 *workspace_handle, int32_t group_id);
  void clear_workspace_handle_group(ext_workspace_handle_v1 *workspace_handle, int32_t group_id);
  void on_workspace_id_available(Workspace &workspace);
  void on_workspace_removed(Workspace &workspace);

  static void handle_workspace_group(void *data, ext_workspace_manager_v1 *manager,
                                     ext_workspace_group_handle_v1 *group);
  static void handle_workspace(void *data, ext_workspace_manager_v1 *manager, ext_workspace_handle_v1 *workspace);
  static void handle_done(void *data, ext_workspace_manager_v1 *manager);
  static void handle_finished(void *data, ext_workspace_manager_v1 *manager);

  // WaylandConnection &m_connection;
  ext_workspace_manager_v1 *m_manager = nullptr;

  ext_workspace_manager_v1_listener m_listener = {};

  int32_t m_next_group_id = 0;
  uint64_t m_next_runtime_workspace_id = 1;

  std::vector<std::unique_ptr<WorkspaceGroup>> m_groups;
  std::vector<std::unique_ptr<Workspace>> m_workspaces;

  std::unordered_map<int32_t, WorkspaceGroup *> m_groups_by_id;

  std::unordered_map<std::string, Workspace *> m_workspaces_by_id;
  std::unordered_map<uint64_t, Workspace *> m_workspaces_by_runtime_id;
  std::unordered_map<ext_workspace_handle_v1 *, Workspace *> m_workspaces_by_handle;

  std::unordered_map<ext_workspace_handle_v1 *, int32_t> m_group_by_workspace_handle;
  std::unordered_map<std::string, int32_t> m_group_by_workspace_id;
  std::unordered_map<uint64_t, int32_t> m_group_by_workspace_runtime_id;

  WorkspaceCreatedCallback m_workspace_created_cb;
  GroupCreatedCallback m_group_created_cb;

  friend class WorkspaceGroup;
};
