#include <string>
#include <rapidxml/rapidxml.hpp>
#include <map>

namespace shijima {

class translator {
private:
    static const std::map<std::string, std::string> map;
    static void translate(rapidxml::xml_node<> *root);
public:
    static std::string translate(std::string const& xml);
};

};