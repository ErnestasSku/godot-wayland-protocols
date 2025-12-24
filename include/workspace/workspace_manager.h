#pragma once

#include "ext-workspace-v1.h"
#include "wayland/wayland_connection.h"
#include <functional>
#include <memory>
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

  const std::vector<std::unique_ptr<WorkspaceGroup>> &groups() const;
  const std::vector<std::unique_ptr<Workspace>> &workspaces() const;

private:
  static void handle_workspace_group(void *data, ext_workspace_manager_v1 *manager,
                                     ext_workspace_group_handle_v1 *group);
  static void handle_workspace(void *data, ext_workspace_manager_v1 *manager, ext_workspace_handle_v1 *workspace);
  static void handle_done(void *data, ext_workspace_manager_v1 *manager);
  static void handle_finished(void *data, ext_workspace_manager_v1 *manager);

  // WaylandConnection &m_connection;
  ext_workspace_manager_v1 *m_manager = nullptr;

  ext_workspace_manager_v1_listener m_listener = {};

  std::vector<std::unique_ptr<WorkspaceGroup>> m_groups;
  std::vector<std::unique_ptr<Workspace>> m_workspaces;

  WorkspaceCreatedCallback m_workspace_created_cb;
  GroupCreatedCallback m_group_created_cb;
};
