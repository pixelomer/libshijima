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
#if !defined(SHIJIMA_NO_PUGIXML)
#include <pugixml.hpp>
#endif // !defined(SHIJIMA_NO_PUGIXML)
#include <set>
#include "animation.hpp"
#include "action/reference.hpp"
#include "behavior/list.hpp"
#include "behavior/base.hpp"
#include "hotspot.hpp"
#include <ostream>
#include <istream>
#include "parser_error.hpp"

namespace shijima {

class parser {
private:
#if !defined(SHIJIMA_NO_PUGIXML)
    void try_parse_sequence(std::shared_ptr<action::base> &action,
        pugi::xml_node node, std::string const& type, std::string const& name);
    void try_parse_instant(std::shared_ptr<action::base> &action,
        pugi::xml_node node, std::string const& type, std::string const& name);
    void try_parse_animation(std::shared_ptr<action::base> &action,
        pugi::xml_node node, std::string const& type, std::string const& name);
    std::shared_ptr<animation> parse_animation(pugi::xml_node node);
    pose parse_pose(pugi::xml_node node);
    bool parse_hotspot(pugi::xml_node node, shijima::hotspot &hotspot);
    std::map<std::string, std::string> all_attributes(pugi::xml_node node,
        std::map<std::string, std::string> const& defaults = {});
    std::shared_ptr<action::base> parse_action(pugi::xml_node action, bool is_child);
    behavior::list parse_behavior_list(pugi::xml_node node, bool allow_references);
    void parse_actions(std::string const& actions);
    void parse_behaviors(std::string const& behaviors);
    void connect_actions(behavior::list &behaviors);
    pugi::xml_document load_xml(std::string const& xml);
    void fail(std::string const& what);
    void warn(std::string const& what);
    void register_image_anchor(std::string const& image_name,
        std::string const& anchor);
    void push_trace(std::string const& msg);
    void pop_trace();
#endif // !defined(SHIJIMA_NO_PUGIXML)
    std::vector<std::shared_ptr<action::reference>> action_refs;
    std::vector<std::shared_ptr<behavior::base>> behavior_refs;
    std::map<std::string, std::shared_ptr<action::base>> actions;
    std::map<std::string, std::string> image_anchors;
    std::vector<std::string> parser_trace;
    std::vector<parser_error> warnings;
public:
    behavior::list behavior_list;
    std::set<shijima::pose> poses;
    std::map<std::string, std::string> constants;
    const std::vector<parser_error> &get_warnings() const { return warnings; }
    parser() {}
    
#if !defined(SHIJIMA_NO_PUGIXML)

    void parse(std::string const& actions_xml, std::string const& behaviors_xml);

    // save parsed objects to output stream
    void saveTo(std::ostream &out);

#endif // !defined(SHIJIMA_NO_PUGIXML)

    // load parsed objects from output stream
    void loadFrom(std::istream &in);

    template<class Archive>
    void serialize(Archive &ar) {
        //NOTE: pose set is *not* serialized
        ar(behavior_list);
    }
};

}
