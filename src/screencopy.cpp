#include "screencopy.h"
#include "godot_cpp/classes/display_server.hpp"
#include "godot_cpp/classes/font_file.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "wlr-screencopy-unstable-v1-client-protocol.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <godot_cpp/classes/image.hpp>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>

using namespace godot;


void Screencopy::registry_global(void *data,
                                     struct wl_registry *wl_registry,
                                     uint32_t id, const char *interface,
                                     uint32_t version) {
  auto *self = static_cast<Screencopy *>(data);
  if (strcmp(interface, zwlr_screencopy_manager_v1_interface.name) == 0) {
    self->manager = static_cast<zwlr_screencopy_manager_v1 *>(wl_registry_bind(
        wl_registry, id, &zwlr_screencopy_manager_v1_interface, 3));
  } else if (strcmp(interface, wl_output_interface.name) == 0) {
    self->target_output = static_cast<wl_output *>(
        wl_registry_bind(wl_registry, id, &wl_output_interface, 2));
  } else if (strcmp(interface, wl_shm_interface.name) == 0) {
    self->shm = static_cast<wl_shm *>(
        wl_registry_bind(self->registry, id, &wl_shm_interface, version));
  }
}

void Screencopy::registry_global_remove(void *data,
                                            struct wl_registry *wl_registry,
                                            uint32_t id) {}

int create_shm_file(off_t size) {
  char name[] = "/wl_shm-XXXXX";
  int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
  shm_unlink(name);
  if (fd < 0)
    return -1;
  if (ftruncate(fd, size) < 0) {
    close(fd);
    return -1;
  }
  return fd;
}

void Screencopy::output_buffer(void *data,
                                   struct zwlr_screencopy_frame_v1 *frame,
                                   uint32_t format, uint32_t width,
                                   uint32_t height, uint32_t stride) {
  auto *self = static_cast<Screencopy *>(data);

  size_t size = stride * height;
  int fd = create_shm_file(size);

  void *shm_data =
      mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  struct wl_shm_pool *pool = wl_shm_create_pool(self->shm, fd, size);
  struct wl_buffer *buffer =
      wl_shm_pool_create_buffer(pool, 0, width, height, stride, format);
  wl_shm_pool_destroy(pool);
  close(fd);

  struct frame_tmp_data f_data = {.shm_data = shm_data,
                                  .frame_buffer = buffer,
                                  .height = height,
                                  .width = width,
                                  .stride = stride,
                                  .format = format};

  self->frame_data = f_data;

  zwlr_screencopy_frame_v1_copy(frame, buffer);
  wl_display_roundtrip(self->display);
}

void Screencopy::output_flags(void *data,
                                  struct zwlr_screencopy_frame_v1 *frame,
                                  uint32_t flags) {
  // Handle flags (e.g., check for Y-invert or other buffer properties)
  // Let's do nothing for now.
}

void Screencopy::output_ready(void *data,
                                  struct zwlr_screencopy_frame_v1 *frame,
                                  uint32_t tv_sec_hi, uint32_t tv_sec_lo,
                                  uint32_t tv_nsec) {
  auto *self = static_cast<Screencopy *>(data);

  struct frame_tmp_data *fr_data = &self->frame_data;
  size_t size = fr_data->stride * fr_data->height;
  uint8_t *src = static_cast<uint8_t *>(fr_data->shm_data);

  PackedByteArray bytes;
  bytes.resize(size);
  memcpy(bytes.ptrw(), src, size);

  Image::Format godot_format = Image::FORMAT_RGBA8;
  switch (fr_data->format) {
  case WL_SHM_FORMAT_XRGB8888:
    godot_format = Image::FORMAT_RGB8;
    break;
  case WL_SHM_FORMAT_ARGB8888:
    godot_format = Image::FORMAT_RGBA8;
    break;
  // case WL_SHM_FORMAT_XBGR8888:
  //   godot_format = Image::FORMAT_RGB8;
  //   // TODO: swap R/B channels if needed
  //   break;
  // case WL_SHM_FORMAT_ABGR8888:
  //   godot_format = Image::FORMAT_RGBA8;
  //   // TODO: swap R/B channels if needed
  //   break;
  default:
    UtilityFunctions::printerr("Unsupported format: ", (int)fr_data->format);
  }

  // Quick and dirty hack to get stuff working
  if (godot_format == Image::FORMAT_RGB8) {
    bytes.resize(fr_data->height * fr_data->width * 3);
    uint8_t *dst = bytes.ptrw();
    uint8_t *src = static_cast<uint8_t *>(fr_data->shm_data);

    for (uint32_t y = 0; y < fr_data->height; y++) {
      for (uint32_t x = 0; x < fr_data->width; x++) {
        int src_i = (y * fr_data->width + x) * 4;
        int dst_i = (y * fr_data->width + x) * 3;
        dst[dst_i + 0] = src[src_i + 2]; // R
        dst[dst_i + 1] = src[src_i + 1]; // G
        dst[dst_i + 2] = src[src_i + 0]; // B
      }
    }
  }

  self->captured_image.instantiate();
  self->captured_image->set_data(fr_data->width, fr_data->height, false,
                                 godot_format, bytes);
  self->emit_signal("capture_ready", self->captured_image);

  // Some cleanup
  munmap(fr_data->shm_data, fr_data->height * fr_data->stride);
  fr_data->shm_data = nullptr;
  wl_buffer_destroy(fr_data->frame_buffer);
  zwlr_screencopy_frame_v1_destroy(frame);
}

