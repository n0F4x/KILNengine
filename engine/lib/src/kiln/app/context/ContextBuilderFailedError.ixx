module;

#include <stdexcept>

export module kiln.app.context.ContextBuilderFailedError;

namespace kiln::app {

export class ContextBuilderFailedError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

}   // namespace kiln::app
