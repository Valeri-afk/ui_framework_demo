#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>

#include <SDL3/SDL.h>

#include "ui_framework/ui_manager.hpp"
#include "ui_framework/components/button.hpp"
#include "ui_framework/stack_panel_node.hpp"
#include "ui_framework/types.hpp"

namespace
{
constexpr int kWindowWidth = 900;
constexpr int kWindowHeight = 600;
constexpr int kSmokeFrames = 30;

class DemoRoot final : public ui::StackPanelNode
{
public:
    DemoRoot()
        : ui::StackPanelNode(Orientation::Vertical)
    {
        setGap(16.0f);
        setPadding({24.0f, 24.0f, 24.0f, 24.0f});
        setSize(ui::LayoutSizeValue::fixed(
            static_cast<float>(kWindowWidth),
            static_cast<float>(kWindowHeight)));
        setCrossAlignment(ui::CrossAxisAlignment::CENTER);
    }
};
}

int main(int argc, char** argv)
{
    const bool smokeMode =
        argc > 1 && argv[1] != nullptr && std::strcmp(argv[1], "--smoke") == 0;

    const int compiledMajor = SDL_MAJOR_VERSION;
    const int compiledMinor = SDL_MINOR_VERSION;
    const int compiledPatch = SDL_MICRO_VERSION;

    std::printf(
        "SDL headers: %d.%d.%d\n",
        compiledMajor,
        compiledMinor,
        compiledPatch);

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (!SDL_CreateWindowAndRenderer(
            smokeMode ? "ui_framework smoke test" : "ui_framework demo",
            kWindowWidth,
            kWindowHeight,
            SDL_WINDOW_RESIZABLE,
            &window,
            &renderer))
    {
        std::fprintf(
            stderr,
            "SDL_CreateWindowAndRenderer failed: %s\n",
            SDL_GetError());
        SDL_Quit();
        return 1;
    }

    ui::UIManager uiManager;

    auto root = std::make_unique<DemoRoot>();

    auto button = std::make_unique<ui::Button>();
    button->setText("SDL compatibility + UI Framework");
    button->setSize(ui::LayoutSizeValue::fixed(420.0f, 64.0f));
    button->setBackgroundColor(ui::Colors::camel);
    button->setTextColor(ui::Colors::white);

    button->on<ui::ButtonActivatedEvent>([](ui::ButtonActivatedEvent&, ui::Node&)
    {
        std::puts("Button activated - UI Framework consumer path is alive.");
    });

    root->addChild(std::move(button), root->getChildCount());
    uiManager.addRoot(std::move(root));

    bool running = true;
    int frames = 0;
    std::uint64_t previousTicks = SDL_GetTicks();

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;

            uiManager.processEvent(event, renderer);
        }

        const std::uint64_t currentTicks = SDL_GetTicks();
        const float dt = static_cast<float>(currentTicks - previousTicks) / 1000.0f;
        previousTicks = currentTicks;

        uiManager.advanceTime(dt);

        SDL_SetRenderDrawColor(renderer, 32, 34, 38, 255);
        SDL_RenderClear(renderer);
        uiManager.render(renderer);
        SDL_RenderPresent(renderer);

        ++frames;
        if (smokeMode && frames >= kSmokeFrames)
        {
            running = false;
        }
    }

    if (smokeMode)
    {
        std::puts("ui_framework smoke test completed successfully.");
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
