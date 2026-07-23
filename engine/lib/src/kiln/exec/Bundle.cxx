module kiln.exec.Bundle;

import kiln.app.Builder;
import kiln.exec.data_structures.WorkTree;

namespace kiln::exec {

auto Bundle::operator()(app::Builder& builder) -> void
{
    builder.register_entry<WorkTree>();
}

}   // namespace kiln::exec
