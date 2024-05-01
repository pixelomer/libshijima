#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <rapidxml/rapidxml.hpp>
#include <set>
#include "animation.hpp"
#include "action/reference.hpp"

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
    std::vector<std::shared_ptr<action::reference>> references;
    static std::map<std::string, std::string> all_attributes(rapidxml::xml_node<> *node);
    std::shared_ptr<action::base> parse_action(rapidxml::xml_node<> *action, bool is_child);
    void parse_actions(std::string const& actions);
    void parse_behaviors(std::string const& behaviors);
public:
    std::map<std::string, std::shared_ptr<action::base>> actions;
    std::set<std::string> resources;
    parser() {}
    void parse(std::string const& actions, std::string const& behaviors) {
        parse_actions(actions);
        parse_behaviors(behaviors);
    }
};

}