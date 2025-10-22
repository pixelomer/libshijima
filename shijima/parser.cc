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

// reduce serialized cereal data size by limiting size tags
#define CEREAL_SIZE_TYPE uint16_t

#include <cereal/types/map.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include "action/action.hpp"
#include <cereal/archives/portable_binary.hpp>

// these macros break clangd
#ifndef __CLANGD__

// register types for serialization/deserialization
CEREAL_REGISTER_TYPE(shijima::action::animate);
CEREAL_REGISTER_TYPE(shijima::action::animation);
CEREAL_REGISTER_TYPE(shijima::action::base);
CEREAL_REGISTER_TYPE(shijima::action::breed);
CEREAL_REGISTER_TYPE(shijima::action::breedjump);
CEREAL_REGISTER_TYPE(shijima::action::breedmove);
CEREAL_REGISTER_TYPE(shijima::action::complexjump);
CEREAL_REGISTER_TYPE(shijima::action::complexmove);
CEREAL_REGISTER_TYPE(shijima::action::dragged);
CEREAL_REGISTER_TYPE(shijima::action::fall);
CEREAL_REGISTER_TYPE(shijima::action::instant);
CEREAL_REGISTER_TYPE(shijima::action::interact);
CEREAL_REGISTER_TYPE(shijima::action::jump);
CEREAL_REGISTER_TYPE(shijima::action::look);
CEREAL_REGISTER_TYPE(shijima::action::move);
CEREAL_REGISTER_TYPE(shijima::action::movewithturn);
CEREAL_REGISTER_TYPE(shijima::action::offset);
CEREAL_REGISTER_TYPE(shijima::action::reference);
CEREAL_REGISTER_TYPE(shijima::action::resist);
CEREAL_REGISTER_TYPE(shijima::action::scaninteract);
CEREAL_REGISTER_TYPE(shijima::action::scanjump);
CEREAL_REGISTER_TYPE(shijima::action::scanmove);
CEREAL_REGISTER_TYPE(shijima::action::select);
CEREAL_REGISTER_TYPE(shijima::action::selfdestruct);
CEREAL_REGISTER_TYPE(shijima::action::sequence);
CEREAL_REGISTER_TYPE(shijima::action::stay);
CEREAL_REGISTER_TYPE(shijima::action::transform);
CEREAL_REGISTER_TYPE(shijima::action::turn);
CEREAL_REGISTER_TYPE(shijima::action::mute);

// register polymorphic relations
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animation,   shijima::action::animate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::base,        shijima::action::animation);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animate,     shijima::action::breed);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animation,   shijima::action::dragged);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animation,   shijima::action::fall);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::base,        shijima::action::instant);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animate,     shijima::action::interact);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animation,   shijima::action::jump);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::instant,     shijima::action::look);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animation,   shijima::action::move);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::move,        shijima::action::movewithturn);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::instant,     shijima::action::offset);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::base,        shijima::action::reference);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animate,     shijima::action::resist);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animate,     shijima::action::scaninteract);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::jump,        shijima::action::complexjump);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::move,        shijima::action::complexmove);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::complexjump, shijima::action::scanjump);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::complexmove, shijima::action::scanmove);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::sequence,    shijima::action::select);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animate,     shijima::action::selfdestruct);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::base,        shijima::action::sequence);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animation,   shijima::action::stay);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animate,     shijima::action::transform);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::animate,     shijima::action::turn);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::jump,        shijima::action::breedjump);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::move,        shijima::action::breedmove);
CEREAL_REGISTER_POLYMORPHIC_RELATION(shijima::action::instant,     shijima::action::mute);

#endif

#include "parser.hpp"
#include <iostream>
#include "behavior/behavior.hpp"

#if !defined(SHIJIMA_NO_PUGIXML)
#include "animation.hpp"
#include <map>
#include <functional>
#include <memory>
#include "shijima/log.hpp"
#include "shijima/math.hpp"
#include "translator.hpp"
#include <stdexcept>
#endif

