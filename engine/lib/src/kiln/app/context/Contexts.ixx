export module kiln.app.context.Contexts;

import kiln.app.context.ErasedContext;
import kiln.util.containers.GenericStack;

namespace kiln::app {

export using Contexts = util::BasicGenericStack<ErasedContext>;

}   // namespace kiln::app
