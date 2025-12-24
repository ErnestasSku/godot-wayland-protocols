#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/variant/packed_int32_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include <memory>
#include <string>

class WorkspaceManager;

using namespace godot;

class WorkspaceViewGD : public RefCounted {
  GDCLASS(WorkspaceViewGD, RefCounted)

private:
  static void _bind_methods();

  std::weak_ptr<WorkspaceManager> m_manager;
  std::string m_id;

public:
  WorkspaceViewGD() = default;

  void _init_view(std::shared_ptr<WorkspaceManager> manager, std::string id);

  String get_id() const;
  String get_name() const;
  int64_t get_state() const;
  int64_t get_capabilities() const;
  int64_t get_group_id() const;
  PackedInt32Array get_coordinates() const;
};
