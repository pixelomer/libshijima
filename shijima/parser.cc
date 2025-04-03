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

#include "parser.hpp"
#include "action/action.hpp"
#include "animation.hpp"
#include <iostream>
#include <map>
#include <functional>
#include <memory>
#include "behavior/behavior.hpp"
#include "shijima/log.hpp"
#include "shijima/math.hpp"
#include "translator.hpp"
#include <stdexcept>

namespace shijima {

pose parser::parse_pose(pugi::xml_node node) {
    if (std::string(node.name()) != "Pose") {
        throw std::invalid_argument("Expected Pose node");
    }
    auto attr = all_attributes(node);
    if (!node.first_child().empty()) {
        throw std::invalid_argument("Non-empty Pose contents");
    }
    std::string image, anchor, sound, image_right, velocity;
    if (attr.count("Image") == 1) {
        image = attr.at("Image");
    }
    if (attr.count("ImageRight") == 1) {
        image_right = attr.at("ImageRight");
    }
    if (attr.count("ImageAnchor") == 1) {
        anchor = attr.at("ImageAnchor");
    }
    if (attr.count("Sound") == 1) {
        sound = attr.at("Sound");
    }
    if (attr.count("Velocity") == 1) {
        velocity = attr.at("Velocity");
    }
    shijima::pose pose { image, image_right, sound, anchor,
        velocity, (int)std::strtol(attr.at("Duration").c_str(),
        nullptr, 10) };
    poses.insert(pose);
    return pose;
}

bool parser::parse_hotspot(pugi::xml_node node, shijima::hotspot &hotspot) {
    if (std::string(node.name()) != "Hotspot") {
        throw std::invalid_argument("Expected Hotspot node");
    }
    auto attr = all_attributes(node);
    if (!node.first_child().empty()) {
        throw std::invalid_argument("Non-empty Hotspot contents");
    }
    hotspot::shape shape = hotspot::shape_from_name(attr.at("Shape"));
    if (shape == hotspot::shape::INVALID) {
        #ifdef SHIJIMA_LOGGING_ENABLED
            log(SHIJIMA_LOG_WARNINGS, "warning: invalid shape");
        #endif
        return false;
    }
    math::vec2 origin = attr.at("Origin");
    math::vec2 size = attr.at("Size");
    std::string behavior;
    if (attr.count("Behavior")) {
        behavior = attr.at("Behavior");
    }
    if (behavior == "") {
        #ifdef SHIJIMA_LOGGING_ENABLED
            log(SHIJIMA_LOG_WARNINGS, "warning: hotspot without behavior");
        #endif
        return false;
    }
    hotspot = { shape, origin, size, behavior };
    return true;
}

// Parse Animations within Actions
std::shared_ptr<animation> parser::parse_animation(pugi::xml_node node) {
    if (std::string(node.name()) != "Animation") {
        #ifdef SHIJIMA_LOGGING_ENABLED
            log(SHIJIMA_LOG_PARSER, "warning: ignoring invalid node in"
                " animation action: " + std::string(node.name()));
        #endif
        return nullptr;
        //throw std::invalid_argument("Expected Animation node");
    }
    auto condition_attr = node.attribute("Condition");
    scripting::condition cond = true;
    if (!condition_attr.empty()) {
        std::string js = condition_attr.value();
        cond = js;
    }
    std::vector<pose> poses;
    std::vector<hotspot> hotspots;
    auto subnode = node.first_child();
    while (!subnode.empty()) {
        if (std::string(subnode.name()) == "Hotspot") {
            shijima::hotspot hotspot;
            if (parse_hotspot(subnode, hotspot)) {
                hotspots.push_back(hotspot);
            }
        }
        else {
            auto pose = parse_pose(subnode);
            poses.push_back(pose);
        }
        subnode = subnode.next_sibling();
    }
    if (poses.size() == 0) {
        throw std::invalid_argument("Animation has no Poses");
    }
    auto anim = std::make_shared<animation>(poses, hotspots);
    anim->condition = cond;
    return anim;
}

// Parse Actions with inner Actions and ActionReferences.
void parser::try_parse_sequence(std::shared_ptr<action::base> &action,
    pugi::xml_node node, std::string const& type)
{
    static const std::map<std::string,
        std::function<std::shared_ptr<action::sequence>()>> sequence_init =
    {
        #define pair(name, type) { name, []{ return std::make_shared<type>(); } }
        pair("Select", action::select),
        pair("Sequence", action::sequence)
        #undef pair
    };
    if (sequence_init.count(type) == 1) {
        std::shared_ptr<action::sequence> seq = sequence_init.at(type)();
        auto sub_action = node.first_child();
        while (!sub_action.empty()) {
            seq->actions.push_back(parse_action(sub_action, true));
            sub_action = sub_action.next_sibling();
        }
        if (seq->actions.size() == 0) {
            throw std::invalid_argument("Sequence has no Actions");
        }
        action = seq;
    }
}

// Parse Actions with no contents. Behavior is determined solely
// through attributes.
void parser::try_parse_instant(std::shared_ptr<action::base> &action,
    pugi::xml_node node, std::string const& type)
{
    static const std::map<std::string,
        std::function<std::shared_ptr<action::instant>()>> instant_init =
    {
        #define pair(name, type) { name, []{ return std::make_shared<type>(); } }
        pair("Offset", action::offset),
        pair("Look", action::look)
        #undef pair
    };
    if (instant_init.count(type) == 1) {
        action = instant_init.at(type)();
        if (!node.first_child().empty()) {
            throw std::invalid_argument("Instant action with non-empty contents");
        }
    }
}

// Parse Actions with Animation content.
void parser::try_parse_animation(std::shared_ptr<action::base> &action,
    pugi::xml_node node, std::string const& type)
{
    static const std::map<std::string,
        std::function<std::shared_ptr<action::animation>()>> animation_init =
    {
        #define pair(name, type) { name, []{ return std::make_shared<type>(); } }
        pair("Jump", action::jump),
        pair("Animate", action::animate),
        pair("Broadcast", action::animate),
        pair("Breed", action::breed),
        pair("Dragged", action::dragged),
        pair("Regist", action::resist), // not a typo
        pair("Stay", action::stay),
        pair("BroadcastStay", action::stay),
        pair("Move", action::move),
        pair("Turn", action::turn),
        pair("MoveWithTurn", action::movewithturn),
        pair("BroadcastMove", action::move),
        pair("Fall", action::fall),
        pair("ScanMove", action::scanmove),
        pair("Interact", action::interact),
        pair("SelfDestruct", action::selfdestruct),
        pair("Transform", action::transform),

        //FIXME: Unimplemented types
        pair("FallWithIE", action::fall),
        pair("WalkWithIE", action::move),
        pair("ThrowIE", action::animate)
        #undef pair
    };
    if (animation_init.count(type) == 1) {
        std::shared_ptr<action::animation> anim_action = animation_init.at(type)();
        auto anim_node = node.first_child();
        while (!anim_node.empty()) {
            auto anim = this->parse_animation(anim_node);
            if (anim != nullptr) {
                anim_action->animations.push_back(anim);
            }
            anim_node = anim_node.next_sibling();
        }
        action = anim_action;
    }
}

// Returns all attributes for the given XML node.
std::map<std::string, std::string> parser::all_attributes(pugi::xml_node node,
    std::map<std::string, std::string> const& defaults)
{
    std::map<std::string, std::string> map;
    auto attribute = node.first_attribute();
    while (!attribute.empty()) {
        std::string name(attribute.name());
        if (map.count(name) != 0) {
            throw std::invalid_argument("Duplicate attribute: " + name);
        }
        map[name] = attribute.value();
        attribute = attribute.next_attribute();
    }
    for (auto const& pair : defaults) {
        if (map.count(pair.first) == 0) {
            map[pair.first] = pair.second;
        }
    }
    return map;
}

std::shared_ptr<action::base> parser::parse_action(pugi::xml_node action, bool is_child) {
    std::string node_name(action.name());
    auto attributes = all_attributes(action);

    if (is_child && node_name == "ActionReference") {
        // references will be linked in parse_actions() because an
        // ActionReference may be referencing an Action that comes after
        // itself
        auto ref = std::make_shared<action::reference>();
        action_refs.push_back(ref);
        ref->init_attr = attributes;
        return ref;
    }
    if (node_name != "Action") {
        throw std::invalid_argument("Expected Action node, got " + node_name);
    }

    auto type = attributes.at("Type");
    
    std::shared_ptr<action::base> result;

    if (type == "Embedded") {
        auto cls = attributes.at("Class");
        const std::string prefix = "com.group_finity.mascot.action.";
        if (cls.substr(0, prefix.size()) != prefix) {
            throw std::invalid_argument("Invalid class name");
        }
        type = cls.substr(prefix.size());
    }

    try_parse_sequence(result, action, type);
    try_parse_instant(result, action, type);
    try_parse_animation(result, action, type);

    if (result == nullptr) {
        throw std::invalid_argument("Unrecognized type: " + type);
    }
    
    result->init_attr = attributes;
    if (!is_child) {
        auto name = attributes.at("Name");
        actions[name] = result;
    }

    return result;
}

void parser::parse_actions(std::string const& actions_xml) {
    std::string translated_xml = translator::translate(actions_xml);
    pugi::xml_document doc;
    doc.load_string(actions_xml.c_str(), pugi::parse_default);
    auto mascot = doc.child("Mascot");
    if (mascot == nullptr) {
        throw std::invalid_argument("Root node is not named Mascot");
    }
    auto action_list = mascot.child("ActionList");
    while (!action_list.empty()) {
        auto action = action_list.first_child();
        while (!action.empty()) {
            parse_action(action, false);
            action = action.next_sibling();
        }
        action_list = action_list.next_sibling("ActionList");
    }
    bool linked = true;
    for (auto &ref : action_refs) {
        auto &target_name = ref->init_attr.at("Name");
        if (actions.count(target_name) == 0) {
            linked = false;
            std::cerr << "Referenced unknown action: " << target_name << std::endl;
            continue;
        }
        ref->target = actions.at(target_name);
    }
    if (!linked) {
        throw std::invalid_argument("Failed to link ActionReferences");
    }
}

behavior::list parser::parse_behavior_list(pugi::xml_node root,
    bool allow_references)
{
    behavior::list list;
    auto node = root.first_child();
    while (!node.empty()) {
        std::string name = node.name();
        if (name == "Behavior" || name == "BehaviorReference") {
            bool reference = (name == "BehaviorReference");
            if (reference && !allow_references) {
                throw std::logic_error("allow_references == false");
            }
            auto attr = all_attributes(node, {{ "Name", "" }, { "Condition", "true" },
                { "Hidden", "false" }, { "Frequency", "0" }});
            int freq = std::stoi(attr.at("Frequency"));
            bool hidden = (attr.at("Hidden") == "true");
            auto behavior = std::make_shared<behavior::base>(attr.at("Name"),
                freq, hidden, attr.at("Condition"));
            auto subnode = node.child("NextBehaviorList");
            if (!subnode.empty()) {
                auto add_attr = subnode.attribute("Add");
                bool add = true;
                if (!add_attr.empty()) {
                    add = (std::string(add_attr.value()) == "true");
                }
                behavior->next_list = std::make_unique<behavior::list>(
                    parse_behavior_list(subnode, true));
                behavior->add_next = add;
                subnode = subnode.next_sibling("NextBehaviorList");
                if (!subnode.empty()) {
                    throw std::invalid_argument("Multiple NextBehaviorList nodes");
                }
            }
            list.children.push_back(behavior);
            if (reference) {
                behavior_refs.push_back(behavior);
            }
        }
        else if (name == "Condition") {
            scripting::condition cond = true;
            auto attr = node.attribute("Condition");
            if (!attr.empty()) {
                cond = std::string(attr.value());
            }
            behavior::list sublist = parse_behavior_list(node, allow_references);
            sublist.condition = cond;
            list.sublists.push_back(sublist);
        }
        else {
            #ifdef SHIJIMA_LOGGING_ENABLED
                log(SHIJIMA_LOG_PARSER, "warning: ignoring invalid behavior node: " + name);
            #endif
        }
        node = node.next_sibling();
    }
    return list;
}

void parser::connect_actions(behavior::list &behaviors) {
    for (auto &child : behaviors.children) {
        if (child->referenced != nullptr) {
            continue;
        }
        child->action = actions.at(child->name);
        if (child->next_list != nullptr) {
            // Having Behaviors within NextList is normally an invalid configuration
            // However, there are some shimeji that do it
            connect_actions(*child->next_list);
        }
    }
    for (auto &sublist : behaviors.sublists) {
        connect_actions(sublist);
    }
}

void parser::cleanup() {
    behavior_refs.clear();
    actions.clear();
    action_refs.clear();
}

void parser::parse_behaviors(std::string const& behaviors_xml) {
    std::string translated_xml = translator::translate(behaviors_xml);
    pugi::xml_document doc;
    doc.load_string(behaviors_xml.c_str());
    auto mascot = doc.child("Mascot");
    if (mascot == nullptr) {
        throw std::invalid_argument("Root node is not named Mascot");
    }
    auto node = mascot.first_child();
    while (!node.empty()) {
        std::string tag_name = node.name();
        if (tag_name == "Constant") {
            auto name_attr = node.attribute("Name");
            auto value_attr = node.attribute("Value");
            if (name_attr == nullptr || value_attr == nullptr) {
                throw std::invalid_argument("Invalid constant");
            }
            std::string name = name_attr.value();
            std::string value = value_attr.value();
            if (constants.count(name) != 0) {
                throw std::invalid_argument("Multiple constants with "
                    "same name: " + name);
            }
            constants[name] = value;
        }
        else if (tag_name == "BehaviorList") {
            behavior_list.sublists.push_back(parse_behavior_list(node, false));
        }
        else {
            throw std::invalid_argument("Invalid tag in behaviours XML: "
                + tag_name);
        }
        node = node.next_sibling();
    }

    // Build references
    for (auto &ref : behavior_refs) {
        auto target = behavior_list.find(ref->name, false);
        if (target == nullptr) {
            #ifdef SHIJIMA_LOGGING_ENABLED
                log(SHIJIMA_LOG_PARSER, "invalid behavior reference: " + ref->name);
            #endif
            target = behavior_list.find("Fall");
        }
        ref->referenced = target;
    }

    // Connect actions and behaviors
    connect_actions(behavior_list);
}

}
