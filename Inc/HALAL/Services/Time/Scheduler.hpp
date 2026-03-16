/*
 * Scheduler.hpp
 *
 * Created on: 17 nov. 2025
 *     Author: Victor (coauthor Stephan)
 */
#pragma once

/* Uso del scheduler, descrito en la wiki:
 * https://wiki.hyperloopupv.com/es/firmware/Timing/Scheduler */

#include "stm32h7xx_ll_tim_wrapper.h"

#include <array>
#include <cstdint>
#include <functional>

#ifndef SIM_ON
#include "stm32h7xx_hal_tim.h"
#endif

extern TIM_TypeDef* Scheduler_global_timer;
void Scheduler_global_timer_callback(void *raw);
void Scheduler_start(void);

struct Scheduler {
    using callback_t = void (*)();
    static constexpr uint32_t kMaxTasks = 16;
    // INVALID_ID must be an even multiple of kMaxTasks.
    // if it isn't it could theoretically be used as an id in set_timeout
    static constexpr uint32_t INVALID_ID = 2 * kMaxTasks;

    // temporary, will be removed
    [[deprecated]]
    static inline void start() {}
    static void update();
    static inline uint64_t get_global_tick() {
        return global_tick_us_ + Scheduler_global_timer->CNT;
    }

    static uint16_t register_task(uint32_t period_us, callback_t func);
    static bool unregister_task(uint16_t id);

    static uint16_t set_timeout(uint32_t microseconds, callback_t func);
    static bool cancel_timeout(uint16_t id);

    // internal
    static void on_timer_update();
    static void schedule_next_interval();
    static constexpr uint32_t FREQUENCY = 1'000'000u; // 1 MHz -> 1us precision
#ifndef SIM_ON
private:
#endif
    struct Task {
        uint32_t next_fire_us{0};
        callback_t callback{};
        uint32_t period_us{0};
        uint16_t id;
        bool repeating{false};
    };

    static_assert(
        ((INVALID_ID / kMaxTasks) % 2) == 0,
        "INVALID_ID must be an even multiple of kMaxTasks"
    );
    static_assert(INVALID_ID >= kMaxTasks, "INVALID_ID must not be a possible task id");

    static_assert((kMaxTasks & (kMaxTasks - 1)) == 0, "kMaxTasks must be a power of two");

    static std::array<Task, kMaxTasks> tasks_;
    static_assert(
        kMaxTasks == 16,
        "kMaxTasks must be 16, if more is needed, sorted_task_ids_ must change"
    );
    /* sorted_task_ids_ is a sorted queue with 4bits for each id in the
     * scheduler's current ids */
    static uint64_t sorted_task_ids_;

    static uint32_t active_task_count_;
    static_assert(
        kMaxTasks <= 32,
        "kMaxTasks must be <= 32, if more is needed, the bitmaps must change"
    );
    static uint32_t ready_bitmap_;
    static uint32_t free_bitmap_;
    static uint64_t global_tick_us_;
    static uint32_t current_interval_us_;
    static uint16_t timeout_idx_;

    static inline uint8_t allocate_slot();
    static inline void release_slot(uint8_t id);
    static void insert_sorted(uint8_t id);
    static void remove_sorted(uint8_t id);

    // helpers
    static inline uint8_t get_at(uint8_t idx);
    static inline void set_at(uint8_t idx, uint8_t id);
    static inline void pop_front();
    static inline uint8_t front_id();

    static inline void global_timer_disable();
    static inline void global_timer_enable();
};
