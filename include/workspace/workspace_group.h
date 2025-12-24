#pragma once

#include "ext-workspace-v1.h"
#include <cstdint>
#include <vector>
#include <wayland-client-protocol.h>

class WorkspaceManager;

class WorkspaceGroup {
public:
  explicit WorkspaceGroup(ext_workspace_group_handle_v1 *handle, WorkspaceManager *manager, int32_t group_id);
  ~WorkspaceGroup();

  ext_workspace_group_handle_v1 *handle() const { return m_handle; }
  int32_t id() const { return m_group_id; }

  uint32_t capabilities() const { return m_capabilities; }
  const std::vector<ext_workspace_handle_v1 *> &workspaces() const;
  const std::vector<wl_output *> &outputs() const;

private:
  static void handle_capabilities(void *data, ext_workspace_group_handle_v1 *, uint32_t capabilities);
  static void handle_output_enter(void *data, ext_workspace_group_handle_v1 *, wl_output *output);
  static void handle_output_leave(void *data, ext_workspace_group_handle_v1 *, wl_output *output);
  static void handle_workspace_enter(void *data, ext_workspace_group_handle_v1 *, ext_workspace_handle_v1 *workspace);
  static void handle_workspace_leave(void *data, ext_workspace_group_handle_v1 *, ext_workspace_handle_v1 *workspace);
  static void handle_removed(void *data, ext_workspace_group_handle_v1 *);

  ext_workspace_group_handle_v1 *m_handle = nullptr;
  ext_workspace_group_handle_v1_listener m_listener = {};

  WorkspaceManager *m_manager = nullptr;
  int32_t m_group_id = -1;

  uint32_t m_capabilities = 0;
  std::vector<ext_workspace_handle_v1 *> m_workspaces;
  std::vector<wl_output *> m_outputs;
};
