#pragma once

#include <concepts>

namespace kiln::util {

template <typename LHS, typename RHS>
concept nothrow_assignable_from_c = std::is_lvalue_reference_v<LHS>
                                 && std::common_reference_with<
                                        const std::remove_reference_t<LHS>&,
                                        const std::remove_reference_t<RHS>&>
                                 && requires(LHS lhs, RHS&& rhs) {
                                        {
                                            lhs = std::forward<RHS>(rhs)
                                        } noexcept -> std::same_as<LHS>;
                                    };

}   // namespace kiln::util
