module;

#include <concepts>
#include <type_traits>

export module kiln.reg.EntryBuilderInterface;

import kiln.reg.entry_c;
import kiln.reg.EntryBuilderBase;
import kiln.reg.Registry;
import kiln.util.type_traits.arguments_of;
import kiln.util.TypeList;

namespace kiln::reg {

export template <entry_c Entry_T>
class EntryBuilderInterface : public internal::EntryBuilderBase {
    template <typename FetchDependency_T, typename Self_T>
        requires std::derived_from<std::remove_cvref_t<Self_T>, EntryBuilderInterface>
    friend auto build(
        EntryBuilderBase,
        Self_T&&            self,
        Registry&           registry,
        FetchDependency_T&& fetch_dependency
    ) -> void
    {
        [&self, &registry, &fetch_dependency]<typename... Dependencies_T>(
            util::TypeList<Dependencies_T...>
        ) -> void
        {
            registry.emplace<Entry_T>(
                std::move(self)
                    .build(fetch_dependency.template operator()<Dependencies_T>()...)   //
            );
        }(util::arguments_of_t<decltype(&std::remove_cvref_t<Self_T>::build)>{});
    }
};

}   // namespace kiln::reg
