#include "workspace/workspace_group.h"
#include "algorithm"
#include "ext-workspace-v1.h"

WorkspaceGroup::WorkspaceGroup(ext_workspace_group_handle_v1 *handle) : m_handle(handle) {
  m_listener.capabilities = handle_capabilities;
  m_listener.output_enter = handle_output_enter;
  m_listener.output_leave = handle_output_leave;
  m_listener.workspace_enter = handle_workspace_enter;
  m_listener.workspace_leave = handle_workspace_leave;
  m_listener.removed = handle_removed;

  ext_workspace_group_handle_v1_add_listener(handle, &m_listener, this);
}

WorkspaceGroup::~WorkspaceGroup() {
  if (m_handle) {
    ext_workspace_group_handle_v1_destroy(m_handle);
    m_handle = nullptr;
  }
}

const std::vector<Workspace *> &WorkspaceGroup::workspaces() const { return m_workspaces; }

const std::vector<wl_output *> &WorkspaceGroup::outputs() const { return m_outputs; }

// callbacks
void WorkspaceGroup::handle_capabilities(void *data, ext_workspace_group_handle_v1 *, uint32_t caps) {
  auto *self = static_cast<WorkspaceGroup *>(data);
  self->m_capabilities = caps;
}

void WorkspaceGroup::handle_output_enter(void *data, ext_workspace_group_handle_v1 *, wl_output *output) {
  auto *self = static_cast<WorkspaceGroup *>(data);
  self->m_outputs.push_back(output);
}

void WorkspaceGroup::handle_output_leave(void *data, ext_workspace_group_handle_v1 *, wl_output *output) {
  auto *self = static_cast<WorkspaceGroup *>(data);
  self->m_outputs.erase(std::remove(self->m_outputs.begin(), self->m_outputs.end(), output), self->m_outputs.end());
}

void WorkspaceGroup::handle_workspace_enter(void *data, ext_workspace_group_handle_v1 *,
                                            ext_workspace_handle_v1 *workspace) {
  auto *self = static_cast<WorkspaceGroup *>(data);
  self->m_workspaces.push_back(reinterpret_cast<Workspace *>(workspace));
}

void WorkspaceGroup::handle_workspace_leave(void *data, ext_workspace_group_handle_v1 *,
                                            ext_workspace_handle_v1 *workspace) {
  auto *self = static_cast<WorkspaceGroup *>(data);
  self->m_workspaces.erase(
      std::remove(self->m_workspaces.begin(), self->m_workspaces.end(), reinterpret_cast<Workspace *>(workspace)),
      self->m_workspaces.end());
}

void WorkspaceGroup::handle_removed(void *data, ext_workspace_group_handle_v1 *) {
  // optional cleanup
}
