export module kiln.wsi.Position;

namespace kiln::wsi {

export template <typename T>
struct Position2 {
    T x;
    T y;
};

export using Position2d = Position2<double>;

}   // namespace kiln::wsi
