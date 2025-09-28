#pragma once

#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome::opentherm {

template<class T> constexpr T read_bit(T value, uint8_t bit) { return (value >> bit) & 0x01; }

template<class T> constexpr T set_bit(T value, uint8_t bit) { return value |= (1UL << bit); }

template<class T> constexpr T clear_bit(T value, uint8_t bit) { return value &= ~(1UL << bit); }

template<class T> constexpr T write_bit(T value, uint8_t bit, uint8_t bit_value) {
  return bit_value ? set_bit(value, bit) : clear_bit(value, bit);
}

// Normal state flow for RMT: IDLE → WRITE → SENT → LISTEN → RECEIVED → IDLE
// Normal state flow for timers: IDLE → WRITE → SENT → LISTEN → READ → RECEIVED → IDLE
/// OpenTherm component state
enum OperationMode {
  IDLE = 0,  // no operation

  LISTEN = 1,    // waiting for transmission to start
  READ = 2,      // reading 32-bit data frame
  RECEIVED = 3,  // data frame received with valid start and stop bit

  WRITE = 4,  // writing data to output
  SENT = 5,   // all data written to output

  ERROR_PROTOCOL = 8,  // protocol error, can happen only during READ
  ERROR_TIMEOUT = 9,   // timeout while waiting for response from device, only during LISTEN
  ERROR_RMT = 10       // error with RMT machinery
};

/// The type of protocol error
enum ProtocolErrorType {
  /// No error
  NO_ERROR = 0,
  /// No transition in the middle of the bit
  NO_TRANSITION = 1,
  /// Start or stop bit wasn't present when expected
  INVALID_START_STOP_BIT = 2,
  /// Parity check didn't pass
  PARITY_ERROR = 3,
  /// No level change for too much time
  NO_CHANGE_TOO_LONG = 4,
  /// Interval had an invalid duration
  INVALID_DURATION = 5,
  /// Not enough data in the OpenTherm frame
  INSUFFICIENT_DATA = 6,
};

/// OpenTherm protocol message type
enum MessageType {
  READ_DATA = 0,
  READ_ACK = 4,
  WRITE_DATA = 1,
  WRITE_ACK = 5,
  INVALID_DATA = 2,
  DATA_INVALID = 6,
  UNKNOWN_DATAID = 7
};

/// OpenTherm protocol message id
enum MessageId {
  STATUS = 0,
  CH_SETPOINT = 1,
  CONTROLLER_CONFIG = 2,
  DEVICE_CONFIG = 3,
  COMMAND_CODE = 4,
  FAULT_FLAGS = 5,
  REMOTE = 6,
  COOLING_CONTROL = 7,
  CH2_SETPOINT = 8,
  CH_SETPOINT_OVERRIDE = 9,
  TSP_COUNT = 10,
  TSP_COMMAND = 11,
  FHB_SIZE = 12,
  FHB_COMMAND = 13,
  MAX_MODULATION_LEVEL = 14,
  MAX_BOILER_CAPACITY = 15,  // u8_hb - u8_lb gives min modulation level
  ROOM_SETPOINT = 16,
  MODULATION_LEVEL = 17,
  CH_WATER_PRESSURE = 18,
  DHW_FLOW_RATE = 19,
  DAY_TIME = 20,
  DATE = 21,
  YEAR = 22,
  ROOM_SETPOINT_CH2 = 23,
  ROOM_TEMP = 24,
  FEED_TEMP = 25,
  DHW_TEMP = 26,
  OUTSIDE_TEMP = 27,
  RETURN_WATER_TEMP = 28,
  SOLAR_STORE_TEMP = 29,
  SOLAR_COLLECT_TEMP = 30,
  FEED_TEMP_CH2 = 31,
  DHW2_TEMP = 32,
  EXHAUST_TEMP = 33,
  FAN_SPEED = 35,
  FLAME_CURRENT = 36,
  ROOM_TEMP_CH2 = 37,
  REL_HUMIDITY = 38,
  DHW_BOUNDS = 48,
  CH_BOUNDS = 49,
  OTC_CURVE_BOUNDS = 50,
  DHW_SETPOINT = 56,
  MAX_CH_SETPOINT = 57,
  OTC_CURVE_RATIO = 58,

  // HVAC Specific Message IDs
  HVAC_STATUS = 70,
  REL_VENT_SETPOINT = 71,
  DEVICE_VENT = 74,
  HVAC_VER_ID = 75,
  REL_VENTILATION = 77,
  REL_HUMID_EXHAUST = 78,
  EXHAUST_CO2 = 79,
  SUPPLY_INLET_TEMP = 80,
  SUPPLY_OUTLET_TEMP = 81,
  EXHAUST_INLET_TEMP = 82,
  EXHAUST_OUTLET_TEMP = 83,
  EXHAUST_FAN_SPEED = 84,
  SUPPLY_FAN_SPEED = 85,
  REMOTE_VENTILATION_PARAM = 86,
  NOM_REL_VENTILATION = 87,
  HVAC_NUM_TSP = 88,
  HVAC_IDX_TSP = 89,
  HVAC_FHB_SIZE = 90,
  HVAC_FHB_IDX = 91,

  RF_SIGNAL = 98,
  DHW_MODE = 99,
  OVERRIDE_FUNC = 100,

  // Solar Specific Message IDs
  SOLAR_MODE_FLAGS = 101,  // hb0-2 Controller storage mode
                           // lb0   Device fault
                           // lb1-3 Device mode status
                           // lb4-5 Device status
  SOLAR_ASF = 102,
  SOLAR_VERSION_ID = 103,
  SOLAR_PRODUCT_ID = 104,
  SOLAR_NUM_TSP = 105,
  SOLAR_IDX_TSP = 106,
  SOLAR_FHB_SIZE = 107,
  SOLAR_FHB_IDX = 108,
  SOLAR_STARTS = 109,
  SOLAR_HOURS = 110,
  SOLAR_ENERGY = 111,
  SOLAR_TOTAL_ENERGY = 112,

