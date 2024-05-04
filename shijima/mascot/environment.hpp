#pragma once
namespace shijima {
namespace mascot {

class environment {
public:
    class border {
    public:
        virtual bool is_on(math::vec2) = 0;
    };

    class hborder : public border {
    public:
        int y;
        int xstart;
        int xend;
        hborder(int y, int xstart, int xend): y(y), xstart(xstart),
            xend(xend) {}
        hborder() {}
        virtual bool is_on(math::vec2 p) {
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
        virtual bool is_on(math::vec2 p) {
            return p.x == x && p.y >= ystart && p.y <= yend;
        }
    };

    class area {
    public:
        int top;
        int right;
        int bottom;
        int left;
        hborder bottom_border() { return { bottom, left, right  }; }
        hborder top_border()    { return { top,    left, right  }; }
        vborder left_border()   { return { left,   top,  bottom }; }
        vborder right_border()  { return { right,  top,  bottom }; }
        int width() { return right - left; }
        int height() { return bottom - top; }
        area(int top, int right, int bottom, int left): top(top),
            right(right), bottom(bottom), left(left) {}
        area() {}
    };

    hborder ceiling;
    hborder floor;
    area work_area;
    area active_ie;
    math::vec2 cursor;
};

}
}