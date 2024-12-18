#include "animate.hpp"

namespace shijima {
namespace action {

class transform : public animate {
public:
    virtual bool tick() override;
};

}
}