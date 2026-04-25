module;

#include <utility>

export module examples.simple_scene.workflow.Scene;

import vulkan_hpp;

import kiln.gfx.renderer.memory.Buffer;

namespace demo {

export class Scene {
public:
    explicit Scene(
        kiln::gfx::renderer::Buffer&& geometry_buffer,
        kiln::gfx::renderer::Buffer&& material_buffer,
        kiln::gfx::renderer::Buffer&& primitive_buffer
    )
        : m_geometry_buffer{ std::move(geometry_buffer) },
          m_material_buffer{ std::move(material_buffer) },
          m_primitive_buffer{ std::move(primitive_buffer) }
    {
    }

private:
    kiln::gfx::renderer::Buffer m_geometry_buffer;
    kiln::gfx::renderer::Buffer m_material_buffer;
    kiln::gfx::renderer::Buffer m_primitive_buffer;
};

}   // namespace demo
