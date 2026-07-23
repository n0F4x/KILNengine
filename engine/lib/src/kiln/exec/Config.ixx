module;

#include <cstdint>
#include <thread>

export module kiln.exec.Config;

import kiln.reg.EntryTraits;

namespace kiln::exec {

export class Config {
public:
    explicit Config(
        const uint32_t number_of_threads = std::thread::hardware_concurrency(),
        const uint32_t task_capacity     = 1u << 12u
    )
        : m_number_of_threads{ number_of_threads },
          m_task_capacity{ task_capacity }
    {
    }

    [[nodiscard]]
    auto number_of_threads() const noexcept -> uint32_t
    {
        return m_number_of_threads;
    }

    [[nodiscard]]
    auto task_capacity() const noexcept -> uint32_t
    {
        return m_task_capacity;
    }

private:
    uint32_t m_number_of_threads;
    uint32_t m_task_capacity;
};

}   // namespace kiln::exec

template <>
struct kiln::reg::EntryTraits<kiln::exec::Config> {
    constexpr static bool is_configuration_entry{ true };
};
