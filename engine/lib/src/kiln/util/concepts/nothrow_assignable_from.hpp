#pragma once

#include <concepts>

namespace kiln::util {

template <typename LHS_T, typename RHS_T>
concept nothrow_assignable_from_c = std::is_lvalue_reference_v<LHS_T>
                                 && std::common_reference_with<
                                        const std::remove_reference_t<LHS_T>&,
                                        const std::remove_reference_t<RHS_T>&>
                                 && requires(LHS_T lhs, RHS_T&& rhs) {
                                        {
                                            lhs = std::forward<RHS_T>(rhs)
                                        } noexcept -> std::same_as<LHS_T>;
                                    };

}   // namespace kiln::util
