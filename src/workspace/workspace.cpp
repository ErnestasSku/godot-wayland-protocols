#include "workspace/workspace.h"
#include "workspace/workspace_manager.h"
#include <cstring>

Workspace::Workspace(ext_workspace_handle_v1 *handle, WorkspaceManager *manager, uint64_t runtime_id)
    : m_handle(handle), m_manager(manager), m_runtime_id(runtime_id) {
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

void Workspace::on_id_changed(IdChangedCallback cb) { m_id_changed_cb = std::move(cb); }
void Workspace::on_name_changed(NameChangedCallback cb) { m_name_changed_cb = std::move(cb); }
void Workspace::on_coordinates_changed(CoordinatesChangedCallback cb) { m_coordinates_changed_cb = std::move(cb); }
void Workspace::on_capabilities_changed(CapabilitiesChangedCallback cb) { m_capabilities_changed_cb = std::move(cb); }
void Workspace::on_state_changed(StateChangedCallback cb) { m_state_changed_cb = std::move(cb); }
void Workspace::on_removed(RemovedCallback cb) { m_removed_cb = std::move(cb); }

// Callbacks
void Workspace::handle_id(void *data, ext_workspace_handle_v1 *, const char *id) {
  auto *self = static_cast<Workspace *>(data);
  const bool was_empty = self->m_id.empty();
  self->m_id = id;

  if (was_empty && !self->m_id.empty() && self->m_id_available_cb) {
    self->m_id_available_cb(*self);
  }

  if (self->m_id_changed_cb) {
    self->m_id_changed_cb(id);
  }
}

void Workspace::handle_name(void *data, ext_workspace_handle_v1 *, const char *name) {
  auto *self = static_cast<Workspace *>(data);
  self->m_name = name;

  if (self->m_name_changed_cb) {
    self->m_name_changed_cb(name);
  }
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

  if (self->m_coordinates_changed_cb) {
    self->m_coordinates_changed_cb(coordinates);
  }
}

void Workspace::handle_state(void *data, ext_workspace_handle_v1 *, uint32_t state) {
  auto *self = static_cast<Workspace *>(data);
  self->m_state = state;

  if (self->m_state_changed_cb) {
    self->m_state_changed_cb(state);
  }
}

void Workspace::handle_capabilities(void *data, ext_workspace_handle_v1 *, uint32_t caps) {
  auto *self = static_cast<Workspace *>(data);
  self->m_capabilities = caps;

  if (self->m_capabilities_changed_cb) {
    self->m_capabilities_changed_cb(caps);
  }
}

void Workspace::handle_removed(void *data, ext_workspace_handle_v1 *) {
  auto *self = static_cast<Workspace *>(data);

  if (self->m_removed_cb) {
    self->m_removed_cb();
  }

  if (self->m_removed_manager_cb) {
    self->m_removed_manager_cb(*self);
  }
}
