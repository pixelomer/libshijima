#include "translator.hpp"
#include "xml_doc.hpp"
#include <sstream>
#include <rapidxml/rapidxml_print.hpp>

namespace shijima {

// shimeji xml tag and attribute names by Kilkakon
// (note: replaced "Behaviour" with "Behavior", added required behaviors)
const std::map<std::string, std::string> translator::map = {
    { "動作リスト", "ActionList" },
    { "動作", "Action" },
    { "動作参照", "ActionReference" },
    { "名前", "Name" },
    { "値", "Value" },
    { "種類", "Type" },
    { "クラス", "Class" },
    { "組み込み", "Embedded" },
    { "移動", "Move" },
    { "静止", "Stay" },
    { "固定", "Animate" },
    { "複合", "Sequence" },
    { "選択", "Select" },
    { "枠", "BorderType" },
    { "天井", "Ceiling" },
    { "壁", "Wall" },
    { "地面", "Floor" },
    { "目的地X", "TargetX" },
    { "目的地Y", "TargetY" },
    { "速度", "VelocityParam" },
    { "初速X", "InitialVX" },
    { "初速Y", "InitialVY" },
    { "重力", "Gravity" },
    { "空気抵抗X", "ResistanceX" },
    { "空気抵抗Y", "ResistanceY" },
    { "右向き", "LookRight" },
    { "IEの端X", "IeOffsetX" },
    { "IEの端Y", "IeOffsetY" },
    //{ "X", "X" },
    //{ "Y", "Y" },
    { "生まれる場所X", "BornX" },
    { "生まれる場所Y", "BornY" },
    { "生まれた時の行動", "BornBehavior" },
    { "行動", "Behavior" },
    { "繰り返し", "Loop" },
    { "アニメーション", "Animation" },
    { "条件", "Condition" },
    { "画像", "Image" },
    { "基準座標", "ImageAnchor" },
    { "移動速度", "Velocity" },
    { "長さ", "Duration" },
    { "行動リスト", "BehaviorList" },
    { "マウスの周りに集まる", "ChaseMouse" },
    { "頻度", "Frequency" },
    { "定数", "Constant" },
    { "次の行動リスト", "NextBehaviorList" },
    { "追加", "Add" },
    { "行動参照", "BehaviorReference" },
    { "落下する", "Fall" },
    { "ドラッグされる", "Dragged" },
    { "投げられる", "Thrown" },
    { "端X", "OffsetX" },
    { "端Y", "OffsetY" },
    { "マスコット", "Mascot" },
    { "ポーズ", "Pose" },
    { "落下する", "Fall" },
    { "ドラッグされる", "Dragged" },
    { "投げられる", "Thrown" }
};

void translator::translate(xml_node<> *root) {
    auto node = root->first_node();
    while (node != nullptr) {
        std::string str = node->name();
        if (map.count(str) == 1) {
            node->name(map.at(str).c_str());
        }
        auto attr = node->first_attribute();
        while (attr != nullptr) {
            str = attr->name();
            if (map.count(str) == 1) {
                attr->name(map.at(str).c_str());
            }
            str = attr->value();
            if (map.count(str) == 1) {
                attr->value(map.at(str).c_str());
            }
            attr = attr->next_attribute();
        }
        translate(node);
        node = node->next_sibling();
    }
}

std::string translator::translate(std::string const& xml) {
    xml_doc(doc, xml, 0);
    translate(&doc);
    std::string ret;
    rapidxml::print(std::back_inserter(ret), doc, 0);
    return ret;
}

};