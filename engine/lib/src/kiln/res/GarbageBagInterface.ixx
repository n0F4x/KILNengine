export module kiln.res.GarbageBagInterface;

namespace kiln::res {

export class GarbageBagInterface {
public:
    virtual ~GarbageBagInterface() = default;

    [[nodiscard]]
    virtual auto can_be_thrown_out() -> bool = 0;
};

}   // namespace kiln::res
