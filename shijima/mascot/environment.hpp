#pragma once
#include <shijima/broadcast/manager.hpp>
#include <shijima/math.hpp>
#include <cmath>
#include <random>

namespace shijima {
namespace mascot {

class environment {
public:
    class dvec2 : public math::vec2 {
    public:
        double dx;
        double dy;
        dvec2(): math::vec2(), dx(0), dy(0) {}
        dvec2(double x, double y): math::vec2(x, y), dx(0), dy(0) {}
        dvec2(double x, double y, double dx, double dy): math::vec2(x, y),
            dx(dx), dy(dy) {}
        dvec2(std::string const& str): math::vec2(str), dx(0), dy(0) {}
        dvec2 operator*(double rhs) {
            return { x * rhs, y * rhs, dx * rhs, dy * rhs };
        }
        dvec2 &operator*=(double rhs) {
            return *this = *this * rhs;
        }
        dvec2 operator/(double rhs) {
            return { x / rhs, y / rhs, dx / rhs, dy / rhs };
        }
        dvec2 &operator/=(double rhs) {
            return *this = *this / rhs;
        }
    };

    class border {
    public:
        virtual bool is_on(math::vec2) const = 0;
        virtual bool faces(math::vec2) const = 0;
    };

    class hborder : public border {
    public:
        double y;
        double xstart;
        double xend;
        hborder(double y, double xstart, double xend): y(y), xstart(xstart),
            xend(xend) {}
        hborder() {}
        virtual bool faces(math::vec2 p) const {
            return p.x >= xstart && p.x <= xend;
        }
        virtual bool is_on(math::vec2 p) const {
            return std::fabs(p.y - y) < 1.0 && faces(p);
        }
        hborder operator*(double rhs) {
            return { y * rhs, xstart * rhs, xend * rhs };
        }
        hborder &operator*=(double rhs) {
            return *this = *this * rhs;
        }
        hborder operator/(double rhs) {
            return { y / rhs, xstart / rhs, xend / rhs };
        }
        hborder &operator/=(double rhs) {
            return *this = *this / rhs;
        }
    };

    class vborder : public border {
    public:
        double x;
        double ystart;
        double yend;
        vborder(double x, double ystart, double yend): x(x), ystart(ystart),
            yend(yend) {}
        vborder() {}
        virtual bool faces(math::vec2 p) const {
            return p.y >= ystart && p.y <= yend;
        }
        virtual bool is_on(math::vec2 p) const {
            return std::fabs(p.x - x) < 1.0 && faces(p);
        }
        vborder operator*(double rhs) {
            return { x * rhs, ystart * rhs, yend * rhs };
        }
        vborder &operator*=(double rhs) {
            return *this = *this * rhs;
        }
        vborder operator/(double rhs) {
            return { x / rhs, ystart / rhs, yend / rhs };
        }
        vborder &operator/=(double rhs) {
            return *this = *this / rhs;
        }
    };

    class area {
    public:
        double top;
        double right;
        double bottom;
        double left;
        bool visible() {
            return (left != right) && (top != bottom);
        }
        hborder bottom_border() const { return { bottom, left, right  }; }
        hborder top_border()    const { return { top,    left, right  }; }
        vborder left_border()   const { return { left,   top,  bottom }; }
        vborder right_border()  const { return { right,  top,  bottom }; }
        double width()  const { return right - left; }
        double height() const { return bottom - top; }
        bool is_on(math::vec2 anchor) const {
            return this->left_border().is_on(anchor) ||
                this->right_border().is_on(anchor) ||
                this->bottom_border().is_on(anchor) ||
                this->top_border().is_on(anchor);
        }
        area(double top, double right, double bottom, double left): top(top),
            right(right), bottom(bottom), left(left) {}
        area() {}
        static area from_rec(math::rec rec) {
            return area { rec.y, rec.x + rec.width, rec.y + rec.height,
                rec.x };
        }
        static area from_vec2(math::vec2 vec2) {
            return from_rec({ 0, 0, vec2.x, vec2.y });
        }
        area operator*(double rhs) {
            return { top * rhs, right * rhs, bottom * rhs, left * rhs };
        }
        area &operator*=(double rhs) {
            return *this = *this * rhs;
        }
        area operator/(double rhs) {
            return { top / rhs, right / rhs, bottom / rhs, left / rhs };
        }
        area &operator/=(double rhs) {
            return *this = *this / rhs;
        }
    };

    class darea : public area {
    public:
        double dx;
        double dy;
        darea(double top, double right, double bottom, double left):
            darea(top, right, bottom, left, 0, 0) {}
        darea(double top, double right, double bottom, double left,
            double dx, double dy):
            area(top, right, bottom, left), dx(dx), dy(dy) {}
        darea(): area(), dx(0), dy(0) {}
        darea(area const& rhs): area(rhs), dx(0), dy(0) {}
        darea operator*(double rhs) {
            return { top * rhs, right * rhs, bottom * rhs, left * rhs,
                dx * rhs, dy * rhs };
        }
        darea &operator*=(double rhs) {
            return *this = *this * rhs;
        }
        darea operator/(double rhs) {
            return { top / rhs, right / rhs, bottom / rhs, left / rhs,
                dx / rhs, dy / rhs };
        }
        darea &operator/=(double rhs) {
            return *this = *this / rhs;
        }
    };

private:
    std::mt19937 rng { (std::random_device{})() };

public:
    hborder ceiling;
    hborder floor;
    area screen;
    area work_area;
    darea active_ie;
    dvec2 cursor;
    bool allows_breeding = true;
    long mascot_count = 0;
    bool sticky_ie = true;

    // [0.0, 1.0)
    double random() {
        std::uniform_real_distribution<double> dist(0, 1);
        double result = dist(rng);
        return result;
    }

    // [0, upper_range)
    int random(int upper_range) {
        std::uniform_int_distribution<int> dist(0, upper_range-1);
        int result = dist(rng);
        return result;
    }

    template<class T>
    void seed(T seq) {
        rng.seed(seq);
    }
private:
    double active_scale = 1.0;
public:
    double get_scale() {
        return active_scale;
    }
    void reset_scale() {
        if (active_scale == 1.0) {
            return;
        }
        ceiling /= active_scale;
        floor /= active_scale;
        screen /= active_scale;
        work_area /= active_scale;
        active_ie /= active_scale;
        cursor /= active_scale;
        active_scale = 1.0;
    }
    void set_scale(double scale) {
        if (active_scale != 1.0) {
            reset_scale();
        }
        ceiling *= scale;
        floor *= scale;
        screen *= scale;
        work_area *= scale;
        active_ie *= scale;
        cursor *= scale;
        active_scale = scale;
    }

    broadcast::manager broadcasts;
};

}
}