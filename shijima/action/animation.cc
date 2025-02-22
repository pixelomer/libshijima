// 
// libshijima - C++ library for shimeji desktop mascots
// Copyright (C) 2024-2025 pixelomer
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 

#include "animation.hpp"
#include "shijima/hotspot.hpp"
#include <stdexcept>

namespace shijima {
namespace action {

bool animation::requests_broadcast() {
    return true;
}

void animation::init(mascot::tick &ctx) {
    base::init(ctx);
    anim_idx = -1;
}

bool animation::tick() {
    if (!base::tick()) {
        return false;
    }
    if (!check_border_type()) {
        return false;
    }
    if (!handle_dragging()) {
        return false;
    }
    auto &pose = get_pose();
    mascot->anchor.x += dx(pose.velocity.x);
    mascot->anchor.y += dy(pose.velocity.y);
    mascot->active_frame = pose;
    return true;
}


bool animation::check_border_type() {
    auto border_type = vars.get_string("BorderType", "");
    bool on_border = true;
    if (border_type == "Floor") {
        on_border = mascot->env->floor.is_on(mascot->anchor) ||
            mascot->env->active_ie.top_border().is_on(mascot->anchor);
    }
    else if (border_type == "Wall") {
        on_border = mascot->env->work_area.left_border().is_on(mascot->anchor) ||
            mascot->env->work_area.right_border().is_on(mascot->anchor) ||
            mascot->env->active_ie.left_border().is_on(mascot->anchor) ||
            mascot->env->active_ie.right_border().is_on(mascot->anchor);
    }
    else if (border_type == "Ceiling") {
        on_border = mascot->env->work_area.top_border().is_on(mascot->anchor) ||
            mascot->env->active_ie.bottom_border().is_on(mascot->anchor);
    }
    else if (border_type == "") {
        on_border = true;
    }
    else {
        throw std::logic_error("Unknown border: " + border_type);
    }
    if (!on_border) {
        mascot->queued_behavior = "Fall";
        return false;
    }
    return true;
}

bool animation::handle_dragging() {
    if (mascot->dragging) {
        bool draggable = vars.get_bool("Draggable", true);
        math::vec2 cursor = mascot->get_cursor();
        math::vec2 topleft;
        if (mascot->looking_right) {
            //FIXME: assumes width of 128
            topleft = mascot->anchor - math::vec2 {
                128 - mascot->active_frame.anchor.x,
                mascot->active_frame.anchor.y };
        }
        else {
            topleft = mascot->anchor - mascot->active_frame.anchor;
        }
        math::vec2 cursor_rel = cursor - topleft;
        auto &anim = get_animation();
        shijima::hotspot hotspot;
        if (mascot->env->allows_hotspots &&
            (hotspot = anim->hotspot_at(cursor_rel)).valid())
        {
            // Hotspot pressed
            if (hotspot.get_behavior().empty()) {
                // Restart animation
                start_time = mascot->time;
            }
            else {
                // Activate target behavior
                mascot->queued_behavior = hotspot.get_behavior();
                mascot->dragging = false;
                return false;
            }
        }
        else if (draggable) {
            // Started dragging
            mascot->was_on_ie = false;
            mascot->interaction.finalize();
            mascot->queued_behavior = "Dragged";
            return false;
        }
        else {
            mascot->dragging = false;
        }
    }
    return true;
}

std::shared_ptr<shijima::animation> &animation::get_animation() {
    for (int i=0; i<(int)animations.size(); i++) {
        auto &anim = animations[i];
        if (vars.get_bool(anim->condition)) {
            if (anim_idx != i) {
                anim_idx = i;
                start_time = mascot->time;
            }
            return anim;
        }
    }
    throw std::logic_error("no animation available");
}

pose const& animation::get_pose() {
    return get_animation()->get_pose(elapsed());
}

bool animation::animation_finished() {
    return elapsed() >= get_animation()->get_duration();
}

}
}