#pragma once

#include "HALAL/Models/GPIO.hpp"
#include "HALAL/Models/Pin.hpp"


#define Oversampling_MAX 1024
#define Oversampling_MAX_Filter_4 215
#define Oversampling_MAX_Filter_5 73
#define Possible_Pin_Channel 20
#define OFFSET_MAX  (1 << 23) - 1
#define OFFSET_MIN  -(1 << 23)
using ST_LIB::GPIODomain;

namespace ST_LIB {
    using Callback = void(*)(void);
    extern void compile_error(const char *msg);

    struct DFSDM_CHANNEL_DOMAIN{
   /* Constant Values of Register*/
   //DatPack = 0 Standar
   //DatMPx = 0 // External input
   //ChinSel = 0  // It make sense for our use case.
   //SPICKSel = ¿?
   enum class SPICKSel: uint8_t{
        CLK_IN = 0, 
        NORMAL_CLK_OUT = 1,
        CLK_DIVIDED_2_RISING = 2,
        CLK_DIVIDED_2_FALLING = 3
   };
   enum class SPI_Type: uint8_t{
        SPI_RISING = 0,
        SPI_FALLING = 1
   };

    enum  class Filter_Type : uint8_t {
        FastSinc = 0,
        Sinc1    = 1,
        Sinc2    = 2,
        Sinc3    = 3,
        Sinc4    = 4,
        Sinc5    = 5
    };

    enum  Fast_Conversion : uint8_t {
        Disable = 0,
        Enable  = 1
    };

    enum  class Dma: uint8_t {
        Disable = 0,
        Enable = 1
    };

    enum  class Sync_Conversion : uint8_t {
        Independent = 0,
        Sync_With_Flt0 = 1
    };

    enum  class Regular_Mode : uint8_t {
        Single     = 0,
        Continuous = 1
    };

    enum  class Analog_Watchdog_Mode : uint8_t {
        After_Filter = 0,   // AWFSEL = 0
        Channel_Data = 1    // AWFSEL = 1
    };
    //AWFSEL = 0 high precision, slow speed <- Ideally for our case
    //AWFSEL = 1 16 bits precision ultra high speed oversampling ratio 1..32 filter (1..3) 8 relojes de clock

    enum  class Overrun : uint8_t {
        Enable = 0,
        Disable = 1
    };
    enum  class Clock_Absence : uint8_t{
        Disable = 0,
        Enable = 1
    };
    enum class Short_Circuit : uint8_t{
        Disable = 0,
        Enable = 1
    };
    enum class Extreme_Detector : uint8_t{
        Disable = 0,
        Enable = 1
    };
    enum class Analog_Watchdog : uint8_t{
        Disable = 0,
        Enable = 1
    };
    enum class Trigger_Timer_Source : uint8_t {
        Unused,
        Tim1,
        Tim3,
        Tim4,
        Tim7,
        Tim8,
        Tim16,
        Tim23,
        Tim24,
        Exti11,
        Exti15,
        Lptim1,
        Lptim2,
        Lptim3
    };
    enum class Type_Conversion: uint8_t{
        Regular,
        Injected
    };
    enum class Injected_Mode: uint8_t{
        Single,
        Scan 
    };
 struct Config_Channel {

    int32_t  offset{0};
    uint32_t right_shift{0};
    SPICKSel spi_clock_sel{SPICKSel::CLK_DIVIDED_2_RISING};
    SPI_Type spi_type{SPI_Type::SPI_RISING};
    Type_Conversion type_conv{Type_Conversion::Regular};
    Trigger_Timer_Source trigger_conv{Trigger_Timer_Source::Unused};
    Filter_Type filter_type{Filter_Type::FastSinc};
    uint16_t oversampling{1};
    uint16_t integrator{1};

    Dma dma{Dma::Disable};
    Fast_Conversion fast{Fast_Conversion::Disable};
    Sync_Conversion rsync{Sync_Conversion::Independent};
    Regular_Mode rcont{Regular_Mode::Single};
    Injected_Mode jscan{Injected_Mode::Scan};
    /* -------- Runtime protections -------- */

    Overrun overrun{Overrun::Disable};
    Clock_Absence clock_absence{Clock_Absence::Disable};
    Short_Circuit short_circuit{Short_Circuit::Disable};
    Extreme_Detector extreme_detector{Extreme_Detector::Disable};

    uint8_t short_circuit_count{0xFF};

    /* -------- Analog watchdog -------- */

    Analog_Watchdog watchdog{Analog_Watchdog::Disable};
    Analog_Watchdog_Mode watchdog_mode{Analog_Watchdog_Mode::After_Filter};
    //If Mode Watchdog == After filter
    Filter_Type filter_wathdog{Filter_Type::Sinc2};
    uint8_t watchdog_oversampling{5};
    int32_t watchdog_low_threshold{0x10000000};
    int32_t watchdog_high_threshold{0x7FFFFFFF};

    //callbacks
    Callback watchdog_callback{nullptr};
    Callback conversion_complete_callback{nullptr};
    Callback overrun_callback{nullptr};
    Callback clock_absence_callback{nullptr};
    Callback short_circuit_callback{nullptr};
};
    
    static constexpr std::array<std::pair<GPIODomain::Pin,uint8_t>,Possible_Pin_Channel> pin_to_channel =
    {{
        {PE4,3},{PC0,4},{PC1,0},{PC3,1},{PC5,2},
        {PB1,1},{PF13,6},{PE7,2},{PE10,4},{PE12,5},{PB10,7},{PB12,1},
        {PB14,2},{PD9,3},{PC7,3},{PC11,5},{PD1,6},{PD6,1},{PD7,4},
        {PB6,5}
    }};

    static consteval uint8_t get_channel(const GPIODomain::Pin& pin){
        for(int i = 0; i < Possible_Pin_Channel;i++){
            if(pin_to_channel[i].first == pin){
                return pin_to_channel[i].second;
            }
        }
        compile_error("This pin cannot be used as a DFSDM_Channel");
    }

       

