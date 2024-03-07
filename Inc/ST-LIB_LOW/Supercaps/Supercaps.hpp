#pragma once

#include "C++Utilities/CppUtils.hpp"
#include "HALAL/Services/Communication/FDCAN/FDCAN.hpp"

#define MODULE_CELLS 48

enum ProtocolNumber : uint8_t {
  DATA_REQUEST = 0,
  MODULE_STATE = 1,
  CELL_STATE = 2,
  STATE_DETAIL = 3,
  CELL_VOLTAGES = 4
};

enum ModuleID { MODULE1 = 1 };
enum ErrorCode : uint8_t {

};

enum ModuleStateMask {
  ACTIVE_CELL_MEASURE = 0b00000001,
  ACTIVE_CELL_SYMM = 0b00000010,
  ACTIVE_WARNING = 0b00000100,
  ACTIVE_ERROR = 0b00001000
};
enum class AcceptAutoTX : uint8_t { unset = 0, set = 1 };
enum class StatusModule : uint8_t { unset = 0, set = 1 };
enum class StatusCells : uint8_t { unset = 0, set = 1 };
enum class StatusDetail : uint8_t { unset = 0, set = 1 };

enum class CellVoltages : uint8_t { unset = 0, set = 1 };

struct Module {
  uint16_t module_voltage;
  int8_t highest_cell_temperature;
  int8_t lowest_cell_temperature;
  ErrorCode last_error;
  uint8_t transfer_count;
  uint16_t max_cell_voltage;
  uint16_t min_cell_voltage;
  uint16_t avg_cell_voltage;
  uint8_t cell_with_max_voltage;
  uint8_t cell_with_min_voltage;
  // for use to address cells, msg come in chunks of 3 cells
  uint8_t msg_number;
  std::array<uint16_t, MODULE_CELLS> cell_voltages;
  uint16_t operating_mode;

  bool is_active_cell_measuring;
  bool is_cell_balancing;
  bool has_warning;
  bool has_error;
  __attribute__((always_inline)) uint16_t query_configuration() {
    return operating_mode;
  }

  void set_module_states(uint16_t module_state);
  void set_operating_mode(AcceptAutoTX accept_tx,
                          StatusModule query_status_module,
                          StatusCells query_status_cell,
                          StatusDetail query_status_detail,
                          CellVoltages query_cell_voltages);
};

class Supercaps {
public:
  Supercaps() {
    // by default use fdcan1
    fdcan_instance = FDCAN::inscribe(FDCAN::fdcan1);
  }

  void send_protocol_to_module(ProtocolNumber protocol, ModuleID id) {
    uint32_t identifier = protocol * 256 + 1 * 16 + id;
    array<uint8_t, 8> data{0, 1, 2, 3, 4, 5, 6, 7};
    FDCAN::transmit(fdcan_instance, identifier, data, FDCAN::DLC::BYTES_8);
  }

private:
  uint8_t fdcan_instance;
};