namespace shijima {

#if !defined(SHIJIMA_NO_PUGIXML)

static void strip_xml(pugi::xml_node node) {
    pugi::xml_node child = node.first_child();
    while (!child.empty()) {
        if (child.type() != pugi::xml_node_type::node_element) {
            pugi::xml_node unwanted = child;
            child = child.next_sibling();
            node.remove_child(unwanted);
        }
        else {
            strip_xml(child);
            child = child.next_sibling();
        }
    }
}

void parser::register_image_anchor(std::string const& image_name,
    std::string const& anchor)
{
    if (image_name.empty() || anchor.empty()) {
        return;
    }
    if (image_anchors.count(image_name) == 1) {
        auto const& old_anchor = image_anchors.at(image_name);
        if (old_anchor != anchor) {
            warn("mismatched anchors for image \"" + image_name + "\": "
                + old_anchor + " ; " + anchor);
        }
    }
    else {
        image_anchors[image_name] = anchor;
    }
}

pose parser::parse_pose(pugi::xml_node node) {
    if (std::string(node.name()) != "Pose") {
        fail("expected Pose node, got " + std::string(node.name()));
    }
    auto attr = all_attributes(node);
    std::string image, anchor, sound, image_right, velocity;
    if (attr.count("Image") == 1) {
        image = attr.at("Image");
    }
    if (!node.first_child().empty()) {
        warn("Pose node should not contain children (image=" + image + ")");
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
    if (attr.count("Duration") == 0) {
        fail("Pose node is missing Duration attribute (image=" + image + ")");
    }
    register_image_anchor(image, anchor);
    register_image_anchor(image_right, anchor);
    std::string const& s_duration = attr.at("Duration");
    const char *duration = s_duration.c_str(), *duration_end;
    long l_duration = std::strtol(duration, (char **)&duration_end, 10);
    if (l_duration < 0 || duration_end == duration) {
        fail("Pose node has invalid Duration attribute (image=" + image +
            "): " + s_duration);
    }
    shijima::pose pose { image, image_right, sound, anchor,
        velocity, (int)l_duration };
    poses.insert(pose);
    return pose;
}

bool parser::parse_hotspot(pugi::xml_node node, shijima::hotspot &hotspot) {
    if (std::string(node.name()) != "Hotspot") {
        warn("expected Hotspot node, got " + std::string(node.name()));
        return false;
    }
    auto attr = all_attributes(node);
    if (!node.first_child().empty()) {
        warn("Hotspot node should not contain children");
    }
    std::string shape_name;
    bool fail = false;
    if (attr.count("Shape") == 1) {
        shape_name = attr.at("Shape");
    }
    else {
        warn("Hotspot node must contain Shape attribute");
        fail = true;
    }
    hotspot::shape shape = hotspot::shape_from_name(shape_name);
    if (shape == hotspot::shape::INVALID) {
        warn("invalid hotspot shape name: " + shape_name);
        fail = true;
    }
    if (attr.count("Origin") == 0) {
        warn("Hotspot node must contain Origin attribute");
        fail = true;
    }
    if (attr.count("Size") == 0) {
        warn("Hotspot node must contain Size attribute");
        fail = true;
    }
    if (attr.count("Behavior") == 0) {
        warn("Hotspot node must contain Behavior attribute");
        fail = true;
    }
    if (fail) {
        return false;
    }
    math::vec2 origin = attr.at("Origin");
    math::vec2 size = attr.at("Size");
    std::string behavior = attr.at("Behavior");
    hotspot = { shape, origin, size, behavior };
    return true;
}

// Parse Animations within Actions
std::shared_ptr<animation> parser::parse_animation(pugi::xml_node node) {
    if (std::string(node.name()) != "Animation") {
        warn("expected Animation node, got: " + std::string(node.name()));
        return nullptr;
    }
    auto is_turn = node.attribute("IsTurn");
    if (!is_turn.empty() && std::string(is_turn.value()) == "true") {
        //FIXME: IsTurn is not supported
        warn("IsTurn is not supported, ignoring animation");
        return nullptr;
    }
    auto condition_attr = node.attribute("Condition");
    std::string cond = "true";
    if (!condition_attr.empty()) {
        cond = condition_attr.value();
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
        warn("animation has no poses");
        return nullptr;
    }
    auto anim = std::make_shared<animation>(poses, hotspots);
    anim->condition = cond;
    return anim;
}

// Parse Actions with inner Actions and ActionReferences.
void parser::try_parse_sequence(std::shared_ptr<action::base> &action,
    pugi::xml_node node, std::string const& type, std::string const& name)
{
    static const std::map<std::string,
        std::function<std::shared_ptr<action::sequence>()>> sequence_init =
    {
        #define pair(name, type) { name, []{ return std::make_shared<type>(); } }
        pair("Select", action::select),
        pair("Sequence", action::sequence),

        //FIXME: unimplemented types
        pair("OpenURL", action::sequence) // non-standard action used in KinitoPet
        #undef pair
    };
    if (sequence_init.count(type) == 1) {
        push_trace("parsing sequence-like action (type=" + type + ", "
            "name=" + name + ")");
        std::shared_ptr<action::sequence> seq = sequence_init.at(type)();
        auto sub_action = node.first_child();
        push_trace("parsing action");
        while (!sub_action.empty()) {
            seq->actions.push_back(parse_action(sub_action, true));
            sub_action = sub_action.next_sibling();
        }
        pop_trace();
        if (seq->actions.size() == 0) {
            fail("sequence has no actions");
        }
        action = seq;
        pop_trace();
    }
}

// Parse Actions with no contents. Behavior is determined solely
// through attributes.
void parser::try_parse_instant(std::shared_ptr<action::base> &action,
    pugi::xml_node node, std::string const& type, std::string const& name)
{
    static const std::map<std::string,
        std::function<std::shared_ptr<action::instant>()>> instant_init =
    {
        #define pair(name, type) { name, []{ return std::make_shared<type>(); } }
        pair("Offset", action::offset),
        pair("Look", action::look),
        pair("Mute", action::mute)
        #undef pair
    };
    if (instant_init.count(type) == 1) {
        push_trace("parsing instant-like action type (type=" + type + ", "
            "name=" + name + ")");
        action = instant_init.at(type)();
        if (!node.first_child().empty()) {
            warn("instant action node should not contain children");
        }
        pop_trace();
    }
}

// Parse Actions with Animation content.
void parser::try_parse_animation(std::shared_ptr<action::base> &action,
    pugi::xml_node node, std::string const& type, std::string const& name)
{
    static const std::map<std::string,
        std::function<std::shared_ptr<action::animation>()>> animation_init =
    {
        #define pair(name, type) { name, []{ return std::make_shared<type>(); } }
        pair("Jump", action::jump),
        pair("Animate", action::animate),
        pair("Broadcast", action::animate),
        pair("Breed", action::breed),
        pair("BreedJump", action::breedjump),
        pair("BreedMove", action::breedmove),
        pair("Dragged", action::dragged),
        pair("Regist", action::resist), // not a typo
        pair("Stay", action::stay),
        pair("BroadcastStay", action::stay),
        pair("Move", action::move),
        pair("Turn", action::turn),
        pair("MoveWithTurn", action::movewithturn),
        pair("BroadcastMove", action::move),
        pair("BroadcastJump", action::jump),
        pair("Fall", action::fall),
        pair("ScanMove", action::scanmove),
        pair("Interact", action::interact),
        pair("SelfDestruct", action::selfdestruct),
        pair("Transform", action::transform),
        pair("ScanInteract", action::scaninteract),
        pair("ScanJump", action::scanjump),
        pair("ComplexMove", action::complexmove),
        pair("ComplexJump", action::complexjump),

        //FIXME: Unimplemented types
        pair("FallWithIE", action::fall),
        pair("WalkWithIE", action::move),
        pair("ThrowIE", action::animate)
        #undef pair
    };
    if (animation_init.count(type) == 1) {
        push_trace("parsing animation-like action (type=" + type + ", "
            "name=" + name + ")");
        std::shared_ptr<action::animation> anim_action = animation_init.at(type)();
        auto anim_node = node.first_child();
        push_trace("parsing animation");
        while (!anim_node.empty()) {
            auto anim = this->parse_animation(anim_node);
            if (anim != nullptr) {
                anim_action->animations.push_back(anim);
            }
            anim_node = anim_node.next_sibling();
        }
        pop_trace();
        action = anim_action;
        pop_trace();
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
            warn("duplicate attribute: " + name);
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
        fail("expected Action node, got " + node_name);
    }

    std::string name;
    if (!is_child && attributes.count("Name") == 0) {
        fail("root actions must have a name, but Name attribute was missing");
    }
    name = attributes.count("Name") == 1 ? attributes.at("Name") : "(null)";

    if (attributes.count("Type") == 0) {
        fail("missing Type attribute in action");
    }
    auto type = attributes.at("Type");
    
    std::shared_ptr<action::base> result;

    // hardcoded fix for KinitoPet
    if (type == "Animate" && attributes.count("Class") == 1 &&
        attributes.at("Class") == "com.group_finity.mascot.action.Dragged")
    {
        type = "Embedded";
    }

    if (type == "Embedded") {
        auto cls = attributes.at("Class");
        static const std::string prefix = "com.group_finity.mascot.action.";
        if (cls.substr(0, prefix.size()) != prefix) {
            fail("invalid class name: " + cls);
        }
        type = cls.substr(prefix.size());
    }

    try_parse_sequence(result, action, type, name);
    try_parse_instant(result, action, type, name);
    try_parse_animation(result, action, type, name);

    if (result == nullptr) {
        fail("unrecognized action type: " + type);
    }
    
    result->init_attr = attributes;
    if (!is_child) {
        actions[name] = result;
    }

    return result;
}

void parser::fail(std::string const& what) {
    parser_error error(parser_trace, what);
    parser_trace.clear();
    throw error;
}

void parser::warn(std::string const& what) {
    parser_error error(parser_trace, what);
    warnings.push_back(error);
}

pugi::xml_document parser::load_xml(std::string const& xml) {
    std::string translated = translator::translate(xml);
    push_trace("loading translated xml");
    pugi::xml_document doc;
    auto load_result = doc.load_string(translated.c_str(), pugi::parse_default);
    if (!load_result) {
        fail("failed to load xml: " + std::string(load_result.description()) +
            " (encoding=" + std::to_string(load_result.encoding) +
            ", offset=" + std::to_string(load_result.offset) +
            ", status=" + std::to_string(load_result.status) + ")");
    }
    pop_trace();
    strip_xml(doc);
    
    return doc;
}

void parser::parse_actions(std::string const& actions_xml) {
    auto doc = load_xml(actions_xml);
    auto mascot = doc.child("Mascot");
    if (mascot == nullptr) {
        fail("root node is not named Mascot");
    }
    auto action_list = mascot.child("ActionList");
    push_trace("parsing action");
    while (!action_list.empty()) {
        auto action = action_list.first_child();
        while (!action.empty()) {
            parse_action(action, false);
            action = action.next_sibling();
        }
        action_list = action_list.next_sibling("ActionList");
    }
    pop_trace();
    bool linked = true;
    push_trace("linking actions with action references");
    for (auto &ref : action_refs) {
        auto &target_name = ref->init_attr.at("Name");
        if (actions.count(target_name) == 0) {
            linked = false;
            warn("referenced unknown action: " + target_name);
            continue;
        }
        ref->target = actions.at(target_name);
    }
    if (!linked) {
        fail("failed to link action references");
    }
    pop_trace();
}

behavior::list parser::parse_behavior_list(pugi::xml_node root,
    bool allow_references)
{
    behavior::list list;
    push_trace("parsing behavior list");
    auto node = root.first_child();
    while (!node.empty()) {
        std::string name = node.name();
        if (name == "Behavior" || name == "BehaviorReference") {
            bool reference = (name == "BehaviorReference");
            auto attr = all_attributes(node, {{ "Name", "" }, { "Condition", "true" },
                { "Hidden", "false" }, { "Frequency", "0" }});
            if (reference && !allow_references) {
                warn("BehaviorReference in unexpected location (name="
                    + attr.at("Name") + ", frequency=" + attr.at("Frequency") + ")");
                node = node.next_sibling();
                continue;
            }
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
                    fail("multiple NextBehaviorList nodes");
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
            warn("ignoring invalid behavior list subnode: " + name);
        }
        node = node.next_sibling();
    }
    pop_trace();
    return list;
}

void parser::connect_actions(behavior::list &behaviors) {
    for (auto &child : behaviors.children) {
        if (child->referenced != nullptr) {
            continue;
        }
        if (actions.count(child->name) == 0) {
            fail("behavior without matching action: " + child->name);
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

void parser::parse_behaviors(std::string const& behaviors_xml) {
    auto doc = load_xml(behaviors_xml);
    auto mascot = doc.child("Mascot");
    if (mascot == nullptr) {
        fail("root node is not named Mascot");
    }
    auto node = mascot.first_child();
    while (!node.empty()) {
        std::string tag_name = node.name();
        if (tag_name == "Constant") {
            auto name_attr = node.attribute("Name");
            auto value_attr = node.attribute("Value");
            if (name_attr == nullptr) {
                fail("constant is missing the Name attribute "
                    "(value=" + std::string(value_attr == nullptr ?
                    "(null)" : value_attr.value()) + ")");
            }
            if (value_attr == nullptr) {
                fail("constant is missing the Value attribute "
                    "(name=" + std::string(name_attr.value()) + ")");
            }
            std::string name = name_attr.value();
            std::string value = value_attr.value();
            if (constants.count(name) != 0) {
                fail("multiple constants named " + name);
            }
            constants[name] = value;
        }
        else if (tag_name == "BehaviorList") {
            behavior_list.sublists.push_back(parse_behavior_list(node, false));
        }
        else {
            fail("invalid tag name in Mascot: " + tag_name);
        }
        node = node.next_sibling();
    }

    auto fall_behavior = behavior_list.find("Fall", false);
    if (fall_behavior == nullptr) {
        fail("missing Fall behavior");
    }

    // Build references
    for (auto &ref : behavior_refs) {
        auto target = behavior_list.find(ref->name, false);
        if (target == nullptr) {
            warn("reference to non-existing behavior: " + ref->name);
            target = fall_behavior;
        }
        ref->referenced = target;
    }

    // Connect actions and behaviors
    push_trace("connecting behaviors to actions");
    connect_actions(behavior_list);
    pop_trace();
}

void parser::push_trace(std::string const& msg) {
    parser_trace.push_back(msg);
}

void parser::pop_trace() {
    parser_trace.pop_back();
}

void parser::parse(std::string const& actions_xml, std::string const& behaviors_xml) {
    // Clean results from any previous parse calls
    parser_trace.clear();
    warnings.clear();
    poses.clear();
    constants.clear();
    behavior_list = {};

    push_trace("parsing actions.xml");
    parse_actions(actions_xml);
    pop_trace();

    push_trace("parsing behaviors.xml");
    parse_behaviors(behaviors_xml);
    pop_trace();

    // Clean intermediary variables
    action_refs.clear();
    behavior_refs.clear();
    image_anchors.clear();
    actions.clear();
}

void parser::saveTo(std::ostream &out) {
    cereal::PortableBinaryOutputArchive ar { out };
    ar(*this);
}

#endif //defined(SHIJIMA_NO_PUGIXML)

void parser::loadFrom(std::istream &in) {
    cereal::PortableBinaryInputArchive ar { in };
    ar(*this);
}

}
