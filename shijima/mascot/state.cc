#include "state.hpp"

namespace shijima {
namespace mascot {

void state::roll_dcursor() {
    if ((int)stored_dcursor_data.size() != env->subtick_count) {
        stored_dcursor = { 0, 0, 0, 0 };
        stored_dcursor_data.resize(env->subtick_count);
        for (auto &vec : stored_dcursor_data) {
            vec = { 0, 0 };
        }
        next_dcursor_roll = 0;
    }
    auto raw_cursor = get_raw_cursor();
    stored_dcursor_data[next_dcursor_roll] = { raw_cursor.dx,
        raw_cursor.dy };
    
    stored_dcursor = { raw_cursor.x, raw_cursor.y, 0, 0 };
    for (auto const& vec : stored_dcursor_data) {
        stored_dcursor.dx += vec.x;
        stored_dcursor.dy += vec.y;
    }
    next_dcursor_roll = (next_dcursor_roll + 1) % env->subtick_count;
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
    environment::dvec2 cursor;
    if (dragging && drag_with_local_cursor) {
        cursor = local_cursor;
    }
    else {
        cursor = env->cursor;
    }
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