/*
 * OpenTherm protocol implementation. Originally taken from https://github.com/jpraus/arduino-opentherm, but
 * heavily modified to comply with ESPHome coding standards and provide better logging.
 * Original code is licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International
 * Public License, which is compatible with GPLv3 license, which covers C++ part of ESPHome project.
 */

#pragma once
#ifdef USE_ESP32
#include <soc/soc_caps.h>
#if SOC_RMT_SUPPORTED

#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "opentherm_base.h"

#include <driver/rmt_rx.h>
#include <driver/rmt_tx.h>

namespace esphome::opentherm {

/// RMT implementation of OpenTherm protocol for ESP32.
class OpenTherm final : public OpenThermBase {
 public:
  OpenTherm(InternalGPIOPin *in_pin, InternalGPIOPin *out_pin);

  bool initialize() override;

  void listen() override;

  void send(OpenthermData &data) override;

  void stop() override;

  void log_protocol_state() const override;

 private:
  // RMT resources
  rmt_channel_handle_t rx_channel_{};
  rmt_channel_handle_t tx_channel_{};
  rmt_receive_config_t rx_config_{};
  rmt_encoder_handle_t tx_encoder_{};

  // One OpenTherm frame contains 34 Manchester symbols (start + 32 data + stop).
  // Keep a little slack for diagnostic captures.
  static constexpr size_t RMT_SYMBOL_CAPACITY = 40;
  rmt_symbol_word_t rmt_buffer_[RMT_SYMBOL_CAPACITY]{};
  size_t rmt_buffer_symbol_count_{};

  // RMT clock resolution in Hz (1 MHz => 1 tick == 1 us)
  static constexpr uint32_t RMT_RESOLUTION_HZ = 1000000u;

  uint8_t bit_idx_ = 0;

  bool rmt_init_();
  void rmt_read_();
  void rmt_write_();
  bool decode_rmt_symbols_(size_t num_symbols);

  static bool rmt_read_callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *evt, void *arg);
  static bool rmt_write_callback(rmt_channel_handle_t channel, const rmt_tx_done_event_data_t *evt, void *arg);

  void set_protocol_error_(ProtocolErrorType error_type);
};

}  // namespace esphome::opentherm
#endif  // SOC_RMT_SUPPORTED
#endif  // USE_ESP32
