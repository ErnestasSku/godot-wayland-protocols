#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/variant/packed_int32_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include <cstdint>
#include <memory>
#include <workspace/workspace.h>

class WorkspaceManager;

using namespace godot;

class WorkspaceViewGD : public RefCounted {
  GDCLASS(WorkspaceViewGD, RefCounted)

private:
  static void _bind_methods();

  std::weak_ptr<WorkspaceManager> m_manager;
  Workspace *m_workspace;
  uint64_t m_runtime_id = 0;

public:
  WorkspaceViewGD() = default;

  void _init_view(std::shared_ptr<WorkspaceManager> manager, Workspace *workspace, uint64_t runtime_id);

  int64_t get_runtime_id() const;

  String get_id() const;
  String get_name() const;
  int64_t get_state() const;
  int64_t get_capabilities() const;
  int64_t get_group_id() const;
  PackedInt32Array get_coordinates() const;
};
