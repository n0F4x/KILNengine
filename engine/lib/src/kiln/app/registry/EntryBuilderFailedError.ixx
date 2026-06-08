module;

#include <stdexcept>

export module kiln.app.registry.EntryBuilderFailedError;

namespace kiln::app {

export class EntryBuilderFailedError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

}   // namespace kiln::app
