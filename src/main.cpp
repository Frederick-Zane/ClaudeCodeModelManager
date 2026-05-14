#include <CLI/CLI.hpp>
#include <iostream>
#include "config/app_config.h"
#include "commands/list.h"
#include "commands/current.h"
#include "commands/use.h"
#include "commands/add.h"
#include "commands/remove.h"
#include "commands/tui.h"

int main(int argc, char** argv) {
    CLI::App app{"Claude Code Model Manager"};
    app.require_subcommand(1);

    // ccm list
    app.add_subcommand("list", "List all models")->callback([&]() {
        AppConfig config;
        if (!config.load()) return EXIT_FAILURE;
        return cmd::list(config);
    });

    // ccm current
    app.add_subcommand("current", "Show current model")->callback([&]() {
        AppConfig config;
        if (!config.load()) return EXIT_FAILURE;
        return cmd::current(config);
    });

    // ccm use <model>
    std::string model_name;
    auto* use_cmd = app.add_subcommand("use", "Switch to a model");
    use_cmd->add_option("model", model_name, "Model name")->required();
    use_cmd->callback([&]() {
        AppConfig config;
        if (!config.load()) return EXIT_FAILURE;
        return cmd::use(config, model_name);
    });

    // ccm add model <name> <provider> <model_id>
    std::string add_model_name, add_model_provider, add_model_id;
    auto* add_model_cmd = app.add_subcommand("add-model", "Add a model");
    add_model_cmd->add_option("name", add_model_name, "Model alias")->required();
    add_model_cmd->add_option("provider", add_model_provider, "Provider name")->required();
    add_model_cmd->add_option("model_id", add_model_id, "Model ID")->required();
    add_model_cmd->callback([&]() {
        AppConfig config;
        if (!config.load()) return EXIT_FAILURE;
        return cmd::add_model(config, add_model_name, add_model_provider, add_model_id);
    });

    // ccm add provider <name> <base_url> <auth_token>
    std::string add_prov_name, add_prov_url, add_prov_token;
    auto* add_prov_cmd = app.add_subcommand("add-provider", "Add a provider");
    add_prov_cmd->add_option("name", add_prov_name, "Provider name")->required();
    add_prov_cmd->add_option("base_url", add_prov_url, "API base URL")->required();
    add_prov_cmd->add_option("auth_token", add_prov_token, "Auth token")->required();
    add_prov_cmd->callback([&]() {
        AppConfig config;
        if (!config.load()) return EXIT_FAILURE;
        return cmd::add_provider(config, add_prov_name, add_prov_url, add_prov_token);
    });

    // ccm tui
    app.add_subcommand("tui", "Interactive model switcher")->callback([&]() {
        AppConfig config;
        if (!config.load()) return EXIT_FAILURE;
        return cmd::tui(config);
    });

    // ccm remove <name>
    std::string remove_name;
    auto* remove_cmd = app.add_subcommand("remove", "Remove a model or provider");
    remove_cmd->add_option("name", remove_name, "Name to remove")->required();
    remove_cmd->callback([&]() {
        AppConfig config;
        if (!config.load()) return EXIT_FAILURE;
        return cmd::remove(config, remove_name);
    });

    CLI11_PARSE(app, argc, argv);
    return EXIT_SUCCESS;
}
