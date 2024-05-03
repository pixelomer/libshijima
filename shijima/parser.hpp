#pragma once
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

namespace shijima {

class parser {
private:
    void try_parse_sequence(std::shared_ptr<action::base> &action,
        rapidxml::xml_node<> *node, std::string const& type);
    void try_parse_instant(std::shared_ptr<action::base> &action,
        rapidxml::xml_node<> *node, std::string const& type);
    void try_parse_animation(std::shared_ptr<action::base> &action,
        rapidxml::xml_node<> *node, std::string type);
    std::shared_ptr<animation> parse_animation(rapidxml::xml_node<> *node);
    pose parse_pose(rapidxml::xml_node<> *node);
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
    std::set<std::string> resources;
    parser() {}
    void parse(std::string const& actions_xml, std::string const& behaviors_xml) {
        // Clean results from any previous parse calls
        resources.clear();
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