void Screencopy::output_failed(void *data,
                                   struct zwlr_screencopy_frame_v1 *frame) {
  UtilityFunctions::printerr("Screencopy failed");

  // clean up
  zwlr_screencopy_frame_v1_destroy(frame);
}

void Screencopy::output_damage(void *data,
                                   struct zwlr_screencopy_frame_v1 *frame,
                                   uint32_t x, uint32_t y, uint32_t width,
                                   uint32_t height) {
  // Handle partial updates if with_damage=true
  UtilityFunctions::print("output damage was called");
  // TODO: see later
}

void Screencopy::output_linux_dmabuf(void *data,
                                         struct zwlr_screencopy_frame_v1 *frame,
                                         uint32_t format, uint32_t width,
                                         uint32_t height) {
  // Handle Linux DMA-BUF buffer
  // Empty for now, will look into later
}

void Screencopy::output_buffer_done(
    void *data, struct zwlr_screencopy_frame_v1 *frame) {
  // Handle buffer completion (optional, can leave empty if not needed)
  UtilityFunctions::print("output buffer called");
}

void Screencopy::_bind_methods() {
  ClassDB::bind_method(D_METHOD("init_wayland"), &Screencopy::init_wayland);
  ClassDB::bind_method(D_METHOD("capture_output", "with_damage"),
                       &Screencopy::capture_output);
  ClassDB::bind_method(D_METHOD("get_captured_image"),
                       &Screencopy::get_captured_image);
  ADD_SIGNAL(MethodInfo("capture_ready",
                        PropertyInfo(Variant::OBJECT, "image",
                                     PROPERTY_HINT_RESOURCE_TYPE, "Image")));
}

Screencopy::Screencopy() {}
Screencopy::~Screencopy() {
  if (shm)
    wl_shm_destroy(shm);
  if (manager)
    zwlr_screencopy_manager_v1_destroy(manager);
  if (target_output)
    wl_output_destroy(target_output);
  if (registry)
    wl_registry_destroy(registry);
  if (display)
    wl_display_disconnect(display);
}

void Screencopy::init_wayland() {
  display = wl_display_connect(nullptr);
  if (!display) {
    UtilityFunctions::printerr("Failed to connect to Wayland display");
    return;
  }

  registry = wl_display_get_registry(display);
  const wl_registry_listener registry_listener = {
      .global = registry_global, .global_remove = registry_global_remove};
  wl_registry_add_listener(registry, &registry_listener, this);
  wl_display_roundtrip(display);
}

void Screencopy::capture_output(bool with_damage) {
  if (!manager || !target_output)
    return;

  auto *frame = zwlr_screencopy_manager_v1_capture_output(
      manager, with_damage ? 1 : 0, target_output);

  const zwlr_screencopy_frame_v1_listener frame_listener = {
      .buffer = output_buffer,
      .flags = output_flags,
      .ready = output_ready,
      .failed = output_failed,
      .damage = output_damage,
      .linux_dmabuf = output_linux_dmabuf,
      .buffer_done = output_buffer_done};
  zwlr_screencopy_frame_v1_add_listener(frame, &frame_listener, this);

  wl_display_roundtrip(display);
}

Ref<Image> Screencopy::get_captured_image() const { return captured_image; }
