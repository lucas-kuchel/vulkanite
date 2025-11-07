#include <vulkanite/renderer/renderer.hpp>

#include <vulkanite/window/configuration.hpp>
#include <vulkanite/window/subsystem.hpp>
#include <vulkanite/window/window.hpp>

int main() {
    vulkanite::window::Subsystem subsystem;
    vulkanite::window::Window window;

    vulkanite::renderer::Instance instance;
    vulkanite::renderer::Surface surface;

    vulkanite::window::WindowCreateInfo windowCreateInfo = {
        .subsystem = subsystem,
        .visibility = vulkanite::window::Visibility::WINDOWED,
        .extent = {800, 600},
        .title = "Basic Test | Vulkanite",
        .resizable = false,
    };

    vulkanite::renderer::InstanceCreateInfo instanceCreateInfo = {
        .applicationName = window.getTitle(),
        .applicationVersionMajor = 0,
        .applicationVersionMinor = 1,
        .applicationVersionPatch = 0,
        .requestDebug = true,
    };

    vulkanite::renderer::SurfaceCreateInfo surfaceCreateInfo = {
        .instance = instance,
        .window = window,
    };

    subsystem.create();
    window.create(windowCreateInfo);

    instance.create(instanceCreateInfo);
    surface.create(surfaceCreateInfo);

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

    surface.destroy();
    instance.destroy();

    window.destroy();
    subsystem.destroy();
}