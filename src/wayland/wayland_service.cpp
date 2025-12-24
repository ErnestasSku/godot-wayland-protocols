#include "wayland/wayland_service.h"
#include "godot_cpp/variant/utility_functions.hpp"
#include "wayland/wayland_connection.h"

void WaylandService::_bind_methods() {}

void WaylandService::_process(double) { WaylandConnection::instance().pump_events(); }
