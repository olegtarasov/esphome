#include "switch.h"

namespace esphome {
namespace opentherm {

void OpenthermSwitch::write_state(bool state) {
    this->publish_state(state);
}

void OpenthermSwitch::setup() {
    this->write_state(this->get_initial_state_with_restore_mode());
}

void OpenthermSwitch::set_mode(esphome::opentherm::OpenthermSwitchMode mode) {
    esphome::switch_::SwitchRestoreMode target_mode = switch_::SWITCH_ALWAYS_OFF;
    switch (mode) {
      case OpenthermSwitchMode::OPENTHERM_SWITCH_RESTORE_DEFAULT_OFF:
        target_mode = switch_::SWITCH_RESTORE_DEFAULT_OFF;
        break;
      case OpenthermSwitchMode::OPENTHERM_SWITCH_RESTORE_DEFAULT_ON:
        target_mode = switch_::SWITCH_RESTORE_DEFAULT_ON;
        break;
      case OpenthermSwitchMode::OPENTHERM_SWITCH_START_OFF:
        target_mode = switch_::SWITCH_ALWAYS_OFF;
        break;
      case OpenthermSwitchMode::OPENTHERM_SWITCH_START_ON:
        target_mode = switch_::SWITCH_ALWAYS_ON;
        break;
    }

    this->set_restore_mode(target_mode);
}

} // namespace opentherm
} // namespace esphome
