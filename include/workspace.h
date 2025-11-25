#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "ext-workspace-v1.h"
#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "wayland.h"
#include <memory>
#include <vector>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>

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
  static void workspace_group_capabilities(void *,
                                           ext_workspace_group_handle_v1 *,
                                           uint32_t);
  static void
  workspace_group_output_enter(void *, struct ext_workspace_group_handle_v1 *,
                               struct wl_output *);

  static void
  workspace_group_output_leave(void *, struct ext_workspace_group_handle_v1 *,
                               struct wl_output *);
  static void
  workspace_group_workspace_enter(void *,
                                  struct ext_workspace_group_handle_v1 *,
                                  struct ext_workspace_handle_v1 *);

  static void
  workspace_group_workspace_leave(void *data,
                                  struct ext_workspace_group_handle_v1 *,
                                  struct ext_workspace_handle_v1 *);
  static void workspace_group_remove(void *data,
                                     struct ext_workspace_group_handle_v1 *);

  // ext_workspace_handle
  static void workspace_id(void *, struct ext_workspace_handle_v1 *,
                           const char *);

  static void workspace_name(void *, struct ext_workspace_handle_v1 *,
                             const char *);

  static void workspace_coordinates(void *, struct ext_workspace_handle_v1 *,
                                    struct wl_array *);

  static void workspace_state(void *data, struct ext_workspace_handle_v1 *,
                              uint32_t state);

  static void workspace_capabilities(void *data,
                                     struct ext_workspace_handle_v1 *,
                                     uint32_t);
  static void workspace_removed(void *, struct ext_workspace_handle_v1 *);

  class WorkspaceGroupWrapper {
  public:
    WorkspaceGroupWrapper(ext_workspace_group_handle_v1 *handle) {
      this->handle = handle;
    }

    ~WorkspaceGroupWrapper() {
      if (handle) {
        UtilityFunctions::print("DEBUG: workspace group destructor was called");
        auto wayland = Wayland::get_singleton();
        ext_workspace_group_handle_v1_destroy(handle);
        wl_display_roundtrip(wayland->display);
      }
    }

    ext_workspace_group_handle_v1 *handle;
    ext_workspace_group_handle_v1_group_capabilities capabilities;
    std::vector<ext_workspace_handle_v1 *> workspaces;
    std::vector<wl_output *> outputs;
  };

  class WorkspaceWrapper {
  public:
    WorkspaceWrapper(ext_workspace_handle_v1 *handle) { this->handle = handle; }
    ~WorkspaceWrapper() {
      if (handle) {
        UtilityFunctions::print("DEBUG: workspace group destructor was called");
        ext_workspace_handle_v1_destroy(handle);
        wl_display_roundtrip(Wayland::get_singleton()->display);
      }
    }

    ext_workspace_handle_v1 *handle;
    std::string id;
    std::string name;
    wl_array *coordinates;
    ext_workspace_handle_v1_state state;
    ext_workspace_handle_v1_workspace_capabilities capabilities;
  };

  std::vector<std::unique_ptr<WorkspaceGroupWrapper>> groups;
  std::vector<std::unique_ptr<WorkspaceWrapper>> workspaces;

  WorkspaceGroupWrapper* find_workspace_group_wrapper(ext_workspace_group_handle_v1*);
  WorkspaceWrapper* find_workspace_wrapper(ext_workspace_handle_v1*);
  
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