    static consteval bool is_correct_oversampling(Filter_Type f, uint16_t osr) {
        switch (f) {
            case Filter_Type::FastSinc:
            case Filter_Type::Sinc1:
            case Filter_Type::Sinc2:
            case Filter_Type::Sinc3:
                return osr >= 1 && osr <= Oversampling_MAX;

            case Filter_Type::Sinc4:
                return osr >= 1 && osr <= Oversampling_MAX_Filter_4;

            case Filter_Type::Sinc5:
                return osr >= 1 && osr <= Oversampling_MAX_Filter_5;
        }
        return false;
    }
    static consteval GPIODomain::AlternateFunction dfsdm_channel_af(const GPIODomain::Pin& pin) {
        if ((pin.port == GPIODomain::Port::B && pin.pin == GPIO_PIN_1)  || 
            (pin.port == GPIODomain::Port::B && pin.pin == GPIO_PIN_10) ||
            (pin.port == GPIODomain::Port::B && pin.pin == GPIO_PIN_12) ||
            (pin.port == GPIODomain::Port::B && pin.pin == GPIO_PIN_14) ||
            (pin.port == GPIODomain::Port::C && pin.pin == GPIO_PIN_0))
        {
            return GPIODomain::AlternateFunction::AF6;
        }
        if((pin.port == GPIODomain::Port::C && pin.pin == GPIO_PIN_7) ||
            (pin.port == GPIODomain::Port::D && pin.pin == GPIO_PIN_6))
        {
            return GPIODomain::AlternateFunction::AF4;
        }  
        if((pin.port == GPIODomain::Port::B && pin.pin == GPIO_PIN_6)){
            return GPIODomain::AlternateFunction::AF11;
        }  
        return GPIODomain::AlternateFunction::AF3; //In any other case
    }
    struct Entry{
        Config_Channel config;
        uint8_t channel;
        size_t gpio_idx;
        int32_t* buffer;
        size_t buffer_size;

    };

    static constexpr size_t max_instances{8};
    template <size_t N>
    struct DFSDM_CHANNEL{
        using domain = DFSDM_CHANNEL_DOMAIN;
        const GPIODomain::Pin& pin;
        GPIODomain::GPIO gpio;
        const Config_Channel config;
        uint8_t channel;
        int32_t* buffer;
        size_t buffer_size;
        consteval DFSDM_CHANNEL(const GPIODomain::Pin& pin,const Config_Channel config, int32_t (&buffer)[N]) 
        : pin(pin), 
        gpio{pin,GPIODomain::OperationMode::ALT_PP,GPIODomain::Pull::None,GPIODomain::Speed::High,dfsdm_channel_af(pin)},
        config(config),
        buffer_size(N)
        {    
            static_assert(N != 0, "N must be bigger than 0");
            this->buffer = buffer;
            channel = get_channel(pin);
            //remove in a future
            if(config.dma == Dma::Enable){
                compile_error("Not implemented DMA yet");
            }

            if(config.offset > OFFSET_MAX || config.offset < OFFSET_MIN){
                compile_error("Your offset is bigger than the maximum size");
            }
            if(config.right_shift > 0x000000FF){
                compile_error("Your right_shift is bigger than the maximum size");
            }
            if(config.integrator <= 0){
                compile_error("DFSDM_FILTER: Integrator out of range");
            }
            if (!is_correct_oversampling(config.filter_type, config.oversampling)){
                compile_error("DFSDM_FILTER: invalid oversampling for selected filter type");
            } 
            if(config.watchdog_oversampling > 32){
                compile_error("DFSDM_Watchdog oversampling is bigger than the maximum allowed");
            }
            if(static_cast<uint32_t>(config.filter_wathdog) > 3){
                compile_error("Why would a sane person need a filter of the watchdog higher than sinc3");
            }
        }
        consteval DFSDM_CHANNEL(const GPIODomain::Pin& pin,const Config_Channel config, int32_t*buffer) 
        : pin(pin), 
        gpio{pin,GPIODomain::OperationMode::ALT_PP,GPIODomain::Pull::None,GPIODomain::Speed::High,dfsdm_channel_af(pin)},
        config(config),
        buffer(buffer),
        buffer_size(N)
        {    
            static_assert(N == 1, "N must be bigger than 0");
            channel = get_channel(pin);
            //remove in a future
            if(config.dma == Dma::Enable){
                compile_error("Not implemented DMA yet");
            }

            if(config.offset > OFFSET_MAX || config.offset < OFFSET_MIN){
                compile_error("Your offset is bigger than the maximum size");
            }
            if(config.right_shift > 0x000000FF){
                compile_error("Your right_shift is bigger than the maximum size");
            }
            if(config.integrator <= 0){
                compile_error("DFSDM_FILTER: Integrator out of range");
            }
            if (!is_correct_oversampling(config.filter_type, config.oversampling)){
                compile_error("DFSDM_FILTER: invalid oversampling for selected filter type");
            } 
            if(config.watchdog_oversampling > 32){
                compile_error("DFSDM_Watchdog oversampling is bigger than the maximum allowed");
            }
            if(static_cast<uint32_t>(config.filter_wathdog) > 3){
                compile_error("Why would a sane person need a filter of the watchdog higher than sinc3");
            }
        }
        template<class Ctx>
        consteval std::size_t inscribe(Ctx &ctx) const {
            const auto gpio_idx = gpio.inscribe(ctx); 
            Entry e{
                .config = config,
                .channel = channel,
                .gpio_idx = gpio_idx,
                .buffer = buffer,
                .buffer_size = buffer_size
            };
            return ctx.template add<DFSDM_CHANNEL_DOMAIN>(e, this);
        }
    };
    // I hate stm32,has volatile in the DFSDM structs.
    struct FilterConfig{
        uint32_t FLTCR1{};
        uint32_t FLTCR2{};
        uint32_t FLTFCR{};
        uint32_t FLTAWHTR{};
        uint32_t FLTAWLTR{};
        uint32_t FLTJCHGR{};
    };
    struct ChannelConfig{
        uint32_t CHCFGR1{};
        uint32_t CHCFGR2{};
        uint32_t CHAWSCDR{};
    };
    struct Config {
        size_t gpio_idx;
        FilterConfig init_data_filter;
        ChannelConfig init_data_channel; 
        
        uint32_t latency_cycles;
        Type_Conversion type_conv;
        Dma dma;

        uint8_t filter;
        uint8_t channel;
        
        size_t buffer_size;
        int32_t* buffer;

