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

    using WindowExtent = glm::uvec2;
    using WindowTitle = std::string;

    class Subsystem;

    struct WindowCreateInfo {
        Subsystem& subsystem;
        Visibility visibility;
        WindowExtent extent;
        WindowTitle title;

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

        void setExtent(const glm::uvec2& extent);
        void setTitle(const std::string& title);
        void setVisibility(const Visibility& visibility);

        [[nodiscard]] const glm::uvec2& getExtent() const;
        [[nodiscard]] const std::string& getTitle() const;
        [[nodiscard]] const Visibility& getVisibility() const;

        [[nodiscard]] bool hasEvents() const;

        [[nodiscard]] Event getNextEvent();
        [[nodiscard]] GLFWwindow*& getHandle();

    private:
        WindowExtent extent_;
        WindowExtent lastWindowedExtent_;
        WindowTitle title_;
        std::queue<Event> events_;

        Visibility visibility_;
        Visibility lastVisibility_;

        GLFWwindow* handle_ = nullptr;

        static void resizeCallback(GLFWwindow* window, int width, int height);
        static void closeCallback(GLFWwindow* window);
        static void focusCallback(GLFWwindow* window, int focused);
        static void iconifyCallback(GLFWwindow* window, int iconified);
        static void keyCallback(GLFWwindow* window, int key, int, int action, int);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int);
        static void mousePositionCallback(GLFWwindow* window, double x, double y);
        static void mouseScrollCallback(GLFWwindow* window, double x, double y);

        [[nodiscard]] static Key mapKey(int key);
        [[nodiscard]] static MouseButton mapMouseButton(int button);

        friend class renderer::Surface;
    };
}