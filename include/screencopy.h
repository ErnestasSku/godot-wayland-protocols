#ifndef SCREENCOPY_NODE_H
#define SCREENCOPY_NODE_H

#include "godot_cpp/classes/ref_counted.hpp"
#include <cstdint>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

// Wayland includes
#include <sys/types.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
extern "C" {
#include "wlr-screencopy-unstable-v1.h"
}

using namespace godot;

struct frame_tmp_data {
  void *shm_data;
  wl_buffer *frame_buffer;
  uint32_t height;
  uint32_t width;
  uint32_t stride;
  uint32_t format;
};

class Screencopy : public RefCounted {
  GDCLASS(Screencopy, RefCounted);

private:
  wl_display *display = nullptr;
  wl_registry *registry = nullptr;
  zwlr_screencopy_manager_v1 *manager = nullptr;
  wl_output *target_output = nullptr;
  Ref<Image> captured_image;

  wl_shm *shm = nullptr;
  frame_tmp_data frame_data;

  static void registry_global(void *data, struct wl_registry *wl_registry,
                              uint32_t id, const char *interface,
                              uint32_t version);
  static void registry_global_remove(void *data,
                                     struct wl_registry *wl_registry,
                                     uint32_t id);
  static void output_buffer(void *data, struct zwlr_screencopy_frame_v1 *frame,
                            uint32_t format, uint32_t width, uint32_t height,
                            uint32_t stride);
  static void output_flags(void *data, struct zwlr_screencopy_frame_v1 *frame,
                           uint32_t flags);
  static void output_ready(void *data, struct zwlr_screencopy_frame_v1 *frame,
                           uint32_t tv_sec_hi, uint32_t tv_sec_lo,
                           uint32_t tv_nsec);
  static void output_failed(void *data, struct zwlr_screencopy_frame_v1 *frame);
  static void output_damage(void *data, struct zwlr_screencopy_frame_v1 *frame,
                            uint32_t x, uint32_t y, uint32_t width,
                            uint32_t height);
  static void output_linux_dmabuf(void *data,
                                  struct zwlr_screencopy_frame_v1 *frame,
                                  uint32_t format, uint32_t width,
                                  uint32_t height);
  static void output_buffer_done(void *data,
                                 struct zwlr_screencopy_frame_v1 *frame);

protected:
  static void _bind_methods();

public:
  Screencopy();
  ~Screencopy();

  void init_wayland();
  void capture_output(bool with_damage = false);
  Ref<Image> get_captured_image() const;
};

#endif
