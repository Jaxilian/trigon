#ifdef __linux__

#include <wayland-client.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_wayland.h>
#include "../gfx.h"
#include "cmn/cmn.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

// Minimal Wayland globals. In real usage, store them in 'win_t' as needed.
static struct wl_display*       global_wl_display    = NULL;
static struct wl_compositor*    global_wl_compositor = NULL;
static struct wl_surface*       global_wl_surface    = NULL;

// Called when a new global object (compositor, etc.) is advertised by Wayland.
static void
wayland_registry_global(void* data,
                        struct wl_registry* registry,
                        uint32_t name,
                        const char* interface,
                        uint32_t version)
{
    if (strcmp(interface, "wl_compositor") == 0) {
        global_wl_compositor = wl_registry_bind(
            registry, name, &wl_compositor_interface, version
        );
    }
}

// Called when a global object is removed.
static void
wayland_registry_global_remove(void* data,
                               struct wl_registry* registry,
                               uint32_t name)
{
    // Not used in this simple example
}

static struct wl_registry_listener registry_listener = {
    .global = wayland_registry_global,
    .global_remove = wayland_registry_global_remove,
};

void win_new(win_t* win, str_t win_name, uint32_t width, uint32_t height, uint32_t flags)
{
    (void)win_name; // Not used in minimal Wayland example
    (void)flags;    // Not used in minimal Wayland example

    // Connect to the Wayland display
    global_wl_display = wl_display_connect(NULL);
    if (!global_wl_display) {
        debug_err("Failed to connect to Wayland display\n");
        return;
    }

    // Obtain the registry and bind a compositor
    struct wl_registry* registry = wl_display_get_registry(global_wl_display);
    wl_registry_add_listener(registry, &registry_listener, NULL);
    wl_display_roundtrip(global_wl_display); // ensure compositor is bound

    if (!global_wl_compositor) {
        debug_err("Failed to bind wl_compositor\n");
        return;
    }

    // Create a Wayland surface
    global_wl_surface = wl_compositor_create_surface(global_wl_compositor);
    if (!global_wl_surface) {
        debug_err("Failed to create Wayland surface\n");
        return;
    }

    // Store references in 'win'
    win->handle     = global_wl_display; // You need these fields in win_t
    win->instance   = global_wl_surface;
    win->width      = width;
    win->height     = height;
    win->aspect     = (float)width / (float)height;
    win->active     = true;

    // Create a Vulkan Wayland surface
    if (!gfx_vkinstance()) {
        debug_err("Call gfx_init() before calling win_new()!\n");
        return;
    }

    VkWaylandSurfaceCreateInfoKHR surf_info = {
        .sType   = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
        .display = global_wl_display,
        .surface = global_wl_surface
    };

    VkResult res = vkCreateWaylandSurfaceKHR(gfx_vkinstance(), &surf_info, NULL, &win->surface);
    if (res != VK_SUCCESS) {
        debug_err("Failed to create Wayland surface (vkCreateWaylandSurfaceKHR)\n");
        return;
    }

    // Acquire logical device, create swapchain, sync objects, etc.
    win->device = gfx_device_get(win);
    gfx_swap_sync(win);
    gfx_sync_objects_create(win);
}

bool win_frame_begin(win_t* win)
{
    // Dispatch any Wayland events
    wl_display_dispatch_pending(global_wl_display);
    wl_display_flush(global_wl_display);

    if (!win->active) return false;

    gfx_frame_begin(win);
    return true;
}

void win_frame_end(win_t* win)
{
    if (win->active) {
        gfx_frame_end(win);
    }
}

void win_del(win_t* win)
{
    if (!win->active) {
        return;
    }
    vkDeviceWaitIdle(gfx_dev()->device);

    if (win->delete_cb) {
        win->delete_cb(win);
        win->delete_cb = NULL;
    }

    // Cleanup
    gfx_swap_del(win);
    gfx_sync_objects_destroy(win);
    gfx_device_del(win);

    if (win->surface) {
        vkDestroySurfaceKHR(gfx_vkinstance(), win->surface, NULL);
        win->surface = VK_NULL_HANDLE;
    }

    if (global_wl_surface) {
        wl_surface_destroy(global_wl_surface);
        global_wl_surface = NULL;
    }

    if (global_wl_display) {
        wl_display_disconnect(global_wl_display);
        global_wl_display = NULL;
    }

    win->active = false;
    memset(win, 0, sizeof(win_t));
}

void win_connect_to_del(win_t* win, win_del_cb cb)
{
    win->delete_cb = cb;
}

#endif // __linux__