        //callbacks
        Callback watchdog_callback{nullptr};
        Callback conversion_complete_callback{nullptr};
        Callback overrun_callback{nullptr};
        Callback clock_absence_callback{nullptr};
        Callback short_circuit_callback{nullptr};
    };
    static consteval uint32_t compute_latency(const Entry& e){
        const uint32_t fosr = e.config.oversampling;
        const uint32_t iosr = e.config.integrator;

        if (e.config.fast == Fast_Conversion::Enable &&
            e.config.rcont == Regular_Mode::Continuous)
        {
            return fosr * iosr;
        }

        if (e.config.filter_type == Filter_Type::FastSinc) {
            return fosr * (iosr - 1 + 4) + 2;
        }

        const uint32_t ford = static_cast<uint32_t>(e.config.filter_type);
        return fosr * (iosr - 1 + ford) + ford;
    }
    static consteval uint32_t get_trigger(Trigger_Timer_Source trig){
        switch(trig){
            case Trigger_Timer_Source::Tim1 : return 0;
            case Trigger_Timer_Source::Tim3 : return 4;
            case Trigger_Timer_Source::Tim4 : return 5;
            case Trigger_Timer_Source::Tim7 : return 8;
            case Trigger_Timer_Source::Tim8 : return 2;
            case Trigger_Timer_Source::Tim16 : return 6;
            case Trigger_Timer_Source::Tim23 : return 11;
            case Trigger_Timer_Source::Tim24 : return 12;
            case Trigger_Timer_Source::Exti11 : return 24;
            case Trigger_Timer_Source::Exti15 : return 25;
            case Trigger_Timer_Source::Lptim1 : return 26;
            case Trigger_Timer_Source::Lptim2 : return 27;
            case Trigger_Timer_Source::Lptim3 : return 28;
            case Trigger_Timer_Source::Unused : break;
        }
        return 0;
    }
    //Filter registers
    static consteval uint32_t make_fltfcr(const Entry& e)
    {
        return
            (uint32_t(e.config.filter_type) << DFSDM_FLTFCR_FORD_Pos) |
            (uint32_t(e.config.oversampling -1) << DFSDM_FLTFCR_FOSR_Pos) |
            (uint32_t(e.config.integrator - 1) << DFSDM_FLTFCR_IOSR_Pos);
    }
    static consteval uint32_t make_fltcr2_global(){
        //Activate the interrupt, to activate the continous detection, then the channel can 
        return (uint32_t)(DFSDM_FLTCR2_CKABIE | DFSDM_FLTCR2_SCDIE);
    }
    static consteval uint32_t make_fltcr2(const Entry& e){
        uint32_t v = 0;
        if(e.config.dma == Dma::Disable || e.config.conversion_complete_callback != nullptr){
            v |= DFSDM_FLTCR2_REOCIE;
            v |= DFSDM_FLTCR2_JEOCIE;
        }
        if(e.config.watchdog == Analog_Watchdog::Enable){
            v |= DFSDM_FLTCR2_AWDIE;
            v |= 1 << (DFSDM_FLTCR2_AWDCH_Pos + e.channel);
        } 
        if (e.config.extreme_detector == Extreme_Detector::Enable){
            v |= 1 << (DFSDM_FLTCR2_EXCH_Pos + e.channel);
        }
        if(e.config.overrun == Overrun::Enable){
             v |= DFSDM_FLTCR2_JOVRIE;
             v |= DFSDM_FLTCR2_ROVRIE;
        }
        return v;
    }
    static consteval uint32_t make_fltcr1(const Entry& e,uint32_t filter)
    {
        uint32_t v = 0;
        
        if(e.config.type_conv == Type_Conversion::Regular){
            v |= (uint32_t(e.config.dma)   << DFSDM_FLTCR1_RDMAEN_Pos);
            v |= (uint32_t(e.config.fast)   << DFSDM_FLTCR1_FAST_Pos);
            v |= (uint32_t(e.config.rsync)  << DFSDM_FLTCR1_RSYNC_Pos);
            v |= (uint32_t(e.config.rcont)  << DFSDM_FLTCR1_RCONT_Pos);
        }else if(e.config.type_conv == Type_Conversion::Injected){
            v |= uint32_t(e.config.jscan) << DFSDM_FLTCR1_JSCAN_Pos; // activate conversion of the entire group
            v |= (uint32_t)(e.config.dma) << DFSDM_FLTCR1_JDMAEN_Pos;
            if(e.config.trigger_conv != Trigger_Timer_Source::Unused){
                v |= DFSDM_FLTCR1_JEXTEN_0; //with the risings
                if(filter == 0){
                    v |= get_trigger(e.config.trigger_conv) << DFSDM_FLTCR1_JEXTSEL_Pos;
                }
                if(e.config.rsync == Sync_Conversion::Sync_With_Flt0){
                    v |= DFSDM_FLTCR1_JSYNC;
                    
                }else{
                    v |= get_trigger(e.config.trigger_conv) << DFSDM_FLTCR1_JEXTSEL_Pos;
                }
            } 
        }
        v |= uint32_t(e.config.watchdog_mode) << DFSDM_FLTCR1_AWFSEL_Pos;
        return v;
    }
    static consteval uint32_t make_fltawhtr(const Entry& e){
        uint32_t v = 0;        
        if (e.config.watchdog_mode == Analog_Watchdog_Mode::Channel_Data)
            v |= (e.config.watchdog_high_threshold & 0xFFFF) << (DFSDM_FLTAWHTR_AWHT_Pos + DFSDM_FLTAWHTR_AWHT_Pos);
        else
            v |= (e.config.watchdog_high_threshold & 0xFFFFFF) << DFSDM_FLTAWHTR_AWHT_Pos;
        return v;
    }
    static consteval uint32_t make_fltawltr(const Entry& e){
        uint32_t v = 0;        
        if (e.config.watchdog_mode == Analog_Watchdog_Mode::Channel_Data)
            v |= (e.config.watchdog_low_threshold & 0xFFFF) << (DFSDM_FLTAWHTR_AWHT_Pos + DFSDM_FLTAWHTR_AWHT_Pos);
        else
            v |= (e.config.watchdog_low_threshold & 0xFFFFFF) << DFSDM_FLTAWHTR_AWHT_Pos;
        return v;
    }
    //Channel
    static consteval uint32_t make_chawscdr(const Entry& e){
        uint32_t v = 0;
        v |= uint32_t(e.config.short_circuit_count) << DFSDM_CHAWSCDR_SCDT_Pos;
        if(e.config.watchdog == Analog_Watchdog::Enable){
            v |= static_cast<uint32_t>(e.config.filter_wathdog) << DFSDM_CHAWSCDR_AWFORD_Pos;
            v |= static_cast<uint32_t>((e.config.watchdog_oversampling-1) & 0xF) << DFSDM_CHAWSCDR_AWFOSR_Pos;
        }
        return v;
    }
    static consteval uint32_t make_chcfgr1(const Entry& e){
        uint32_t v = 0;
        //DATPACK = 0  -> Standard mode
        //DATMPX = 0 -> Comes from an external serial input
        //Chinsel = 0 -> channel input are taken from pin of the same channel y
        v |= uint32_t(e.config.spi_clock_sel) << DFSDM_CHCFGR1_SPICKSEL_Pos;
        v |= uint32_t (e.config.spi_type) << DFSDM_CHCFGR1_SITP_Pos;
        v |= uint32_t(e.config.clock_absence) << DFSDM_CHCFGR1_CKABEN_Pos;
        v |= uint32_t(e.config.short_circuit) << DFSDM_CHCFGR1_SCDEN_Pos;

        return v;

    }
    static consteval uint32_t make_chcfgr2(const Entry& e){
        uint32_t v = 0;
        v |= (e.config.offset & 0x00FFFFFF) << DFSDM_CHCFGR2_OFFSET_Pos;
        v |= uint8_t(e.config.right_shift & 0x0F) << DFSDM_CHCFGR2_DTRBS_Pos;
        return v;
    }

