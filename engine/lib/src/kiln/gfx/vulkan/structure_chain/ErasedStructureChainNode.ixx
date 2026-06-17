export module kiln.gfx.vulkan.structure_chain.ErasedStructureChainNode;

import vulkan_hpp;

import kiln.gfx.vulkan.structure_chain.StructureChainNodeInterface;
import kiln.util.concepts.naked;
import kiln.util.containers.Polymorphic;

namespace kiln::gfx::vulkan {

export template <util::naked_c RootStruct_T>
using ErasedStructureChainNode
    = util::Polymorphic<StructureChainNodeInterface<RootStruct_T>>;

}   // namespace kiln::gfx::vulkan
