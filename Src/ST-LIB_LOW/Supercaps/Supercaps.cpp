#include "ST-LIB_LOW/Supercaps/Supercaps.hpp"

void Module::set_module_states(uint16_t module_state){
   is_active_cell_measuring = module_state & ACTIVE_CELL_MEASURE;
   is_cell_balancing        = module_state & ACTIVE_CELL_SYMM;
   has_warning              = module_state & ACTIVE_WARNING;
   has_error                = module_state & ACTIVE_ERROR;
}

void Module::set_operating_mode(AcceptAutoTX accept_tx,StatusModule query_status_module,
                        StatusCells query_status_cell,StatusDetail query_status_detail,
                        CellVoltages query_cell_voltages)
{
    operating_mode = static_cast<uint8_t>(accept_tx) + 
                     static_cast<uint8_t>(query_status_module) * 2 + 
                     static_cast<uint8_t>(query_status_cell)   * 4 + 
                     static_cast<uint8_t>(query_status_detail) * 8 + 
                     static_cast<uint8_t>(query_cell_voltages) * 16;
}