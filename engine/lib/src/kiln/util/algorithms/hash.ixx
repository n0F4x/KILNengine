module;

#include <cstdint>
#include <string_view>

export module kiln.util.algorithms.hash;

namespace kiln::util {

export [[nodiscard]]
constexpr auto hash_u64(std::string_view string) noexcept -> uint64_t;

}   // namespace kiln::util

namespace kiln::util {

constexpr auto hash_u64(const std::string_view string) noexcept -> uint64_t
{
    /*
     * "Fowler–Noll–Vo - 1a" hash function
     */

    constexpr uint64_t offset{ 14'695'981'039'346'656'037ull };
    // ReSharper disable once CppTooWideScope
    constexpr uint64_t prime{ 1'099'511'628'211ull };

    uint64_t result{ offset };

    for (const auto character : string)
    {
        result = (result ^ static_cast<uint64_t>(character)) * prime;
    }

    return result;
}

}   // namespace kiln::util
