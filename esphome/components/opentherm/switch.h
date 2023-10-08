#pragma once

#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"
#include "esphome/core/log.h"

namespace esphome {
namespace opentherm {

enum OpenthermSwitchMode {
    OPENTHERM_SWITCH_RESTORE_DEFAULT_ON,
    OPENTHERM_SWITCH_RESTORE_DEFAULT_OFF,
    OPENTHERM_SWITCH_START_ON,
    OPENTHERM_SWITCH_START_OFF
};

class OpenthermSwitch : public switch_::Switch, public Component {
protected:
    void write_state(bool state) override;

public:
    // DEPRECATED — use restore_mode on base switch!
    void set_mode(OpenthermSwitchMode mode);

    void setup() override;
    void dump_config() override;
};

} // namespace opentherm
} // namespace esphome
