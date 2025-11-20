#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "wayland.h"

using namespace godot;

class Workspace: public RefCounted {
  GDCLASS(Workspace, RefCounted)

private:

  // ext_workspace_manager
  static void workspace_group();
  static void workspace();
  static void workspace_done();
  static void workspace_finished();

  // ext_workspace_group_hanlder

  // ext_workspace_handle
  
protected:
    static void _bind_methods();
  
public:
  void hello_workspace();
  void test_increment();
};


#endif
