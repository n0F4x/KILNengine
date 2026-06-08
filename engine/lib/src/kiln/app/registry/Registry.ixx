export module kiln.app.registry.Registry;

import kiln.app.registry.ErasedEntry;
import kiln.util.containers.GenericStack;

namespace kiln::app {

export using Registry = util::BasicGenericStack<ErasedEntry>;

}   // namespace kiln::app
