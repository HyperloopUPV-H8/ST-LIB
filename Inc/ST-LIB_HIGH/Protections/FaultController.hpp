#pragma once

#include "C++Utilities/CppUtils.hpp"
#include "StateMachine/StateMachine.hpp"

class FaultBroadcaster {
public:
    virtual ~FaultBroadcaster() = default;
    virtual bool broadcast_fault() = 0;
};

enum class FaultBroadcasterRegistrationError : uint8_t {
    CAPACITY_EXCEEDED = 0,
    STORAGE_TOO_SMALL,
};

class FaultController {
public:
    using state_id = uint8_t;
    static constexpr size_t max_broadcasters = 4;
    static constexpr size_t max_broadcaster_storage = 512;

    template <typename Broadcaster, typename... Args>
    static expected<Broadcaster*, FaultBroadcasterRegistrationError> emplace_broadcaster(
        Args&&... args
    ) {
        if (broadcaster_count >= max_broadcasters) {
            return unexpected(FaultBroadcasterRegistrationError::CAPACITY_EXCEEDED);
        }
        if constexpr (
            sizeof(Broadcaster) > max_broadcaster_storage ||
            alignof(Broadcaster) > alignof(std::max_align_t)) {
            return unexpected(FaultBroadcasterRegistrationError::STORAGE_TOO_SMALL);
        } else {
            BroadcasterStorage& slot = broadcaster_storage[broadcaster_count];
            auto* broadcaster = construct_at(
                reinterpret_cast<Broadcaster*>(slot.bytes.data()),
                std::forward<Args>(args)...
            );
            slot.broadcaster = broadcaster;
            slot.destroy = [](FaultBroadcaster* base) {
                destroy_at(static_cast<Broadcaster*>(base));
            };
            broadcasters[broadcaster_count++] = broadcaster;
            return broadcaster;
        }
    }

    static void link_state_machine(IStateMachine& general_state_machine, state_id fault_id);
    static void enter_fault();
    static void enter_external_fault();

    static void clear_broadcasters_for_testing();

private:
    struct BroadcasterStorage {
        alignas(std::max_align_t) array<byte, max_broadcaster_storage> bytes{};
        FaultBroadcaster* broadcaster{nullptr};
        void (*destroy)(FaultBroadcaster*){nullptr};

        void reset() {
            if (broadcaster != nullptr && destroy != nullptr) {
                destroy(broadcaster);
            }
            broadcaster = nullptr;
            destroy = nullptr;
        }
    };

    static IStateMachine* general_state_machine;
    static state_id fault_state_id;
    static array<BroadcasterStorage, max_broadcasters> broadcaster_storage;
    static array<FaultBroadcaster*, max_broadcasters> broadcasters;
    static size_t broadcaster_count;
};
