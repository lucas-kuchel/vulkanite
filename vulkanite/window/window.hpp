#pragma once

#include <queue>
#include <string>

#include <glm/glm.hpp>

struct GLFWwindow;

namespace vulkanite::renderer {
    class Surface;
}

namespace vulkanite::window {
    enum class Visibility : int;
    enum class EventType : int;
    enum class MouseButton : int;
    enum class Key : int;

    struct ResizeEventInfo {
        glm::uvec2 extent;
    };

    struct KeyPressedEventInfo {
        Key key;
    };

    struct KeyReleasedEventInfo {
        Key key;
    };

    struct MouseButtonPressedEventInfo {
        MouseButton button;
    };

    struct MouseButtonReleasedEventInfo {
        MouseButton button;
    };

    struct MouseScrolledEventInfo {
        glm::dvec2 offset;
    };

    struct MouseMovedEventInfo {
        glm::dvec2 position;
    };

    union EventInfo {
        ResizeEventInfo resize;
        KeyPressedEventInfo keyPress;
        KeyReleasedEventInfo keyRelease;
        MouseButtonPressedEventInfo mouseButtonPress;
        MouseButtonReleasedEventInfo mouseButtonRelease;
        MouseScrolledEventInfo mouseScroll;
        MouseMovedEventInfo mouseMove;
    };

    struct Event {
        EventType type;
        EventInfo info;
    };

    using Extent = glm::uvec2;
    using Title = std::string;
    using Handle = GLFWwindow*;
    using EventList = std::queue<Event>;

    class Subsystem;

    struct WindowCreateInfo {
        Subsystem& subsystem;
        Visibility visibility;
        Extent extent;
        Title title;

        bool resizable;
    };

    class Window {
    public:
        Window(const WindowCreateInfo& createInfo);
        ~Window();

        Window(const Window&) = delete;
        Window(Window&&) noexcept = default;

        Window& operator=(const Window&) = delete;
        Window& operator=(Window&&) noexcept = default;

        void setExtent(const Extent& extent);
        void setTitle(const Title& title);
        void setVisibility(const Visibility& visibility);

        [[nodiscard]] const Extent& getExtent() const;
        [[nodiscard]] const Title& getTitle() const;
        [[nodiscard]] const Visibility& getVisibility() const;

        [[nodiscard]] bool hasEvents() const;

        [[nodiscard]] Event getNextEvent();
        [[nodiscard]] Handle getHandle();

    private:
        Extent extent_;
        Extent lastWindowedExtent_;
        Visibility visibility_;
        Visibility lastVisibility_;
        Title title_;
        EventList events_;
        Handle handle_ = nullptr;

        static void resizeCallback(Handle window, int width, int height);
        static void closeCallback(Handle window);
        static void focusCallback(Handle window, int focused);
        static void iconifyCallback(Handle window, int iconified);
        static void keyCallback(Handle window, int key, int, int action, int);
        static void mouseButtonCallback(Handle window, int button, int action, int);
        static void mousePositionCallback(Handle window, double x, double y);
        static void mouseScrollCallback(Handle window, double x, double y);

        [[nodiscard]] static Key mapKey(int key);
        [[nodiscard]] static MouseButton mapMouseButton(int button);
    };
}

#include "detail/window.inl"