    template <size_t N>
    static consteval std::array<Config, N> build(std::span<const Entry> entries) {
        if (N == 0) return {};
        std::array<Config, N> cfgs{};
        std::array<bool,8> channels_used{false};
        std::array<int8_t,4> filters_used{-1,-1,-1,-1};
         bool filter_per_channel = (N <= 4) ? true : false;

        for (size_t i = 0; i < N; ++i) {
            const Entry &e = entries[i];

            if(channels_used[e.channel] == true){
                compile_error("You have two pins using the same channel");
            }
            channels_used[e.channel] = true;
            Config& cfg = cfgs[i];
            
            cfg.gpio_idx = e.gpio_idx;
            cfg.channel = e.channel;
            cfg.buffer_size = e.buffer_size;
            cfg.buffer = e.buffer;
            cfg.type_conv = e.config.type_conv;
            cfg.dma = e.config.dma;

            //add the callbacks
            cfg.overrun_callback = e.config.overrun_callback;
            cfg.clock_absence_callback = e.config.clock_absence_callback;
            cfg.short_circuit_callback = e.config.short_circuit_callback;
            cfg.watchdog_callback = e.config.watchdog_callback;
            cfg.conversion_complete_callback = e.config.conversion_complete_callback;
            if(filter_per_channel){
                cfg.filter = i;
            }else{
                cfg.filter = e.channel / 2;
            }
            cfg.init_data_filter.FLTCR1 |= make_fltcr1(e,cfg.filter);
            cfg.init_data_filter.FLTCR2 |= make_fltcr2(e);
            cfg.init_data_filter.FLTFCR  |= make_fltfcr(e);
            cfg.init_data_channel.CHCFGR1 |= make_chcfgr1(e);
            cfg.init_data_channel.CHCFGR2 |= make_chcfgr2(e);
            cfg.init_data_channel.CHAWSCDR |= make_chawscdr(e);
            cfg.init_data_filter.FLTAWHTR |= make_fltawhtr(e);
            cfg.init_data_filter.FLTAWLTR |= make_fltawltr(e);
            if(e.config.type_conv == Type_Conversion::Injected) cfg.init_data_filter.FLTJCHGR |= 1 << e.channel;
            if(cfg.filter == 0) cfg.init_data_filter.FLTCR2 |= make_fltcr2_global();

            cfg.latency_cycles = compute_latency(e);
            if(filters_used[cfg.filter] != -1){
                if(cfgs[filters_used[cfg.filter]].init_data_filter.FLTCR1 != cfg.init_data_filter.FLTCR1 ||
                    (cfgs[filters_used[cfg.filter]].init_data_filter.FLTCR2 & 0xFF) != (cfg.init_data_filter.FLTCR2 & 0xFF) ||
                    cfgs[filters_used[cfg.filter]].init_data_filter.FLTFCR != cfg.init_data_filter.FLTFCR ||
                    cfgs[filters_used[cfg.filter]].init_data_filter.FLTAWLTR != cfg.init_data_filter.FLTAWLTR ||
                    cfgs[filters_used[cfg.filter]].init_data_filter.FLTAWHTR != cfg.init_data_filter.FLTAWHTR){
                        compile_error("You have two channels that goes to the same filter with different filter configuration");
                }
                //have the same thing in every register of the filter
                //Channel group conversion in injected mode
                cfgs[filters_used[cfg.filter]].init_data_filter.FLTJCHGR |= cfg.init_data_filter.FLTJCHGR;
                cfg.init_data_filter.FLTJCHGR = cfgs[filters_used[cfg.filter]].init_data_filter.FLTJCHGR;
                //Watchdog and Extreme detector channel enabled
                cfgs[filters_used[cfg.filter]].init_data_filter.FLTCR2 |= cfg.init_data_filter.FLTCR2;
                cfg.init_data_filter.FLTCR2 = cfgs[filters_used[cfg.filter]].init_data_filter.FLTCR2;
                //Watchdog and Extreme detector channel enabled
            }
            filters_used[cfg.filter] = i;    
        }
        return cfgs;
    }

    struct Instance {
        GPIODomain::Instance *gpio_instance;

        DFSDM_Filter_TypeDef *filter_regs{};
        DFSDM_Channel_TypeDef *channel_regs{};

        Callback watchdog_cb{};
        Callback short_circuit_cb{};
        Callback clock_absence_cb{};
        Callback overrun_cb{};
        Callback end_conversion_cb{};
        
        uint32_t latency_cycles; 
        uint8_t channel;
        uint8_t filter;
        Type_Conversion type_conv;
        Dma dma;
        
