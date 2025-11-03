#pragma once

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

        Backend queryBackend() const;
    };
}