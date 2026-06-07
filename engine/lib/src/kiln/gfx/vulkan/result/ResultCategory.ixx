export module kiln.gfx.vulkan.result.ResultCategory;

namespace kiln::gfx::vulkan {

export enum struct ResultCategory
{
    eSuccess,
    eRuntimeError,
    ePreconditionViolationError,
    eInternalContractViolationError,
};

}   // namespace kiln::gfx::vulkan