        int32_t* buffer{};
        size_t length_buffer{};
        size_t idx{};
        private:
            //split in differents categories the enable to clearness
            bool is_enabled_channel() const{
                return (channel_regs->CHCFGR1 & DFSDM_CHCFGR1_CHEN_Msk);
            }
            bool is_enabled_filter() const {
                return (filter_regs->FLTCR1 & DFSDM_FLTCR1_DFEN_Msk);
            }
            bool is_enabled_DFSDM() const{
                return (DFSDM1_Channel0->CHCFGR1 & DFSDM_CHCFGR1_DFSDMEN_Msk);
            }
            void enable_filter() {
                filter_regs->FLTCR1 |= DFSDM_FLTCR1_DFEN;
            }
            void enable_channel(){
                channel_regs->CHCFGR1 |= DFSDM_CHCFGR1_CHEN;
            }
            void enable_DFSDM_Peripheral(){
                DFSDM1_Channel0->CHCFGR1 |= DFSDM_CHCFGR1_DFSDMEN;
            }
            void disable_filter() {
                filter_regs->FLTCR1 &= ~(DFSDM_FLTCR1_DFEN_Msk);
            }
            void disable_channel(){
                channel_regs->CHCFGR1 &= ~(DFSDM_CHCFGR1_CHEN);
            }
            void disable_DFSDM_Peripheral(){
                DFSDM1_Channel0->CHCFGR1 &= ~(DFSDM_CHCFGR1_DFSDMEN);
            }

        public:
            DFSDM_Filter_TypeDef* get_filter_struct() const{
                return filter_regs;
            }
            DFSDM_Channel_TypeDef* get_channel_struct() const{
                return channel_regs;
            }
            bool is_enabled(){
                return is_enabled_DFSDM() && is_enabled_channel() && is_enabled_filter();
            }
            void enable() {
                //just in case enable everything to work
                enable_DFSDM_Peripheral();
                enable_channel();
                enable_filter();
            }

            void disable() {
                //only disable channel 
                channel_regs->CHCFGR1 &= ~(DFSDM_CHCFGR1_CHEN_Msk);
            }
            /*channel functions */
            void enable_clock_absence_detector(){
                channel_regs->CHCFGR1 |= DFSDM_CHCFGR1_CKABEN;
            }
            void enable_short_circuit_detector(){
                channel_regs->CHCFGR1 |= DFSDM_CHCFGR1_SCDEN;
            }
            void change_offset(int32_t offset){
                channel_regs->CHCFGR2 &= ~(DFSDM_CHCFGR2_OFFSET_Msk);
                channel_regs->CHCFGR2 |= (offset & 0x00FFFFFF) << DFSDM_CHCFGR2_OFFSET_Pos;
            }
            
            /*Filter functions*/         
            void start()
            {
                if (!is_enabled()) enable();
                
                if(type_conv == Type_Conversion::Regular) {
                        filter_regs->FLTCR1 |= DFSDM_FLTCR1_RSWSTART; // regular
                } else {
                    filter_regs->FLTCR1 |= DFSDM_FLTCR1_JSWSTART; // injected
                }
            }

            void modify_sync_conversion(Sync_Conversion type) {
                bool was_enabled_filter = is_enabled_filter();
                if (was_enabled_filter) disable_filter();

                filter_regs->FLTCR1 &= ~DFSDM_FLTCR1_RSYNC_Msk;
                filter_regs->FLTCR1 |= (uint32_t(type) << DFSDM_FLTCR1_RSYNC_Pos);

                if (was_enabled_filter) enable_filter();
            }
            void modify_regular_mode(Regular_Mode mode) {
                bool was_enabled_filter = is_enabled_filter();
                if(was_enabled_filter) disable_filter();

                filter_regs->FLTCR1 &= ~DFSDM_FLTCR1_RCONT_Msk;
                filter_regs->FLTCR1 |= (uint32_t(mode) << DFSDM_FLTCR1_RCONT_Pos);
                
                if(was_enabled_filter) enable_filter();
            }
            void read_this_channel_in_regular_mode(){
                bool was_enabled_filter = is_enabled_filter();
                if(was_enabled_filter) disable_filter();

                filter_regs->FLTCR1 &= ~DFSDM_FLTCR1_RCH_Msk;
                filter_regs->FLTCR1 |= (uint32_t(this->channel) << DFSDM_FLTCR1_RCONT_Pos);
            
                if(was_enabled_filter) enable_filter();
                start();
            }
            bool modify_oversampling(uint16_t oversampling) {
                if (oversampling == 0) return false;

                uint32_t ford = (filter_regs->FLTFCR & DFSDM_FLTFCR_FORD_Msk)  >> DFSDM_FLTFCR_FORD_Pos;

                if (ford <= 3 && oversampling > Oversampling_MAX) return false;
                if (ford == 4 && oversampling > Oversampling_MAX_Filter_4)  return false;
                if (ford == 5 && oversampling > Oversampling_MAX_Filter_5)   return false;

                bool was_enabled_filter = is_enabled_filter();
                if (was_enabled_filter) disable_filter();

                filter_regs->FLTFCR &= ~DFSDM_FLTFCR_FOSR_Msk;
                filter_regs->FLTFCR |= ((uint32_t)(oversampling -1) << DFSDM_FLTFCR_FOSR_Pos);
                if (was_enabled_filter) enable_filter();
                return true;
            }

            bool modify_integrator(uint8_t integrator) {

                if (integrator == 0 || integrator > 256) return false;
                bool was_enabled_filter = is_enabled_filter();
                if (was_enabled_filter) disable_filter();

                filter_regs->FLTFCR &= ~DFSDM_FLTFCR_IOSR_Msk;
                filter_regs->FLTFCR |= ((integrator -1) << DFSDM_FLTFCR_IOSR_Pos);

                if (was_enabled_filter) enable_filter();
                return true;
            }

            bool modify_filter_order(Filter_Type type) {

                uint32_t fosr =((filter_regs->FLTFCR & DFSDM_FLTFCR_FOSR_Msk) >> DFSDM_FLTFCR_FOSR_Pos);

                if (type == Filter_Type::Sinc4 && fosr > Oversampling_MAX_Filter_4) return false;
                if (type == Filter_Type::Sinc5 && fosr > Oversampling_MAX_Filter_5)  return false;

                bool was_enabled_filter = is_enabled_filter();
                if (was_enabled_filter) disable_filter();

                filter_regs->FLTFCR &= ~DFSDM_FLTFCR_FORD_Msk;
                filter_regs->FLTFCR |= (uint32_t(type)
                                << DFSDM_FLTFCR_FORD_Pos);

                if (was_enabled_filter) enable_filter();
                return true;
            }

