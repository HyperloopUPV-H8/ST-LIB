#pragma once

#include <string>

#include "HALAL/HALAL.hpp"
#include "ST-LIB_HIGH.hpp"
#include "ST-LIB_LOW.hpp"

class STLIB {
public:
#ifdef STLIB_ETH
    static void
    start(MAC mac, IPV4 ip, IPV4 subnet_mask, IPV4 gateway, UART::Peripheral& printf_peripheral);

    static void start(
        const std::string& mac = "00:80:e1:00:00:00",
        const std::string& ip = "192.168.1.4",
        const std::string& subnet_mask = "255.255.0.0",
        const std::string& gateway = "192.168.1.1",
        UART::Peripheral& printf_peripheral = UART::uart2
    );
#else
    static void start(UART::Peripheral& printf_peripheral = UART::uart2);
#endif

    static void update();
};

namespace ST_LIB {
extern void compile_error(const char* msg);

// The contract of BuildCtx/Board is documented in docs/st-lib-board-contract.md.
template <typename... Domains> struct BuildCtx {
    template <typename D> using Decl = typename D::Entry;
    template <typename D> static constexpr std::size_t max_count_v = D::max_instances;

    std::tuple<std::array<Decl<Domains>, max_count_v<Domains>>...> storage{};
    std::tuple<std::array<const void*, max_count_v<Domains>>...> owners{};
    std::array<std::size_t, sizeof...(Domains)> sizes{};

    template <typename D, std::size_t I = 0> static consteval std::size_t domain_index() {
        if constexpr (I >= sizeof...(Domains)) {
            static_assert([] { return false; }(), "Domain not found");
            return 0;
        } else if constexpr (std::is_same_v<D, std::tuple_element_t<I, std::tuple<Domains...>>>) {
            return I;
        } else {
            return domain_index<D, I + 1>();
        }
    }

    template <typename D, typename Owner>
    consteval std::size_t add(typename D::Entry e, const Owner* owner) {
        constexpr std::size_t I = domain_index<D>();
        auto& arr = std::get<I>(storage);
        auto& own = std::get<I>(owners);
        auto& size = sizes[I];

        const auto idx = size;
        arr[size] = e;
        own[size] = owner;
        ++size;
        return idx;
    }

    template <typename D> consteval auto span() const {
        constexpr std::size_t I = domain_index<D>();
        auto const& arr = std::get<I>(storage);
        auto const size = sizes[I];
        using E = typename D::Entry;
        return std::span<const E>{arr.data(), size};
    }

    template <typename D> consteval auto owners_span() const {
        constexpr std::size_t I = domain_index<D>();
        auto const& arr = std::get<I>(owners);
        auto const size = sizes[I];
        return std::span<const void* const>{arr.data(), size};
    }

    template <typename D> consteval std::size_t size() const {
        constexpr std::size_t I = domain_index<D>();
        return sizes[I];
    }
};

using DomainsCtx = BuildCtx<
    MPUDomain,
    GPIODomain,
    TimerDomain,
    DMADomain,
    SPIDomain,
    DigitalOutputDomain,
    DigitalInputDomain,
    MdmaPacketDomain,
    SdDomain,
    EthernetDomain,
    ADCDomain,
    EXTIDomain,
    DFSDM_CHANNEL_DOMAIN,
    DFSDM_CLK_DOMAIN /* PWMDomain, ...*/>;

namespace BuildUtils {

template <std::size_t TotalN, std::size_t... ExtraNs>
consteval std::array<DMADomain::Entry, TotalN> merge_dma_entries(
    std::span<const DMADomain::Entry> base_entries,
    const std::array<DMADomain::Entry, ExtraNs>&... extra_entries
) {
    if (base_entries.size() + (ExtraNs + ...) != TotalN) {
        compile_error("DMA merged entry count mismatch");
    }

    std::array<DMADomain::Entry, TotalN> merged{};
    std::size_t cursor = 0;

    for (const auto& entry : base_entries) {
        merged[cursor++] = entry;
    }

    auto append = [&]<std::size_t N>(const std::array<DMADomain::Entry, N>& entries) {
        for (const auto& entry : entries) {
            merged[cursor++] = entry;
        }
    };
    (append(extra_entries), ...);

    return merged;
}

template <std::size_t TotalN, std::size_t... ExtraNs>
consteval std::array<DMADomain::Config, TotalN> build_dma_configs(
    std::span<const DMADomain::Entry> base_entries,
    const std::array<DMADomain::Entry, ExtraNs>&... extra_entries
) {
    return DMADomain::template build<TotalN>(std::span<const DMADomain::Entry, TotalN>{
        merge_dma_entries<TotalN>(base_entries, extra_entries...)
    });
}

} // namespace BuildUtils

template <auto&... devs> struct Board {
    static consteval auto build_ctx() {
        DomainsCtx ctx{};
        (devs.inscribe(ctx), ...);
        return ctx;
    }

    static constexpr auto ctx = build_ctx();

    template <typename D> static consteval std::size_t domain_size() {
        return ctx.template span<D>().size();
    }

    static consteval auto build() {
        constexpr std::size_t mpuN = domain_size<MPUDomain>();
        constexpr std::size_t gpioN = domain_size<GPIODomain>();
        constexpr std::size_t timN = domain_size<TimerDomain>();
        constexpr std::size_t adcN = domain_size<ADCDomain>();
        constexpr auto adc_cfgs = ADCDomain::template build<adcN>(ctx.template span<ADCDomain>());
        constexpr std::size_t adc_dma_extraN = ADCDomain::dma_contribution_count(
            std::span<const ADCDomain::Config, adcN>{adc_cfgs},
            ctx.template span<DMADomain>()
        );
        constexpr auto adc_dma_entries =
            ADCDomain::template build_dma_contributions<adc_dma_extraN>(
                ctx.template span<DMADomain>(),
                std::span<const ADCDomain::Config, adcN>{adc_cfgs}
            );
        constexpr std::size_t dfsdmN = domain_size<DFSDM_CHANNEL_DOMAIN>();
        constexpr auto dfsdm_cfgs =
            DFSDM_CHANNEL_DOMAIN::template build<dfsdmN>(ctx.template span<DFSDM_CHANNEL_DOMAIN>());
        constexpr std::size_t dfsdm_dma_extraN = DFSDM_CHANNEL_DOMAIN::dma_contribution_count(
            std::span<const DFSDM_CHANNEL_DOMAIN::Config, dfsdmN>{dfsdm_cfgs},
            ctx.template span<DMADomain>()
        );
        constexpr auto dfsdm_dma_entries =
            DFSDM_CHANNEL_DOMAIN::template build_dma_contributions<dfsdm_dma_extraN>(
                ctx.template span<DMADomain>(),
                std::span<const DFSDM_CHANNEL_DOMAIN::Config, dfsdmN>{dfsdm_cfgs}
            );
        constexpr std::size_t dfsdm_clkN = domain_size<DFSDM_CLK_DOMAIN>();
        constexpr std::size_t spiN = domain_size<SPIDomain>();
        constexpr std::size_t doutN = domain_size<DigitalOutputDomain>();
        constexpr std::size_t dinN = domain_size<DigitalInputDomain>();
        constexpr std::size_t mdmaPacketN = domain_size<MdmaPacketDomain>();
        constexpr std::size_t sdN = domain_size<SdDomain>();
        constexpr std::size_t ethN = domain_size<EthernetDomain>();
        constexpr std::size_t dmaN = domain_size<DMADomain>() + adc_dma_extraN + dfsdm_dma_extraN;
        constexpr std::size_t extiN = domain_size<EXTIDomain>();

        // ...

        struct ConfigBundle {
            std::array<MPUDomain::Config, mpuN> mpu_cfgs;
            std::array<GPIODomain::Config, gpioN> gpio_cfgs;
            std::array<TimerDomain::Config, timN> tim_cfgs;
            std::array<DMADomain::Config, dmaN> dma_cfgs;
            std::array<SPIDomain::Config, spiN> spi_cfgs;
            std::array<DigitalOutputDomain::Config, doutN> dout_cfgs;
            std::array<DigitalInputDomain::Config, dinN> din_cfgs;
            std::array<MdmaPacketDomain::Config, mdmaPacketN> mdma_packet_cfgs;
            std::array<SdDomain::Config, sdN> sd_cfgs;
            std::array<EthernetDomain::Config, ethN> eth_cfgs;
            std::array<ADCDomain::Config, adcN> adc_cfgs;
            std::array<EXTIDomain::Config, extiN> exti_cfgs;
            std::array<DFSDM_CHANNEL_DOMAIN::Config, dfsdmN> dfsdm_cfgs;
            std::array<DFSDM_CLK_DOMAIN::Config, dfsdm_clkN> dfsdm_clk_cfgs;
            // ...
        };

        return ConfigBundle{
            .mpu_cfgs = MPUDomain::template build<mpuN>(ctx.template span<MPUDomain>()),
            .gpio_cfgs = GPIODomain::template build<gpioN>(ctx.template span<GPIODomain>()),
            .tim_cfgs = TimerDomain::template build<timN>(ctx.template span<TimerDomain>()),
            .dma_cfgs = BuildUtils::build_dma_configs<dmaN>(
                ctx.template span<DMADomain>(),
                adc_dma_entries,
                dfsdm_dma_entries
            ),
            .spi_cfgs = SPIDomain::template build<spiN>(ctx.template span<SPIDomain>()),
            .dout_cfgs =
                DigitalOutputDomain::template build<doutN>(ctx.template span<DigitalOutputDomain>()
                ),
            .din_cfgs =
                DigitalInputDomain::template build<dinN>(ctx.template span<DigitalInputDomain>()),
            .mdma_packet_cfgs =
                MdmaPacketDomain::template build<mdmaPacketN>(ctx.template span<MdmaPacketDomain>()
                ),
            .sd_cfgs = SdDomain::template build<sdN>(ctx.template span<SdDomain>()),
            .eth_cfgs = EthernetDomain::template build<ethN>(ctx.template span<EthernetDomain>()),
            .adc_cfgs = adc_cfgs,
            .exti_cfgs = EXTIDomain::template build<extiN>(ctx.template span<EXTIDomain>()),
            .dfsdm_cfgs = dfsdm_cfgs,
            .dfsdm_clk_cfgs =
                DFSDM_CLK_DOMAIN::template build<dfsdm_clkN>(ctx.template span<DFSDM_CLK_DOMAIN>())
            // ...
        };
    }

    static constexpr auto cfg = build();

    static void init() {
        constexpr std::size_t mpuN = domain_size<MPUDomain>();
        constexpr std::size_t gpioN = domain_size<GPIODomain>();
        constexpr std::size_t timN = domain_size<TimerDomain>();
        constexpr std::size_t dmaN = std::tuple_size_v<decltype(cfg.dma_cfgs)>;
        constexpr std::size_t spiN = domain_size<SPIDomain>();
        constexpr std::size_t doutN = domain_size<DigitalOutputDomain>();
        constexpr std::size_t dinN = domain_size<DigitalInputDomain>();
        constexpr std::size_t mdmaPacketN = domain_size<MdmaPacketDomain>();
        constexpr std::size_t sdN = domain_size<SdDomain>();
        constexpr std::size_t ethN = domain_size<EthernetDomain>();
        constexpr std::size_t adcN = domain_size<ADCDomain>();
        constexpr std::size_t extiN = domain_size<EXTIDomain>();
        constexpr std::size_t dfsdmN = domain_size<DFSDM_CHANNEL_DOMAIN>();
        constexpr std::size_t dfsdm_clkN = domain_size<DFSDM_CLK_DOMAIN>();
        // ...

#ifdef HAL_IWDG_MODULE_ENABLED
        Watchdog::check_reset_flag();
        Hard_fault_check();
#endif
        HAL_Init();
        HALconfig::system_clock();
        HALconfig::peripheral_clock();

#ifdef HAL_RTC_MODULE_ENABLED
        (void)Global_RTC::ensure_started();
#endif

        MPUDomain::Init<mpuN, cfg.mpu_cfgs>::init();
        GPIODomain::Init<gpioN>::init(cfg.gpio_cfgs);
        TimerDomain::Init<timN>::init(cfg.tim_cfgs);
        DMADomain::Init<dmaN>::init(cfg.dma_cfgs);
        SPIDomain::Init<spiN>::init(
            cfg.spi_cfgs,
            GPIODomain::Init<gpioN>::instances,
            DMADomain::Init<dmaN>::instances
        );
        DigitalOutputDomain::Init<doutN>::init(cfg.dout_cfgs, GPIODomain::Init<gpioN>::instances);
        DigitalInputDomain::Init<dinN>::init(cfg.din_cfgs, GPIODomain::Init<gpioN>::instances);
        MdmaPacketDomain::Init<mdmaPacketN>::init(
            cfg.mdma_packet_cfgs,
            MPUDomain::Init<mpuN, cfg.mpu_cfgs>::instances
        );
        SdDomain::Init<sdN>::init(
            cfg.sd_cfgs,
            MPUDomain::Init<mpuN, cfg.mpu_cfgs>::instances,
            DigitalInputDomain::Init<dinN>::instances
        );
        EthernetDomain::Init<ethN>::init(cfg.eth_cfgs, DigitalOutputDomain::Init<doutN>::instances);
        ADCDomain::Init<adcN, cfg.adc_cfgs>::init(
            cfg.adc_cfgs,
            GPIODomain::Init<gpioN>::instances,
            DMADomain::Init<dmaN>::instances
        );
        EXTIDomain::Init<extiN>::init(cfg.exti_cfgs, GPIODomain::Init<gpioN>::instances);

        DFSDM_CHANNEL_DOMAIN::Init<dfsdmN, cfg.dfsdm_cfgs>::init(
            GPIODomain::Init<gpioN>::instances,
            DMADomain::Init<dmaN>::instances
        );
        DFSDM_CLK_DOMAIN::Init<dfsdm_clkN>::init(
            cfg.dfsdm_clk_cfgs,
            GPIODomain::Init<gpioN>::instances
        );
        // ...
    }

    template <typename Domain, auto& Target, std::size_t I = 0>
    static consteval std::size_t owner_index_of() {
        constexpr auto owners = ctx.template owners_span<Domain>();

        if constexpr (I >= owners.size()) {
            compile_error("Device not registered in domain");
            return 0;
        } else {
            return owners[I] == &Target ? I : owner_index_of<Domain, Target, I + 1>();
        }
    }

    template <auto& Target> static auto& instance_of() {
        using DevT = std::remove_cvref_t<decltype(Target)>;
        using Domain = typename DevT::domain;

        constexpr std::size_t idx = owner_index_of<Domain, Target>();

        constexpr std::size_t N = domain_size<Domain>();

        if constexpr (std::is_same_v<Domain, MPUDomain>) {
            return Domain::template Init<N, cfg.mpu_cfgs>::instances[idx];
        } else if constexpr (std::is_same_v<Domain, ADCDomain>) {
            return Domain::template Init<N, cfg.adc_cfgs>::instances[idx];
        } else if constexpr (std::is_same_v<Domain, DFSDM_CHANNEL_DOMAIN>) {
            return Domain::template Init<N, cfg.dfsdm_cfgs>::instances[idx];
        } else {
            return Domain::template Init<N>::instances[idx];
        }
    }
};

} // namespace ST_LIB

/**
 * @brief   This is a function that gets called early in the startup process,
 *          before the global constructors and main() are called.
 *          It is responsible for initializing the hardware and peripherals
 */
extern void BoardInit(void);
