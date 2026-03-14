#include "HALAL/Services/DFSDM/DFSDM.hpp"


//ST_LIB::DFSDM_CHANNEL_DOMAIN::Init::

extern "C"{

void DFSDM1_FLT0_IRQHandler(void)
{
   ST_LIB::DFSDM_CHANNEL_DOMAIN::handle_irq(0);
}
void DFSDM1_FLT1_IRQHandler(void)
{
    ST_LIB::DFSDM_CHANNEL_DOMAIN::handle_irq(1);
}
void DFSDM1_FLT2_IRQHandler(void)
{
    ST_LIB::DFSDM_CHANNEL_DOMAIN::handle_irq(2);
}
void DFSDM1_FLT3_IRQHandler(void)
{
    ST_LIB::DFSDM_CHANNEL_DOMAIN::handle_irq(3);
}
}