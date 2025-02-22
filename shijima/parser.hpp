#pragma once

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

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <rapidxml/rapidxml.hpp>
#include <set>
#include "animation.hpp"
#include "action/reference.hpp"
#include "behavior/list.hpp"
#include "behavior/base.hpp"
#include "hotspot.hpp"

namespace shijima {

class parser {
private:
    void try_parse_sequence(std::shared_ptr<action::base> &action,
        rapidxml::xml_node<> *node, std::string const& type);
    void try_parse_instant(std::shared_ptr<action::base> &action,
        rapidxml::xml_node<> *node, std::string const& type);
    void try_parse_animation(std::shared_ptr<action::base> &action,
        rapidxml::xml_node<> *node, std::string const& type);
    std::shared_ptr<animation> parse_animation(rapidxml::xml_node<> *node);
    pose parse_pose(rapidxml::xml_node<> *node);
    bool parse_hotspot(rapidxml::xml_node<> *node, shijima::hotspot &hotspot);
    static std::map<std::string, std::string> all_attributes(rapidxml::xml_node<> *node,
        std::map<std::string, std::string> const& defaults = {});
    std::shared_ptr<action::base> parse_action(rapidxml::xml_node<> *action, bool is_child);
    behavior::list parse_behavior_list(rapidxml::xml_node<> *node, bool allow_references);
    void parse_actions(std::string const& actions);
    void parse_behaviors(std::string const& behaviors);
    void connect_actions(behavior::list &behaviors);
    void cleanup();

    std::vector<std::shared_ptr<action::reference>> action_refs;
    std::vector<std::shared_ptr<behavior::base>> behavior_refs;
    std::map<std::string, std::shared_ptr<action::base>> actions;
public:
    behavior::list behavior_list;
    std::set<shijima::pose> poses;
    std::map<std::string, std::string> constants;
    parser() {}
    void parse(std::string const& actions_xml, std::string const& behaviors_xml) {
        // Clean results from any previous parse calls
        poses.clear();
        constants.clear();
        behavior_list = {};

        parse_actions(actions_xml);
        parse_behaviors(behaviors_xml);

        // Clean intermediary variables
        action_refs.clear();
        behavior_refs.clear();
        actions.clear();
    }
};

}
