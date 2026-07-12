module;

#include <stdexcept>

export module kiln.reg.EntryBuilderFailedError;

namespace kiln::reg {

export class EntryBuilderFailedError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

}   // namespace kiln::reg
