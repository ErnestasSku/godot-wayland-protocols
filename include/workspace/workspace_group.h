#pragma once

#include "ext-workspace-v1.h"
#include <cstdint>
#include <vector>
#include <wayland-client-protocol.h>

class Workspace;

class WorkspaceGroup {
public:
  explicit WorkspaceGroup(ext_workspace_group_handle_v1 *handle);
  ~WorkspaceGroup();

  ext_workspace_group_handle_v1 *handle() const { return m_handle; }

  uint32_t capabilities() const { return m_capabilities; }
  const std::vector<Workspace *> &workspaces() const;
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

  uint32_t m_capabilities = 0;
  std::vector<Workspace *> m_workspaces;
  std::vector<wl_output *> m_outputs;
};
