export module kiln.exec.Bundle;

import kiln.app.Builder;

namespace kiln::exec {

export class Bundle {
public:
    static auto operator()(app::Builder& builder) -> void;
};

}   // namespace kiln::exec
