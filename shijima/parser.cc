#include "parser.hpp"
#include "action/action.hpp"
#include "animation.hpp"
#include <iostream>
#include <map>
#include <functional>
#include <memory>

using namespace rapidxml;

#define xml_doc(var, str) \
    xml_document<> var; \
    std::vector<char> var##_##backingVector(str.begin(), str.end()); \
    var##_##backingVector.push_back('\0'); \
    var.parse<0>((char *)&var##_##backingVector[0])

namespace shijima {

pose parser::parse_pose(xml_node<> *node) {
    if (std::string(node->name()) != "Pose") {
        throw std::invalid_argument("Expected Pose node");
    }
    auto attr = all_attributes(node);
    if (node->first_node() != nullptr) {
        throw std::invalid_argument("Non-empty Pose contents");
    }
    return shijima::pose(attr.at("Image"), attr.at("ImageAnchor"),
        attr.at("Velocity"), (int)std::strtol(attr.at("Duration").c_str(),
        nullptr, 10));
}

// Parse Animations within Actions
std::shared_ptr<animation> parser::parse_animation(rapidxml::xml_node<> *node) {
    if (std::string(node->name()) != "Animation") {
        throw std::invalid_argument("Expected Animation node");
    }
    std::vector<pose> poses;
    auto pose_node = node->first_node();
    while (pose_node != nullptr) {
        auto pose = parse_pose(pose_node);
        poses.push_back(pose);
        pose_node = pose_node->next_sibling();
    }
    if (poses.size() == 0) {
        throw std::invalid_argument("Animation has no Poses");
    }
    auto anim = std::make_shared<animation>(poses);
    return anim;
}

// Parse Actions with inner Actions and ActionReferences.
void parser::try_parse_sequence(std::shared_ptr<action::base> &action,
    rapidxml::xml_node<> *node, std::string const& type)
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
        auto sub_action = node->first_node();
        while (sub_action != nullptr) {
            seq->actions.push_back(parse_action(sub_action, true));
            sub_action = sub_action->next_sibling();
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
    rapidxml::xml_node<> *node, std::string const& type)
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
        if (node->first_node() != nullptr) {
            throw std::invalid_argument("Instant action with non-empty contents");
        }
    }
}

// Parse Actions with Animation content.
void parser::try_parse_animation(std::shared_ptr<action::base> &action,
    rapidxml::xml_node<> *node, std::string type)
{
    //FIXME: Unimplemented types
    if (type == "FallWithIE") {
        type = "Fall";
    }
    if (type == "WalkWithIE") {
        type = "Move";
    }
    if (type == "ThrowIE") {
        type = "Animate";
    }
    if (type == "Breed") {
        type = "Animate";
    }
    static const std::map<std::string,
        std::function<std::shared_ptr<action::animation>()>> animation_init =
    {
        #define pair(name, type) { name, []{ return std::make_shared<type>(); } }
        pair("Jump", action::jump),
        pair("Animate", action::animate),
        pair("Dragged", action::dragged),
        pair("Regist", action::resist), // not a typo
        pair("Stay", action::stay),
        pair("Move", action::move),
        pair("Fall", action::fall)
        #undef pair
    };
    if (animation_init.count(type) == 1) {
        std::shared_ptr<action::animation> anim_action = animation_init.at(type)();
        auto anim_node = node->first_node();
        while (anim_node != nullptr) {
            auto anim = this->parse_animation(anim_node);
            anim_action->animations.push_back(anim);
            anim_node = anim_node->next_sibling();
        }
        action = anim_action;
    }
}

// Returns all attributes for the given XML node.
std::map<std::string, std::string> parser::all_attributes(xml_node<> *node) {
    std::map<std::string, std::string> map;
    auto attribute = node->first_attribute();
    while (attribute != NULL) {
        std::string name(attribute->name());
        if (map.count(name) != 0) {
            throw std::invalid_argument("Duplicate attribute: " + name);
        }
        map[name] = attribute->value();
        attribute = attribute->next_attribute();
    }
    return map;
}

std::shared_ptr<action::base> parser::parse_action(xml_node<> *action, bool is_child) {
    std::string node_name(action->name());
    auto attributes = all_attributes(action);

    if (is_child && node_name == "ActionReference") {
        // references will be linked in parse_actions() because an
        // ActionReference may be referencing an Action that comes after
        // itself
        auto ref = std::make_shared<action::reference>();
        references.push_back(ref);
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
    actions.clear();
    references.clear();
    xml_doc(doc, actions_xml);
    auto mascot = doc.first_node("Mascot");
    if (mascot == nullptr) {
        throw std::invalid_argument("Root node is not named Mascot");
    }
    auto action_list = mascot->first_node("ActionList");
    while (action_list != nullptr) {
        auto action = action_list->first_node();
        while (action != nullptr) {
            parse_action(action, false);
            action = action->next_sibling();
        }
        action_list = action_list->next_sibling("ActionList");
    }
    bool linked = true;
    for (auto &ref : references) {
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

void parser::parse_behaviors(std::string const& behaviors) {

}

}