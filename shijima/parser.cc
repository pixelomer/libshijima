#include "parser.hpp"
#include "action/action.hpp"
#include "animation.hpp"
#include <iostream>
#include <map>
#include <functional>
#include <memory>
#include "behavior/behavior.hpp"
#include "xml_doc.hpp"
#include "translator.hpp"

namespace shijima {

pose parser::parse_pose(xml_node<> *node) {
    if (std::string(node->name()) != "Pose") {
        throw std::invalid_argument("Expected Pose node");
    }
    auto attr = all_attributes(node);
    if (node->first_node() != nullptr) {
        throw std::invalid_argument("Non-empty Pose contents");
    }
    std::string image, anchor;
    if (attr.count("Image") == 1) {
        image = attr.at("Image");
    }
    if (attr.count("ImageAnchor") == 1) {
        anchor = attr.at("ImageAnchor");
    }
    shijima::pose pose { image, anchor,
        attr.at("Velocity"), (int)std::strtol(attr.at("Duration").c_str(),
        nullptr, 10) };
    poses.insert(pose);
    return pose;
}

// Parse Animations within Actions
std::shared_ptr<animation> parser::parse_animation(rapidxml::xml_node<> *node) {
    if (std::string(node->name()) != "Animation") {
        throw std::invalid_argument("Expected Animation node");
    }
    auto condition_attr = node->first_attribute("Condition");
    scripting::condition cond = true;
    if (condition_attr != nullptr) {
        std::string js = condition_attr->value();
        cond = js;
    }
    std::vector<pose> poses;
    auto pose_node = node->first_node();
    while (pose_node != nullptr) {
        //FIXME: Hotspots are ignored
        if (std::string(pose_node->name()) == "Hotspot") {
            pose_node = pose_node->next_sibling();
            continue;
        }
        auto pose = parse_pose(pose_node);
        poses.push_back(pose);
        pose_node = pose_node->next_sibling();
    }
    if (poses.size() == 0) {
        throw std::invalid_argument("Animation has no Poses");
    }
    auto anim = std::make_shared<animation>(poses);
    anim->condition = cond;
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
        pair("BroadcastMove", action::move),
        pair("Fall", action::fall),
        pair("ScanMove", action::scanmove),
        pair("Interact", action::interact),
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
std::map<std::string, std::string> parser::all_attributes(xml_node<> *node,
    std::map<std::string, std::string> const& defaults)
{
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
    for (auto const& pair : defaults) {
        if (map.count(pair.first) == 0) {
            map[pair.first] = pair.second;
        }
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
    xml_doc(doc, translated_xml, parse_no_data_nodes | parse_no_element_values);
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

behavior::list parser::parse_behavior_list(rapidxml::xml_node<> *root,
    bool allow_references)
{
    behavior::list list;
    auto node = root->first_node();
    while (node != nullptr) {
        std::string name = node->name();
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
            auto subnode = node->first_node("NextBehaviorList");
            if (subnode != nullptr) {
                auto add_attr = subnode->first_attribute("Add");
                bool add = true;
                if (add_attr != nullptr) {
                    add = (std::string(add_attr->value()) == "true");
                }
                behavior->next_list = std::make_unique<behavior::list>(
                    parse_behavior_list(subnode, true));
                behavior->add_next = add;
                subnode = subnode->next_sibling("NextBehaviorList");
                if (subnode != nullptr) {
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
            auto attr = node->first_attribute("Condition");
            if (attr != nullptr) {
                cond = std::string(attr->value());
            }
            behavior::list sublist = parse_behavior_list(node, allow_references);
            sublist.condition = cond;
            list.sublists.push_back(sublist);
        }
        else {
            throw std::invalid_argument("Invalid node: " + name);
        }
        node = node->next_sibling();
    }
    return list;
}

void parser::connect_actions(behavior::list &behaviors) {
    for (auto &child : behaviors.children) {
        child->action = actions.at(child->name);
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
    xml_doc(doc, translated_xml, parse_no_data_nodes | parse_no_element_values);
    auto mascot = doc.first_node("Mascot");
    if (mascot == nullptr) {
        throw std::invalid_argument("Root node is not named Mascot");
    }
    auto node = mascot->first_node();
    while (node != nullptr) {
        std::string tag_name = node->name();
        if (tag_name == "Constant") {
            auto name_attr = node->first_attribute("Name");
            auto value_attr = node->first_attribute("Value");
            if (name_attr == nullptr || value_attr == nullptr) {
                throw std::invalid_argument("Invalid constant");
            }
            std::string name = name_attr->value();
            std::string value = value_attr->value();
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
        node = node->next_sibling();
    }

    // Build references
    for (auto &ref : behavior_refs) {
        auto target = behavior_list.find(ref->name);
        ref->referenced = target;
    }

    // Connect actions and behaviors
    connect_actions(behavior_list);
}

}