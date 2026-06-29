export module kiln.gfx.vulkan.structure_chain.extends_struct_c;

import vulkan;

namespace kiln::gfx::vulkan {

export template <typename T, typename ExtendedStruct_T>
concept extends_struct_c
    = static_cast<bool>(vk::StructExtends<T, ExtendedStruct_T>::value);

}   // namespace kiln::gfx::vulkan
