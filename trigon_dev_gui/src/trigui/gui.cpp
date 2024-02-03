#include "gui.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include <stdio.h>         
#include <stdlib.h>         
#include <cstdarg>
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
/* --------------- SETUP VULKAN -------------------*/
static VkPipelineCache pipeline_cache = VK_NULL_HANDLE;
static VkDescriptorPool pool = VK_NULL_HANDLE;
static ImGuiIO* io;

void gui_destroy(gui_vk_info_t* info) {
    if (pool == VK_NULL_HANDLE) return;
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    vkDestroyDescriptorPool((VkDevice)info->vulkan_device, pool, NULL);
    ImGui::DestroyContext();
}

static void load_vk_impl(gui_vk_info_t* info) {

    VkDescriptorPoolSize pool_sizes[] =
    {
       { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
       { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
       { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
       { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
       { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
       { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
       { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
       { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
       { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
       { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
       { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1;
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    VkResult err = vkCreateDescriptorPool((VkDevice)info->vulkan_device, &pool_info, NULL, &pool);
    if (err != VK_SUCCESS) {
        printf("Failed to create imgui descriptor pool!\n");
        exit(1);
    }

    ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)info->vulkan_window, true);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = (VkInstance)info->vulkan_instance;
    init_info.PhysicalDevice = (VkPhysicalDevice)info->vulkan_physical_device;
    init_info.Device = (VkDevice)info->vulkan_device;
    init_info.QueueFamily   = info->vulkan_graphics_family;
    init_info.Queue         = (VkQueue)info->vulkan_graphics_queue;
    init_info.PipelineCache = pipeline_cache;
    init_info.DescriptorPool = pool;
    init_info.Subpass = 0;
    init_info.MinImageCount = info->vulkan_min_image_count;
    init_info.ImageCount = info->vulkan_image_count;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = NULL;
    init_info.CheckVkResultFn = NULL;
    if (!ImGui_ImplVulkan_Init(&init_info, (VkRenderPass)info->vulkan_renderpass)) {
        printf("failed to load vulkan for imgui!\n");
        exit(1);
    }

  
}



/* --------------- INTERFACE -------------------*/


void gui_init_vk(gui_vk_info_t* info) {
    gui_destroy(info);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO(); (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    //below comment doesn't work! don't enable yet
   // io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    ImGui::StyleColorsDark();

    load_vk_impl(info);
    gui_apply_style();
}

//bool show_demo = false;
void gui_frame_begin() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    //ImGui::ShowDemoWindow(&show_demo);
}

void gui_frame_end(void* command_buffer) {
    ImGui::Render();
    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), (VkCommandBuffer)command_buffer);

}

bool gui_window_new(const char* name, bool* active) {
    return ImGui::Begin(name, active);
}

void gui_window_end() {
    ImGui::End();

}

void gui_text(const char* content, ...) {
    va_list args;
    va_start(args, content);
    ImGui::TextV(content, args);
    va_end(args);
}

bool gui_check_box(const char* label, bool* value) {
    return ImGui::Checkbox(label, value);
}

bool gui_button(const char* label, float extent[2]) {
    return ImGui::Button(label, ImVec2(extent[0], extent[1]));
}

bool gui_main_menu_begin() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiID dockspace_id = ImGui::GetID("default_dock_space");
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGui::DockSpaceOverViewport(viewport, dockspace_flags, nullptr);
    return ImGui::BeginMainMenuBar();
}

void gui_main_menu_end() {
    ImGui::EndMainMenuBar();
}

bool gui_menu_begin(const char* name, bool active) {
    return ImGui::BeginMenu(name, active);
}

void gui_menu_end() {
    ImGui::EndMenu();
}

bool gui_menu_item(const char* name, const char* shortcut, bool* is_selected, bool active) {
    return ImGui::MenuItem(name, shortcut, is_selected, active);
}