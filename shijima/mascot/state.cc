#include "state.hpp"

namespace shijima {
namespace mascot {

void state::roll_dcursor() {
    int roller_size = env->subtick_count + 1;
    if ((int)stored_dcursor_data.size() != roller_size) {
        stored_dcursor = { 0, 0, 0, 0 };
        stored_dcursor_data.resize(roller_size);
        for (auto &vec : stored_dcursor_data) {
            vec = { 0, 0 };
        }
        next_dcursor_roll = 0;
    }
    
    auto raw_cursor = get_raw_cursor();
    if (dragging) {
        stored_dcursor_data[next_dcursor_roll] = { raw_cursor.dx,
            raw_cursor.dy };
    }
    else {
        stored_dcursor_data[next_dcursor_roll] = { 0, 0 };
    }
    
    stored_dcursor = { raw_cursor.x, raw_cursor.y, 0, 0 };
    for (auto const& vec : stored_dcursor_data) {
        stored_dcursor.dx += vec.x;
        stored_dcursor.dy += vec.y;
    }
    next_dcursor_roll = (next_dcursor_roll + 1) % roller_size;
}

environment::dvec2 &state::get_raw_cursor() {
    if (dragging && drag_with_local_cursor) {
        return local_cursor;
    }
    else {
        return env->cursor;
    }
}

environment::dvec2 state::get_cursor() {
    return stored_dcursor;
}

bool state::on_land() {
    return env->floor.is_on(anchor) ||
        env->ceiling.is_on(anchor) ||
        env->work_area.is_on(anchor) ||
        env->active_ie.is_on(anchor);
}

}
}
