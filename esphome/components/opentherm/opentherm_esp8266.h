#pragma once

#ifdef ESP8266

#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "opentherm_base.h"

namespace esphome::opentherm {

/// Timer-based implementation of the OpenTherm protocol for ESP8266.
class OpenTherm final : public OpenThermBase {
 public:
  OpenTherm(InternalGPIOPin *in_pin, InternalGPIOPin *out_pin);

  bool initialize() override;

  void listen() override;

  void send(OpenthermData &data) override;

  void stop() override;

  void debug_protocol_state() const override;

  static bool timer_isr(OpenTherm *arg);
  static void esp8266_timer_isr();

 private:
  ISRInternalGPIOPin isr_in_pin_;
  ISRInternalGPIOPin isr_out_pin_;

  uint32_t capture_{};
  uint8_t clock_{};
  uint8_t bit_pos_{};
  int32_t timeout_counter_ = -1;  // <0 no timeout

  static constexpr int32_t DEVICE_TIMEOUT = 800;
  static constexpr uint8_t STOP_BIT_POSITION = 33;

  void stop_timer_();

  void read_();               // data detected start reading
  void start_read_timer_();   // reading timer_ to sample at 1/5 of manchester code bit length (at 5kHz)
  void start_write_timer_();  // writing timer_ to send manchester code (at 2kHz)
  void bit_read_(uint8_t value);
  ProtocolErrorType verify_stop_bit_(uint8_t value);
  void write_bit_(uint8_t high, uint8_t clock);

  // ESP8266 timer can accept callback with no parameters, so we have this hack to save a static instance of OpenTherm
  static OpenTherm *instance;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
};

}  // namespace esphome::opentherm
#endif  // ESP8266
