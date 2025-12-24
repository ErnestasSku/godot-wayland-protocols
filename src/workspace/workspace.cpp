#include "workspace/workspace.h"
#include <cstring>

Workspace::Workspace(ext_workspace_handle_v1 *handle) : m_handle(handle) {
  wl_array_init(&m_coordinates);

  m_listener.id = handle_id;
  m_listener.name = handle_name;
  m_listener.coordinates = handle_cooridinates;
  m_listener.state = handle_state;
  m_listener.capabilities = handle_capabilities;
  m_listener.removed = handle_removed;

  ext_workspace_handle_v1_add_listener(handle, &m_listener, this);
}

Workspace::~Workspace() {
  wl_array_release(&m_coordinates);

  if (m_handle) {
    ext_workspace_handle_v1_destroy(m_handle);
    m_handle = nullptr;
  }
}

// Callbacks
void Workspace::handle_id(void *data, ext_workspace_handle_v1 *, const char *id) {
  auto *self = static_cast<Workspace *>(data);
  self->m_id = id;
}

void Workspace::handle_name(void *data, ext_workspace_handle_v1 *, const char *name) {
  auto *self = static_cast<Workspace *>(data);
  self->m_name = name;
}

void Workspace::handle_cooridinates(void *data, ext_workspace_handle_v1 *, wl_array *coordinates) {
  auto *self = static_cast<Workspace *>(data);

  wl_array_release(&self->m_coordinates);
  wl_array_init(&self->m_coordinates);

  if (!coordinates || coordinates->size == 0) {
    return;
  }

  void *dst = wl_array_add(&self->m_coordinates, coordinates->size);
  if (!dst) {
    return;
  }

  std::memcpy(dst, coordinates->data, coordinates->size);
}

void Workspace::handle_state(void *data, ext_workspace_handle_v1 *, uint32_t state) {
  auto *self = static_cast<Workspace *>(data);
  self->m_state = state;
}

void Workspace::handle_capabilities(void *data, ext_workspace_handle_v1 *, uint32_t caps) {
  auto *self = static_cast<Workspace *>(data);
  self->m_capabilities = caps;
}

void Workspace::handle_removed(void *data, ext_workspace_handle_v1 *) {
  // optional cleanup
}
