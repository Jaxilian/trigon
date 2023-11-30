#include "mesh.h"
#include "dev/mem/mem.h"
#include "dev/gpu/vk.h"

static bool     initialized = false;
static cpool_t  mesh_pool = { 0 };

typedef struct trigon {
    bool            initialized;
    uint32_t        vertex_count;
    uint32_t        index_count;
    vk_buffer_t     buffer;
    size_t          index_offset;
} mesh_t;

static void initialize() {
    cpool_new(&mesh_pool, sizeof(mesh_t), 2);
    initialized = true;
}

uint32_t mesh_new(const mesh_info_t* data) {
    if (!initialized) initialize();

    uint32_t id = cpool_add(&mesh_pool, NULL);
    mesh_t* mesh = cpool_get(&mesh_pool, id);

    mesh->vertex_count          = data->vertex_count;
    mesh->index_count           = data->index_count;
    mesh->index_offset          = sizeof(vertex_t) * data->vertex_count;

    size_t vertex_data_size     = data->vertex_count * sizeof(vertex_t);
    size_t index_data_size      = data->index_count * sizeof(uint32_t);
    size_t mesh_size            = vertex_data_size + index_data_size;
    uint32_t mesh_count         = 1;

    VkResult success = VK_FALSE;

    vk_buffer_new(
        mesh_size,
        mesh_count,
        &mesh->buffer,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
    );

    char* mapped_memory = (char*)vk_buffer_set_advanced(&mesh->buffer);
    memcpy(mapped_memory, data->vertices, mesh->index_offset);
    memcpy(mapped_memory + mesh->index_offset, data->indices, sizeof(uint32_t) * data->index_count);
    vk_buffer_set_advanced_submit(&mesh->buffer, mapped_memory);

    mesh->initialized = true;

    return id;
}

void mesh_bind(uint32_t id) {
    mesh_t* mesh = cpool_get(&mesh_pool, id);
    if (!mesh->initialized) return;

    VkDeviceSize vertex_offset = 0;
    vkCmdBindVertexBuffers(vk_command_buffer(), 0, 1, &mesh->buffer.buffer, &vertex_offset);
    vkCmdBindIndexBuffer(vk_command_buffer(), mesh->buffer.buffer, mesh->index_offset, VK_INDEX_TYPE_UINT32);
}

void mesh_draw(uint32_t id, uint32_t instance_count) {
    mesh_t* mesh = cpool_get(&mesh_pool, id);
    if (!mesh->initialized) return;

    vkCmdDrawIndexed(vk_command_buffer(), mesh->index_count, instance_count, 0, 0, 0);
}

void mesh_del(uint32_t id) {
    mesh_t* mesh = cpool_get(&mesh_pool, id);
    if (!mesh->initialized) return;

    vk_buffer_del(&mesh->buffer);
    cpool_pop(&mesh_pool, id);
}

void mesh_clear() {
    for (uint32_t i = 0; i < mesh_pool.max_count; i++) {
        mesh_del(i);
    }
    cpool_del(&mesh_pool);
}

