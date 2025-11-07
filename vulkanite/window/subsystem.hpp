#pragma once

#include "../macros/cppstd.hpp"

#if VULKANITE_SUPPORTED

#include <cstdlib>

namespace vulkanite::window {
    enum class Backend : int;

    class Subsystem {
    public:
        Subsystem();
        ~Subsystem();

        Subsystem(const Subsystem&) = delete;
        Subsystem(Subsystem&&) noexcept = default;

        Subsystem& operator=(const Subsystem&) = delete;
        Subsystem& operator=(Subsystem&&) noexcept = default;

        void pollEvents();
        void awaitEvents();

        void awaitEventsTimeout(double timeout);

        [[nodiscard]] Backend queryBackend() const;

    private:
        static inline std::size_t subsystemCount_ = 0;
    };
}

#include "detail/subsystem.inl"

#endif