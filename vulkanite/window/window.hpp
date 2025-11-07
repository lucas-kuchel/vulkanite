#pragma once

#include "../macros/cppstd.hpp"

#if VULKANITE_SUPPORTED

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

    using ExtentType = glm::uvec2;
    using TitleType = std::string;
    using HandleType = GLFWwindow*;
    using EventListType = std::queue<Event>;

    class Subsystem;

    struct WindowCreateInfo {
        Subsystem& subsystem;
        Visibility visibility;
        ExtentType extent;
        TitleType title;

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

        void setExtent(const ExtentType& extent);
        void setTitle(const TitleType& title);
        void setVisibility(const Visibility& visibility);

        [[nodiscard]] const ExtentType& getExtent() const;
        [[nodiscard]] const TitleType& getTitle() const;
        [[nodiscard]] const Visibility& getVisibility() const;
        [[nodiscard]] const HandleType& getHandle();

        [[nodiscard]] bool hasEvents() const;
        [[nodiscard]] Event getNextEvent();

    private:
        ExtentType extent_;
        ExtentType lastWindowedExtent_;
        Visibility visibility_;
        Visibility lastVisibility_;
        TitleType title_;
        EventListType events_;
        HandleType handle_ = nullptr;

        static void resizeCallback(HandleType window, int width, int height);
        static void closeCallback(HandleType window);
        static void focusCallback(HandleType window, int focused);
        static void iconifyCallback(HandleType window, int iconified);
        static void keyCallback(HandleType window, int key, int, int action, int);
        static void mouseButtonCallback(HandleType window, int button, int action, int);
        static void mousePositionCallback(HandleType window, double x, double y);
        static void mouseScrollCallback(HandleType window, double x, double y);

        [[nodiscard]] static Key mapKey(int key);
        [[nodiscard]] static MouseButton mapMouseButton(int button);
    };
}

#include "detail/window.inl"

#endif