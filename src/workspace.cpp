#include "workspace.h"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "wayland.h"

using namespace godot;

void Workspace::_bind_methods() {
  ClassDB::bind_method(D_METHOD("hello_workspace"), &Workspace::hello_workspace);
  ClassDB::bind_method(D_METHOD("test_increment"), &Workspace::test_increment);

}

void Workspace::hello_workspace() {
  auto wayland = Wayland::get_singleton();

  if (!wayland) {
    UtilityFunctions::printerr("No wayland singleton :(");
    return;
  }

  UtilityFunctions::print("We try to get singleton");
  wayland->test = "Set a value for a thing";
}

void Workspace::test_increment() {
  auto wayland = Wayland::get_singleton();

  if (!wayland) {
    UtilityFunctions::printerr("No wayland singleton :(");
    return;
  }

  wayland->test_counter += 1;
  UtilityFunctions::print("Test counter: ", wayland->test_counter);
}
