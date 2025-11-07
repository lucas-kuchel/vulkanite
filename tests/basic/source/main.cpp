#include <vulkanite/window/configuration.hpp>
#include <vulkanite/window/subsystem.hpp>
#include <vulkanite/window/window.hpp>

#include <vulkanite/renderer/renderer.hpp>

int main() {
    vulkanite::window::Subsystem subsystem;

    vulkanite::window::WindowCreateInfo windowCreateInfo = {
        .subsystem = subsystem,
        .visibility = vulkanite::window::Visibility::WINDOWED,
        .extent = {800, 600},
        .title = "Basic Test | Vulkanite",
        .resizable = false,
    };

    vulkanite::window::Window window(windowCreateInfo);

    vulkanite::renderer::InstanceCreateInfo instanceCreateInfo = {
        .applicationName = window.getTitle(),
        .applicationVersionMajor = 0,
        .applicationVersionMinor = 1,
        .applicationVersionPatch = 0,
        .requestDebug = true,
    };

    vulkanite::renderer::Instance instance(instanceCreateInfo);

    vulkanite::renderer::SurfaceCreateInfo surfaceCreateInfo = {
        .instance = instance,
        .window = window,
    };

    vulkanite::renderer::Surface surface(surfaceCreateInfo);

    bool running = true;

    while (running) {
        subsystem.pollEvents();

        while (window.hasEvents()) {
            vulkanite::window::Event event = window.getNextEvent();

            switch (event.type) {
                case vulkanite::window::EventType::CLOSED:
                    running = false;
                    break;

                default:
                    break;
            }
        }
    }
}