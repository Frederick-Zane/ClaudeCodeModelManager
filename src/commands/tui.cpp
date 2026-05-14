#include "commands/tui.h"
#include "config/claude_config.h"
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>

namespace cmd {

enum class View { MENU, ADD_MODEL, ADD_PROVIDER };

static void do_switch(AppConfig& config, const ModelConfig& target, const ProviderConfig& provider) {
    ClaudeCodeAdapter adapter;
    if (!adapter.load()) { return; }
    adapter.switch_model(target, provider);
    config.set_current_model(target.name);
    config.save();
}

int tui(AppConfig& config) {
    using namespace ftxui;

    if (config.models().empty()) {
        std::cerr << "ERROR: no models configured" << std::endl;
        return EXIT_FAILURE;
    }

    int selected = 0;
    View view = View::MENU;
    std::string status_msg;
    auto status_time = std::chrono::steady_clock::now();

    // Add-model inputs
    std::string am_name, am_provider, am_model;
    auto am_name_in = Input(&am_name, "name");
    auto am_prov_in = Input(&am_provider, "provider");
    auto am_model_in = Input(&am_model, "model_id");
    auto am_container = Container::Vertical({am_name_in, am_prov_in, am_model_in});

    // Add-provider inputs
    std::string ap_name, ap_url, ap_token;
    auto ap_name_in = Input(&ap_name, "name");
    auto ap_url_in = Input(&ap_url, "base_url");
    auto ap_token_in = Input(&ap_token, "auth_token");
    auto ap_container = Container::Vertical({ap_name_in, ap_url_in, ap_token_in});

    // Helper to rebuild menu entries
    auto build_entries = [&]() -> std::vector<std::string> {
        std::vector<std::string> entries;
        for (const auto& m : config.models()) {
            std::string label = m.name;
            if (m.name == config.current_model_name()) label += "  [current]";
            entries.push_back(label);
        }
        return entries;
    };

    auto entries = build_entries();
    auto menu = Menu(&entries, &selected);

    // Main view renderer
    auto menu_renderer = Renderer(menu, [&] {
        const auto& models = config.models();
        if (models.empty()) {
            return vbox({text("No models configured. Press 'a' to add one.") | center}) | border;
        }
        if (selected >= static_cast<int>(models.size())) selected = 0;

        const auto& sel = models[selected];
        const auto* sp = config.get_provider(sel.provider);
        Element info = vbox({
            text("Provider: " + sel.provider) | bold,
            sp ? text("URL: " + sp->base_url) : text("URL: (unknown)"),
            text("Model ID: " + sel.model),
        });

        // Status message (fades after 3 seconds)
        Element status = text("");
        if (!status_msg.empty()) {
            auto elapsed = std::chrono::steady_clock::now() - status_time;
            if (elapsed < std::chrono::seconds(3)) {
                status = text(status_msg) | bold | color(Color::Green);
            }
        }

        return vbox({
            text("ccm - Model Switcher") | bold | center,
            separator(),
            menu->Render(),
            separator(),
            info | dim,
            separator(),
            status,
            text("Enter: switch | a: add-model | p: add-provider | r: remove | q: quit") | center | dim,
        }) | border;
    });

    // Add-model view renderer
    auto am_renderer = Renderer(am_container, [&] {
        return vbox({
            text("Add Model") | bold | center,
            separator(),
            hbox({text("  Name:     "), am_name_in->Render()}),
            hbox({text("  Provider: "), am_prov_in->Render()}),
            hbox({text("  Model ID: "), am_model_in->Render()}),
            separator(),
            text("Tab: next field | Enter: save | Esc: cancel") | center | dim,
        }) | border;
    });

    // Add-provider view renderer
    auto ap_renderer = Renderer(ap_container, [&] {
        return vbox({
            text("Add Provider") | bold | center,
            separator(),
            hbox({text("  Name:      "), ap_name_in->Render()}),
            hbox({text("  Base URL:  "), ap_url_in->Render()}),
            hbox({text("  Auth Token: "), ap_token_in->Render()}),
            separator(),
            text("Tab: next field | Enter: save | Esc: cancel") | center | dim,
        }) | border;
    });

    // Top-level container that switches between views
    auto main_container = Container::Tab(
        {menu_renderer, am_renderer, ap_renderer},
        reinterpret_cast<int*>(&view)
    );

    auto main_renderer = Renderer(main_container, [&] {
        switch (view) {
            case View::MENU: return menu_renderer->Render();
            case View::ADD_MODEL: return am_renderer->Render();
            case View::ADD_PROVIDER: return ap_renderer->Render();
        }
        return text("");
    });

    auto screen = ScreenInteractive::Fullscreen();

    main_renderer |= CatchEvent([&](Event event) {
        // Global quit
        if (view != View::MENU && (event == Event::Escape)) {
            view = View::MENU;
            return true;
        }

        if (view == View::MENU) {
            if (event == Event::Character('q')) {
                screen.ExitLoopClosure()();
                return true;
            }
            if (event == Event::Return) {
                // Switch model
                const auto& models = config.models();
                if (!models.empty() && selected < static_cast<int>(models.size())) {
                    const auto& target = models[selected];
                    const auto* provider = config.get_provider(target.provider);
                    if (provider) {
                        do_switch(config, target, *provider);
                        status_msg = "Switched to: " + target.name;
                        status_time = std::chrono::steady_clock::now();
                        entries = build_entries();
                    }
                }
                return true;
            }
            if (event == Event::Character('r')) {
                // Remove selected model
                const auto& models = config.models();
                if (!models.empty() && selected < static_cast<int>(models.size())) {
                    std::string name = models[selected].name;
                    if (config.current_model_name() == name) {
                        config.set_current_model("");
                        config.save();
                    }
                    if (config.remove_model(name)) {
                        status_msg = "Removed: " + name;
                        entries = build_entries();
                        if (selected >= static_cast<int>(config.models().size()))
                            selected = std::max(0, static_cast<int>(config.models().size()) - 1);
                    }
                    status_time = std::chrono::steady_clock::now();
                }
                return true;
            }
            if (event == Event::Character('a')) {
                am_name.clear(); am_provider.clear(); am_model.clear();
                view = View::ADD_MODEL;
                return true;
            }
            if (event == Event::Character('p')) {
                ap_name.clear(); ap_url.clear(); ap_token.clear();
                view = View::ADD_PROVIDER;
                return true;
            }
        }

        if (view == View::ADD_MODEL && event == Event::Return) {
            if (!am_name.empty() && !am_provider.empty() && !am_model.empty()) {
                if (!config.get_provider(am_provider)) {
                    status_msg = "ERROR: provider not found: " + am_provider;
                } else if (config.get_model(am_name)) {
                    status_msg = "ERROR: model already exists: " + am_name;
                } else if (config.add_model(am_name, am_provider, am_model)) {
                    status_msg = "Added model: " + am_name;
                    entries = build_entries();
                }
                status_time = std::chrono::steady_clock::now();
            }
            view = View::MENU;
            return true;
        }

        if (view == View::ADD_PROVIDER && event == Event::Return) {
            if (!ap_name.empty() && !ap_url.empty() && !ap_token.empty()) {
                if (config.get_provider(ap_name)) {
                    status_msg = "ERROR: provider already exists: " + ap_name;
                } else if (config.add_provider(ap_name, ap_url, ap_token)) {
                    status_msg = "Added provider: " + ap_name;
                }
                status_time = std::chrono::steady_clock::now();
            }
            view = View::MENU;
            return true;
        }

        return false;
    });

    screen.Loop(main_renderer);

    return EXIT_SUCCESS;
}

} // namespace cmd
