export module kiln.util.type_traits.always_true;

namespace kiln::util {

export template <typename>
struct always_true {
    constexpr static bool value = true;
};

}   // namespace kiln::util