            uint32_t check_latency_cycles() {
                return filter_regs->FLTCNVTIMR >> DFSDM_FLTCNVTIMR_CNVCNT_Pos;
            }

            uint32_t check_min_extreme_detector() {
                return filter_regs->FLTEXMIN >> DFSDM_FLTEXMIN_EXMIN_Pos;
            }

            uint32_t check_max_extreme_detector() {
                return filter_regs->FLTEXMAX >> DFSDM_FLTEXMAX_EXMAX_Pos;
            }
            void modify_watchdog_lth(uint32_t value) {

                filter_regs->FLTAWLTR &= ~DFSDM_FLTAWLTR_AWLT_Msk;
                bool fast = (filter_regs->FLTCR1 & DFSDM_FLTCR1_AWFSEL);

                if (fast)
                    filter_regs->FLTAWLTR = (value & 0xFFFF) << (DFSDM_FLTAWLTR_AWLT_Pos + DFSDM_FLTAWLTR_AWLT_Pos); // Only 16 bits
                else
                    filter_regs->FLTAWLTR = (value & 0xFFFFFF) << DFSDM_FLTAWLTR_AWLT_Pos; // 24 bits
            }

            void modify_watchdog_hth(uint32_t value) {
                filter_regs->FLTAWHTR &=  ~DFSDM_FLTAWHTR_AWHT_Msk;
                bool fast = (filter_regs->FLTCR1 & DFSDM_FLTCR1_AWFSEL);
                
                if (fast)
                    filter_regs->FLTAWHTR = (value & 0xFFFF) << (DFSDM_FLTAWHTR_AWHT_Pos + DFSDM_FLTAWHTR_AWHT_Pos);
                else
                    filter_regs->FLTAWHTR = (value & 0xFFFFFF) << DFSDM_FLTAWHTR_AWHT_Pos;
            }
            //get the last conversion from a filter
            static uint8_t get_last_conversion_from_filter(uint8_t filter, Type_Conversion conv){
                uint8_t channel = 0xFF;
                switch(filter){
                    case 0:
                        if(conv == Type_Conversion::Injected){
                            channel = (DFSDM1_Filter0->FLTJDATAR & 0x7);
                        }else{
                            channel = (DFSDM1_Filter0->FLTRDATAR & 0x7);
                        }
                        break;
                    case 1: 
                        if(conv == Type_Conversion::Injected){
                            channel = (DFSDM1_Filter1->FLTJDATAR & 0x7);
                        }else{
                            channel = (DFSDM1_Filter1->FLTRDATAR & 0x7);
                        }
                        break;

                    case 2:
                        if(conv == Type_Conversion::Injected){
                            channel = (DFSDM1_Filter2->FLTJDATAR & 0x7);
                        }else{
                            channel = (DFSDM1_Filter2->FLTRDATAR & 0x7);
                        }
                        break;
                    case 3: 
                        if(conv == Type_Conversion::Injected){
                            channel = (DFSDM1_Filter3->FLTJDATAR & 0x7);
                        }else{
                            channel = (DFSDM1_Filter3->FLTRDATAR & 0x7);
                        }
                        break;
                    default: break;
                }
                return channel;
            }
    };
    static inline Instance* channel_instances[DFSDM_CHANNEL_DOMAIN::max_instances] = {nullptr}; 
    static inline uint8_t channels_enabled{};
    static constexpr DFSDM_Filter_TypeDef* filter_hw[4] = {
            DFSDM1_Filter0,
            DFSDM1_Filter1,
            DFSDM1_Filter2,
            DFSDM1_Filter3
        };
        static constexpr DFSDM_Channel_TypeDef* channel_hw[8] = {
            DFSDM1_Channel0,
            DFSDM1_Channel1,
            DFSDM1_Channel2,
            DFSDM1_Channel3,
            DFSDM1_Channel4,
            DFSDM1_Channel5,
            DFSDM1_Channel6,
            DFSDM1_Channel7
        };
    template <std::size_t N> struct Init {
       
        static inline std::array<Instance, N> instances{};
        static void init(std::span<const Config, N> cfgs,std::span<GPIODomain::Instance> gpio_instances) {
            if(N == 0) return;
            std::array<bool,4> filters_configured = {false,false,false,false};
            RCC->APB2ENR |= RCC_APB2ENR_DFSDM1EN; //Activate the DFSDM clock
            for (size_t i = 0; i < N; ++i) {
                const Config &cfg = cfgs[i];
                filter_hw[cfg.filter]->FLTCR1 &= ~DFSDM_FLTCR1_DFEN;
                channel_hw[cfg.channel]->CHCFGR1 &= ~DFSDM_CHCFGR1_CHEN;
            }
            for (std::size_t i = 0; i < N; ++i) {
                const Config &cfg = cfgs[i];
                Instance &inst = instances[i];
                
                inst.gpio_instance = &gpio_instances[cfg.gpio_idx];

                inst.filter_regs = filter_hw[cfg.filter];
                inst.channel_regs = channel_hw[cfg.channel];

                inst.latency_cycles = cfg.latency_cycles;
                inst.type_conv = cfg.type_conv;
                inst.filter = cfg.filter;
                inst.channel = cfg.channel;
                inst.dma = cfg.dma;

                inst.buffer = cfg.buffer;
                inst.length_buffer = cfg.buffer_size;

                //callbacks
                inst.overrun_cb = cfg.overrun_callback;
                inst.short_circuit_cb = cfg.short_circuit_callback;
                inst.watchdog_cb = cfg.watchdog_callback;
                inst.end_conversion_cb = cfg.conversion_complete_callback;
                if(!filters_configured[cfg.filter]){
                    //add everything to the register of the filter
                    inst.filter_regs->FLTCR1 |= cfg.init_data_filter.FLTCR1;
                    if(inst.type_conv == Type_Conversion::Regular){
                        inst.filter_regs->FLTCR1 &= ~DFSDM_FLTCR1_RCH_Msk;
                        inst.filter_regs->FLTCR1 |= uint32_t(inst.channel) << DFSDM_FLTCR1_RCH_Pos;
                    }
                    inst.filter_regs->FLTCR2 |= cfg.init_data_filter.FLTCR2;
                    inst.filter_regs->FLTFCR |= cfg.init_data_filter.FLTFCR;   
                    inst.filter_regs->FLTAWHTR |= cfg.init_data_filter.FLTAWHTR;
                    inst.filter_regs->FLTAWLTR |= cfg.init_data_filter.FLTAWLTR;
                    inst.filter_regs->FLTJCHGR = cfg.init_data_filter.FLTJCHGR;
                    
                    filters_configured[cfg.filter] = true;
                }   
                //add everything to the channel register
                inst.channel_regs->CHCFGR1 |= cfg.init_data_channel.CHCFGR1;
                inst.channel_regs->CHCFGR2 |= cfg.init_data_channel.CHCFGR2;
                inst.channel_regs->CHAWSCDR |= cfg.init_data_channel.CHAWSCDR;
                
                
                //update channel_instances
                channel_instances[inst.channel] = &inst;
                channels_enabled |= 1 << inst.channel;
            }
            if(N > 0){
                //Activate the DFSDM GLOBAL Interface 
                DFSDM1_Channel0->CHCFGR1 |= DFSDM_CHCFGR1_DFSDMEN;
                for(int i = 0; i < 8; i++){
                    channel_hw[i]->CHCFGR1 |= DFSDM_CHCFGR1_CHEN;
                }
                for(int i = 0; i < 4;i++){
                    filter_hw[i]->FLTCR1 |= DFSDM_FLTCR1_DFEN;
                }
                //activate the NVIC
                for(int i = 0; i < 4; i++){
                    if(filters_configured[i] == true){
                        switch(i){
                            case 0:  NVIC_EnableIRQ(DFSDM1_FLT0_IRQn);     break;
                            case 1:  NVIC_EnableIRQ(DFSDM1_FLT1_IRQn);     break;
                            case 2: NVIC_EnableIRQ(DFSDM1_FLT2_IRQn);      break;
                            case 3: NVIC_EnableIRQ(DFSDM1_FLT3_IRQn);      break;
                        }
                    }
                } 
            }
        }
    };
    
