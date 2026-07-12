export module kiln.reg.Registry;

import kiln.reg.ErasedEntry;
import kiln.util.containers.GenericStack;

namespace kiln::reg {

export using Registry = util::BasicGenericStack<ErasedEntry>;

}   // namespace kiln::reg
