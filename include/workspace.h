#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "ext-workspace-v1.h"
#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "wayland.h"

using namespace godot;

class Workspace : public RefCounted {
  GDCLASS(Workspace, RefCounted)

private:
  // ext_workspace_manager
  static void workspace_group(void *, ext_workspace_manager_v1 *,
                              ext_workspace_group_handle_v1 *);
  static void workspace(void *, ext_workspace_manager_v1 *,
                        ext_workspace_handle_v1 *);
  static void workspace_done(void *, ext_workspace_manager_v1 *);
  static void workspace_finished(void *, ext_workspace_manager_v1 *);

  // ext_workspace_group_hanlder

  // ext_workspace_handle

protected:
  static void _bind_methods();

public:
  Workspace();
  void register_to_workspace_events();
  void unregister_from_workspace_events();
  void hello_workspace();
  void test_increment();
};

#endif