    static void handle_irq(uint8_t filter_index)
    {

        DFSDM_Filter_TypeDef* filter = filter_hw[filter_index];

        uint32_t isr = filter->FLTISR;

        if(isr & DFSDM_FLTISR_REOCF_Msk){
            //Save it in the address provide by the user
            int32_t data = filter->FLTRDATAR;
            Instance* inst = channel_instances[(data & DFSDM_FLTRDATAR_RDATACH_Msk)>>DFSDM_FLTRDATAR_RDATACH_Pos];
            if(inst != nullptr && inst->buffer != nullptr){
                if(inst->dma == Dma::Disable){
                    inst->buffer[inst->idx] = int32_t(data & DFSDM_FLTRDATAR_RDATA_Msk) >> DFSDM_FLTRDATAR_RDATA_Pos;
                    inst->idx = (inst->idx + 1) % inst->length_buffer;
                }
                if(inst->end_conversion_cb != nullptr){
                    inst->end_conversion_cb();
                }
            }
        }
        if(isr & DFSDM_FLTISR_JEOCF_Msk){
            //Save it in the address provide by the user
            int32_t data = filter->FLTJDATAR;
            Instance* inst = channel_instances[(data & DFSDM_FLTJDATAR_JDATACH_Msk) >> DFSDM_FLTJDATAR_JDATACH_Pos];
            if(inst != nullptr && inst->buffer != nullptr){
                if(inst->dma == Dma::Disable){
                    inst->buffer[inst->idx] = int32_t(data & DFSDM_FLTJDATAR_JDATA_Msk) >> DFSDM_FLTJDATAR_JDATA_Pos;
                    inst->idx = (inst->idx + 1) % inst->length_buffer;
                }
                if(inst->end_conversion_cb != nullptr){
                    inst->end_conversion_cb();
                }
            }
        }
        if(isr & DFSDM_FLTISR_ROVRF_Msk){
            Instance* inst = channel_instances[filter->FLTRDATAR & DFSDM_FLTRDATAR_RDATACH_Msk];
            if(inst != nullptr && inst->overrun_cb != nullptr) inst->overrun_cb();
            //clear
            filter->FLTICR |= DFSDM_FLTISR_ROVRF;
        }
        if(isr & DFSDM_FLTISR_JOVRF_Msk){
            Instance* inst = channel_instances[filter->FLTJDATAR & DFSDM_FLTJDATAR_JDATACH_Msk];
            if(inst != nullptr && inst->overrun_cb != nullptr) inst->overrun_cb();
            //clear
            filter->FLTICR |= DFSDM_FLTISR_JOVRF;
        }
        if(isr & (channels_enabled << DFSDM_FLTICR_CLRSCDF_Pos)){
            uint32_t ch = __builtin_ctz(isr & DFSDM_FLTISR_SCDF_Msk) >> DFSDM_FLTISR_SCDF_Pos;
            if(channel_instances[ch] != nullptr && channel_instances[ch]->short_circuit_cb != nullptr) channel_instances[ch]->short_circuit_cb();
            //clear
            filter->FLTICR |= DFSDM_FLTICR_CLRSCDF;
        }
        if(isr & (channels_enabled << DFSDM_FLTISR_CKABF_Pos)){
            uint32_t ch = __builtin_ctz(isr & DFSDM_FLTISR_CKABF_Msk)>> DFSDM_FLTISR_CKABF_Pos;
            if(channel_instances[ch] != nullptr && channel_instances[ch]->clock_absence_cb != nullptr) channel_instances[ch]->clock_absence_cb();
            //clear
            filter->FLTICR |= DFSDM_FLTICR_CLRCKABF;
        }
        //Analog watchdog
        if (isr & (DFSDM_FLTISR_AWDF << DFSDM_FLTISR_AWDF_Pos))
        {
            if(filter->FLTAWSR & DFSDM_FLTAWSR_AWHTF_Msk){
                uint32_t ch = __builtin_ctz(filter->FLTAWSR & DFSDM_FLTAWSR_AWHTF_Msk);
                if(channel_instances[ch] != nullptr && channel_instances[ch]->watchdog_cb != nullptr) channel_instances[ch]->watchdog_cb();
                //clear
                filter->FLTAWCFR = DFSDM_FLTAWCFR_CLRAWHTF;
            }
            if(filter->FLTAWSR & DFSDM_FLTAWSR_AWLTF_Msk){
                uint32_t ch = __builtin_ctz(filter->FLTAWSR & DFSDM_FLTAWSR_AWLTF_Msk);
                if(channel_instances[ch] != nullptr && channel_instances[ch]->watchdog_cb != nullptr) channel_instances[ch]->watchdog_cb();
                //clear
                filter->FLTAWCFR = DFSDM_FLTAWCFR_CLRAWLTF;
            }
        }
    }
};

struct DFSDM_CLK_DOMAIN{
    static constexpr GPIODomain::Pin valid_clk_pins[] = {
            {GPIODomain::Port::C,GPIO_PIN_2}, 
            {GPIODomain::Port::B, GPIO_PIN_0}, 
            {GPIODomain::Port::E, GPIO_PIN_9}, 
            {GPIODomain::Port::D, GPIO_PIN_3},
            {GPIODomain::Port::D, GPIO_PIN_10}
    };

