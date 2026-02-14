module;

#include <stdexcept>

export module kiln.wsi.Error;

namespace kiln::wsi {

export class Error : public std::runtime_error {
public:
    explicit Error(const char* description) : std::runtime_error{ description } {}
};

}   // namespace kiln::wsi
