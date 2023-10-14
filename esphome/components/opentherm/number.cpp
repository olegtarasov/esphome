#include "number.h"

namespace esphome {
namespace opentherm {

void OpenthermNumber::control(float value) {
  this->publish_state(value);

  if (this->restore_value_)
    this->pref_.save(&value);
}

void OpenthermNumber::setup() {
  float value;
  if (!this->restore_value_) {
    value = this->initial_value_;
  } else {
    this->pref_ = global_preferences->make_preference<float>(this->get_object_id_hash());
    if (!this->pref_.load(&value)) {
      if (!std::isnan(this->initial_value_)) {
        value = this->initial_value_;
      } else {
        value = this->traits.get_min_value();
      }
    }
  }
  this->publish_state(value);
}

}
}