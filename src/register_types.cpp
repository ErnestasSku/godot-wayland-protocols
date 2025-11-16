#include "screencopy_node.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>

void initialize_screencopy_module(godot::ModuleInitializationLevel p_level) {
  if (p_level != godot::MODULE_INITIALIZATION_LEVEL_SCENE)
    return;
  ClassDB::register_class<ScreencopyNode>();
}

void uninitialize_screencopy_module(godot::ModuleInitializationLevel p_level) {
  if (p_level != godot::MODULE_INITIALIZATION_LEVEL_SCENE)
    return;
}

// extern "C" {
// GDExtensionBool GDE_EXPORT gdextension_screencopy_library_init(
//     GDExtensionInterfaceGetProcAddress p_get_proc_address,
//     GDExtensionClassLibraryPtr p_library,
//     GDExtensionInitialization *r_initialization) {
//   godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address,
//   p_library,
//                                                  r_initialization);
//   init_obj.register_initializer(initialize_screencopy_module);
//   init_obj.register_terminator(uninitialize_screencopy_module);
//   init_obj.set_minimum_library_initialization_level(
//       godot::MODULE_INITIALIZATION_LEVEL_SCENE);
//   return init_obj.init();
// }
// }

extern "C" {
GDExtensionBool GDE_EXPORT
gdextension_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                 GDExtensionClassLibraryPtr p_library,
                 GDExtensionInitialization *r_initialization) {
  godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library,
                                                 r_initialization);

  init_obj.register_initializer(initialize_screencopy_module);
  init_obj.register_terminator(uninitialize_screencopy_module);
  init_obj.set_minimum_library_initialization_level(
      godot::MODULE_INITIALIZATION_LEVEL_SCENE);

  return init_obj.init();
}
}
