module;

#include <memory_resource>
#include <span>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.asset.Buffer;

import kiln.util.contracts;

namespace kiln::gfx::asset {

Buffer::Buffer(Buffer&& other) noexcept
    : m_allocator{ other.m_allocator },
      m_size{ other.m_size },
      m_handle{ std::exchange(other.m_handle, nullptr) }
{
}

Buffer::Buffer(Buffer&& other, const allocator_type& allocator)
    : Buffer{ std::move(other) }
{
    PRECOND(allocator == other.get_allocator());
}

Buffer::~Buffer()
{
    if (m_handle != nullptr)
    {
        m_allocator.deallocate(m_handle, m_size);
    }
}

Buffer::Buffer(const uint64_t size)
    : Buffer{ std::allocator_arg, std::pmr::get_default_resource(), size }
{
}

Buffer::Buffer(std::allocator_arg_t, const allocator_type& allocator, const uint64_t size)
    : m_allocator{ allocator },
      m_size{ size },
      m_handle{ m_allocator.allocate(m_size) }
{
}

auto Buffer::operator=(Buffer&& other) -> Buffer&
{
    PRECOND(get_allocator() == other.get_allocator());

    clear();
    swap(other);

    return *this;
}

auto Buffer::get_allocator() const noexcept -> allocator_type
{
    return m_allocator;
}

auto Buffer::bytes() noexcept -> std::span<std::byte>
{
    return std::span{ m_handle, m_size };
}

auto Buffer::bytes() const noexcept -> std::span<const std::byte>
{
    return std::span{ m_handle, m_size };
}

auto Buffer::clear() -> void
{
    if (m_handle != nullptr)
    {
        m_allocator.deallocate(m_handle, m_size);
        m_size   = 0;
        m_handle = nullptr;
    }
}

auto Buffer::swap(Buffer& other) -> void
{
    PRECOND(get_allocator() == other.get_allocator());

    std::swap(m_size, other.m_size);
    std::swap(m_handle, other.m_handle);
}

// ReSharper disable once CppSpecialFunctionWithoutNoexceptSpecification
auto swap(Buffer& lhs, Buffer& rhs) -> void
{
    lhs.swap(rhs);
}

}   // namespace kiln::gfx::asset
