#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/core/memory.hpp"
#include "screencopy.h"
#include "wayland.h"
#include "wayland/wayland_service.h"
#include "workspace.h"
#include "workspace/workspace_gd.h"
#include "workspace/workspace_group_view_gd.h"
#include "workspace/workspace_view_gd.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>

void initialize_wayland_module(godot::ModuleInitializationLevel p_level) {
  if (p_level != godot::MODULE_INITIALIZATION_LEVEL_SCENE)
    return;

  ClassDB::register_class<Screencopy>();
  ClassDB::register_class<Workspace>();
  ClassDB::register_class<WorkspaceGD>();
  ClassDB::register_class<WorkspaceViewGD>();
  ClassDB::register_class<WorkspaceGroupViewGD>();
  ClassDB::register_class<WaylandService>();

  Engine::get_singleton()->register_singleton("Workspace", memnew(Workspace));
  Engine::get_singleton()->register_singleton("WorkspaceGD_single", memnew(WorkspaceGD));
}

void uninitialize_wayland_module(godot::ModuleInitializationLevel p_level) {
  if (p_level != godot::MODULE_INITIALIZATION_LEVEL_SCENE)
    return;

  Engine::get_singleton()->unregister_singleton("Workspace");
}

extern "C" {
GDExtensionBool GDE_EXPORT gdextension_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                                            GDExtensionClassLibraryPtr p_library,
                                            GDExtensionInitialization *r_initialization) {
  godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

  init_obj.register_initializer(initialize_wayland_module);
  init_obj.register_terminator(uninitialize_wayland_module);
  init_obj.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_SCENE);

  return init_obj.init();
}
}
