module;

#include <stdexcept>

export module kiln.wsi.error.Error;

namespace kiln::wsi {

export class Error : public std::runtime_error {
public:
    explicit Error(const int error_code, const char* description)
        : std::runtime_error{ description },
          m_error_code{ error_code }
    {
    }

    [[nodiscard]]
    auto code() const noexcept -> int
    {
        return m_error_code;
    }

private:
    int m_error_code;
};

}   // namespace kiln::wsi