  FAILED_BURNER_STARTS = 113,
  BURNER_FLAME_LOW = 114,
  OEM_DIAGNOSTIC = 115,
  BURNER_STARTS = 116,
  CH_PUMP_STARTS = 117,
  DHW_PUMP_STARTS = 118,
  DHW_BURNER_STARTS = 119,
  BURNER_HOURS = 120,
  CH_PUMP_HOURS = 121,
  DHW_PUMP_HOURS = 122,
  DHW_BURNER_HOURS = 123,
  OT_VERSION_CONTROLLER = 124,
  OT_VERSION_DEVICE = 125,
  VERSION_CONTROLLER = 126,
  VERSION_DEVICE = 127
};

/// Structure to hold OpenTherm data packet content.
/// Use get_f88(), get_u16() or get_s16() functions to get the appropriate value for the message id.
struct OpenthermData {
  uint8_t type;
  uint8_t id;
  uint8_t valueHB;
  uint8_t valueLB;

  OpenthermData() : type(0), id(0), valueHB(0), valueLB(0) {}

  /// @return float representation of data packet value
  float get_f88();

  /// @param float number to set as value of this data packet
  void set_f88(float value);

  /// @return unsigned 16b integer representation of data packet value
  uint16_t get_u16();

  /// @param unsigned 16b integer number to set as value of this data packet
  void set_u16(uint16_t value);

  /// @return signed 16b integer representation of data packet value
  int16_t get_s16();

  /// @param signed 16b integer number to set as value of this data packet
  void set_s16(int16_t value);
};

/// Formats protocol error type as string
const char *protocol_error_to_str(ProtocolErrorType error_type);
/// Formats protocol message type as string
const char *message_type_to_str(MessageType message_type);
/// Formats component state as string
const char *operation_mode_to_str(OperationMode mode);
/// Formats message id as string
const char *message_id_to_str(MessageId id);

#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_DEBUG
/// Prints OpenTherm data frame with detailed debug information
void debug_data(const OpenthermData &data);
#else
inline void debug_data(const OpenthermData &data) {};
#endif

/// Checks parity of a value
bool check_parity(uint32_t val);

/// Base class for OpenTherm component implementation.
class OpenThermBase {
 public:
  OpenThermBase(InternalGPIOPin *in_pin, InternalGPIOPin *out_pin);

  /// Initialize pins, timer or RMT (depending on implementation).
  virtual bool initialize();

  /// Start listening for OpenTherm data frame. If data frame is received then has_message() function returns true
  /// and data frame can be retrieved by calling get_message() function.
  virtual void listen();

  /// Send out OpenTherm data frame on the output pin. Completed data transfer is indicated by is_sent() function.
  /// Error state is indicated by is_error() function.
  /// @param data OpenTherm data frame.
  virtual void send(OpenthermData &data);

  /// Stops listening for data frame or sending a data frame and resets internal state of this class.
  /// Stops all timers and unattaches all interrupts.
  virtual void stop();

  /// Prints debug information that is specific to OpenTherm component implementation.
  virtual void debug_protocol_state() const {}

  /// Use this to retrieve data frame captured by listen() function. Data frame is ready when has_message() function
  /// returns true. This function can be called multiple times until stop() is called.
  /// @param data reference to data structure to which fill the data packet data.
  /// @return true if packet was ready and was filled into data structure passed, false otherwise.
  bool get_message(OpenthermData &data) const;

  /// Use this function to check whether listen() function already captured a valid data packet.
  ///  @return true if data packet has been captured from line by listen() function.
  bool has_message() const { return mode_ == OperationMode::RECEIVED; }

  /// Use this function to check whether send() function already finished sending data frame to line.
  bool is_sent() { return mode_ == OperationMode::SENT; }

  /// Indicates whether listening or sending is not in progress.
  /// That also means that no timers are running and no interrupts are attached.
  bool is_idle() { return mode_ == OperationMode::IDLE; }

  /// Indicates whether last listen() or send() operation ends up with an error. Includes both timeout and
  /// protocol errors.
  bool is_error() {
    return mode_ == OperationMode::ERROR_TIMEOUT || mode_ == OperationMode::ERROR_PROTOCOL || mode_ == ERROR_RMT;
  }

  /// Indicates whether last listen() or send() operation ends up with a *timeout* error.
  bool is_timeout() { return mode_ == OperationMode::ERROR_TIMEOUT; }

  /// Indicates whether last listen() or send() operation ends up with a *protocol* error.
  bool is_protocol_error() { return mode_ == OperationMode::ERROR_PROTOCOL; }

  /// Indicates whether a RMT-related error occurred. Only relevant when used on ESP32.
  bool is_rmt_error() { return mode_ == OperationMode::ERROR_RMT; }

  /// Indicates whether listen, read or write operation is in progress.
  bool is_active() { return mode_ == LISTEN || mode_ == READ || mode_ == WRITE; }

  /// Gets current component state.
  OperationMode get_mode() { return mode_; }

  /// Get protocol error details in case a protocol error occurred.
  ProtocolErrorType get_protocol_error_type() const { return this->error_type_; }

 protected:
  ~OpenThermBase() = default;

  InternalGPIOPin *in_pin_{};
  InternalGPIOPin *out_pin_{};

  OperationMode mode_{OperationMode::IDLE};
  ProtocolErrorType error_type_{ProtocolErrorType::NO_ERROR};
  uint32_t data_{};
};

}  // namespace esphome::opentherm
