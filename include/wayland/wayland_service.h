#pragma once

#include "godot_cpp/classes/node.hpp"
using namespace godot;

class WaylandService : public ::Node {
  GDCLASS(WaylandService, Node)

protected:
  static void _bind_methods();

public:
  void _process(double delta);
};
