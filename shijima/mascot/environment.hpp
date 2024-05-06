#pragma once
namespace shijima {
namespace mascot {

class environment {
public:
    class dvec2 : public math::vec2 {
    public:
        int dx;
        int dy;
        dvec2(): math::vec2(), dx(0), dy(0) {}
        dvec2(double x, double y): math::vec2(x, y), dx(0), dy(0) {}
        dvec2(double x, double y, double dx, double dy): math::vec2(x, y),
            dx(dx), dy(dy) {}
        dvec2(std::string const& str): math::vec2(str), dx(0), dy(0) {}
    };

    class border {
    public:
        virtual bool is_on(math::vec2) const = 0;
    };

    class hborder : public border {
    public:
        int y;
        int xstart;
        int xend;
        hborder(int y, int xstart, int xend): y(y), xstart(xstart),
            xend(xend) {}
        hborder() {}
        virtual bool is_on(math::vec2 p) const {
            return p.y == y && p.x >= xstart && p.x <= xend;
        }
    };

    class vborder : public border {
    public:
        int x;
        int ystart;
        int yend;
        vborder(int x, int ystart, int yend): x(x), ystart(ystart),
            yend(yend) {}
        vborder() {}
        virtual bool is_on(math::vec2 p) const {
            return p.x == x && p.y >= ystart && p.y <= yend;
        }
    };

    class area {
    public:
        int top;
        int right;
        int bottom;
        int left;
        bool visible() {
            return (left != right) && (top != bottom);
        }
        hborder bottom_border() const { return { bottom, left, right  }; }
        hborder top_border()    const { return { top,    left, right  }; }
        vborder left_border()   const { return { left,   top,  bottom }; }
        vborder right_border()  const { return { right,  top,  bottom }; }
        int width() { return right - left; }
        int height() { return bottom - top; }
        area(int top, int right, int bottom, int left): top(top),
            right(right), bottom(bottom), left(left) {}
        area() {}
    };

    hborder ceiling;
    hborder floor;
    area screen;
    area work_area;
    area active_ie;
    dvec2 cursor;
};

}
}