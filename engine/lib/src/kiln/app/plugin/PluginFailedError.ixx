module;

#include <stdexcept>

export module kiln.app.plugin.PluginFailedError;

namespace kiln::app {

export class PluginFailedError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

}   // namespace kiln::app