    static consteval bool is_valid_dfsdm_clk_pin(GPIODomain::Port port, uint32_t pin) {
        bool found = false;
        for (auto &p : valid_clk_pins) {
            if (p.port == port && p.pin == pin) {
                found = true;
                break;
            }
        }
        return found;
    }

    static consteval GPIODomain::AlternateFunction dfsdm_clk_af(const GPIODomain::Pin& pin) {
        if ((pin.port == GPIODomain::Port::C && pin.pin == GPIO_PIN_2)|| (pin.port == GPIODomain::Port::B && pin.pin == GPIO_PIN_0))
            return GPIODomain::AlternateFunction::AF6;  
        return GPIODomain::AlternateFunction::AF3; //In every other case
    }
    struct Entry{
        size_t gpio_idx;
        uint16_t clk_divider;
    };
    
    struct DFSDM_CLK{
        using domain = DFSDM_CLK_DOMAIN;
        GPIODomain::GPIO gpio;
        GPIODomain::Pin pin;
        uint8_t clk_divider;
        consteval DFSDM_CLK(const GPIODomain::Pin &pin,uint8_t clk_divider = 100): // clk_divider = 100 -> 1Mhz
        gpio{pin,GPIODomain::OperationMode::ALT_PP,GPIODomain::Pull::None, GPIODomain::Speed::High,dfsdm_clk_af(pin)},
        pin(pin),
        clk_divider(clk_divider)
        {}


        template<class Ctx> consteval std::size_t inscribe(Ctx &ctx) const{
            const auto gpio_idx = gpio.inscribe(ctx);
            if(!is_valid_dfsdm_clk_pin(pin.port,pin.pin)){
                compile_error("Invalid clk dfsdm pin used");
            }
            if(clk_divider < 7 || clk_divider > 256){
                compile_error("The clk_divider has to be between 7 and 256");
            }
            Entry e{.gpio_idx = gpio_idx,.clk_divider = clk_divider};
            return ctx.template add<DFSDM_CLK_DOMAIN>(e,this);
        }
    };
    static constexpr std::size_t max_instances{1};
    struct Config{
        size_t gpio_idx;
        uint16_t clk_divider;
    };
    template <size_t N>
    static consteval std::array<Config, N> build(std::span<const Entry> entries) {
        std::array<Config, N> cfgs{};
        static_assert(N <= 1,"You can't have more than one clock_out");
        for (std::size_t i = 0; i < N; ++i) {
            cfgs[i] = {
                .gpio_idx = entries[i].gpio_idx,
                .clk_divider = entries[i].clk_divider
            };
        }
        return cfgs;
    }
        struct Instance{
            GPIODomain::Instance *gpio_instance;
            uint16_t clk_divider;
            /*Already called in init()*/
            void init(){
                RCC->APB2ENR |= RCC_APB2ENR_DFSDM1EN; //Activate the DFSDM Clock OUT in RCC by default it uses rcc_pclk2
                //Disable DFSDMEN to change parameters
                DFSDM1_Channel0->CHCFGR1 &= ~DFSDM_CHCFGR1_DFSDMEN;

                //CKOUTSRC = 0 -> kernel clock (rcc_pclk2)  It works 137,5 Mhz, 
                DFSDM1_Channel0->CHCFGR1 &= ~DFSDM_CHCFGR1_CKOUTSRC;
                //CKOUT Divider. Divider = CKOUTDIV + 1
                DFSDM1_Channel0->CHCFGR1 &= ~DFSDM_CHCFGR1_CKOUTDIV;

                DFSDM1_Channel0->CHCFGR1 |= uint32_t(clk_divider -1) << DFSDM_CHCFGR1_CKOUTDIV_Pos;
                
                //enable the DFSDM Global Interface
                DFSDM1_Channel0->CHCFGR1 |= DFSDM_CHCFGR1_DFSDMEN;
            }
            bool disable(){
                DFSDM1_Channel0->CHCFGR1 &= ~DFSDM_CHCFGR1_DFSDMEN;
                return (DFSDM1_Channel0->CHCFGR1 & DFSDM_CHCFGR1_DFSDMEN) == 0;
            }
            bool enable(){
                DFSDM1_Channel0->CHCFGR1 |=  DFSDM_CHCFGR1_DFSDMEN;
                return(DFSDM1_Channel0->CHCFGR1 & DFSDM_CHCFGR1_DFSDMEN);
            }
            bool change_divider(uint8_t div){
                if(div < 4) return false;
                clk_divider = div;
                if(disable()){
                    init();
                    return true;
                }
                return false;
            } 

        };
        template <std::size_t N>
        struct Init {
            static inline std::array<Instance, N> instances{};
            static void init(std::span<const Config, N> cfgs,std::span<GPIODomain::Instance> gpio_instances) {
                if(N == 0) return;
                const auto &c = cfgs[0];
                auto &inst = instances[0];
                inst.gpio_instance = &gpio_instances[c.gpio_idx];
                inst.clk_divider = c.clk_divider;
                 inst.init();
            }
        };
    };

};