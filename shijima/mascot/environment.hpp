#pragma once
namespace shijima {
namespace mascot {

class environment {
public:
    class area {
    public:
        int top;
        int right;
        int bottom;
        int left;
    };

    area work_area;
    math::vec2 cursor;
};

}
}