# ccm - Claude Code Model Manager

A Linux/WSL native CLI tool for managing and switching Claude Code's third-party model providers.

No GUI. No Node.js. Single binary.

---

## Features

- **Quick switching** - Switch between models/providers in one command
- **TUI mode** - Interactive terminal UI with arrow key navigation
- **Auto backup** - Settings backed up before every modification
- **Atomic writes** - Safe config updates (write temp → fsync → rename)
- **Guard hook** - Blocks accidental writes to live Claude Code config files

## Install

Build from source:

```bash
git clone https://github.com/Frederick-Zane/ClaudeCodeModelManager 
cd ccm
cmake -B build
cmake --build build
```

Copy the binary:

```bash
sudo cp build/ccm /usr/local/bin/
```

### Dependencies

- C++20 compiler (GCC 13+ or Clang 16+)
- CMake 3.16+

Third-party libraries (included in `third_party/`):

- [CLI11](https://github.com/CLIUtils/CLI11) - CLI parsing
- [toml++](https://github.com/marzer/tomlplusplus) - TOML config
- [nlohmann/json](https://github.com/nlohmann/json) - JSON handling
- [FTXUI](https://github.com/ArthurSonzogni/FTXUI) - Terminal UI

## Quick Start

```bash
# Add a provider
ccm add-provider openrouter https://openrouter.ai/api/v1 sk-xxx

# Add a model
ccm add-model r1 openrouter deepseek/deepseek-r1

# Switch model (modifies ~/.claude/settings.json)
ccm use r1

# Or use the interactive TUI
ccm tui
```

## Commands

| Command | Description |
|---------|-------------|
| `ccm list` | List all models, current marked with `*` |
| `ccm current` | Show current model and provider |
| `ccm use <model>` | Switch model, update Claude Code config |
| `ccm tui` | Interactive TUI (arrow keys, Enter to switch) |
| `ccm add-model <name> <provider> <model_id>` | Add a model |
| `ccm add-provider <name> <base_url> <auth_token>` | Add a provider |
| `ccm remove <name>` | Remove a model or provider |

## TUI Keybindings

| Key | Action |
|-----|--------|
| Up/Down | Navigate |
| Enter | Switch to selected model |
| `a` | Add model form |
| `p` | Add provider form |
| `r` | Remove selected model |
| `q` / Esc | Quit |

## Configuration

Config file: `~/.config/ccm/config.toml`

```toml
[current]
model = "ClaudeSonnnet"

[providers.Anthropic]
base_url = "https://api.example.com/anthropic"
auth_token = "your-token-here"

[models.claudeSonnet]
provider = "Athropic"
model = "ClaudeSonnet"
```

## How It Works

`ccm use <model>` does the following:

1. Reads `~/.claude/settings.json`
2. Backs up to `~/.config/ccm/backups/`
3. Patches the `env` block (`ANTHROPIC_AUTH_TOKEN`, `ANTHROPIC_BASE_URL`, `ANTHROPIC_MODEL`)
4. Atomically writes back to `~/.claude/settings.json`

## Safety

- **Guard hook** blocks writes to live Claude Code config files from any code path other than the intended `ccm use` / `ccm tui` flow
- **Atomic write** (temp → fsync → rename) prevents corruption on crash
- **Auto backup** keeps the last 20 versions in `~/.config/ccm/backups/`

---

# ccm - Claude Code 模型管理器

一个 Linux/WSL 原生 CLI 工具，用于管理和切换 Claude Code 的第三方模型 provider。

无 GUI，无 Node.js 依赖，单二进制部署。

## 功能

- **快速切换** - 一条命令切换模型/provider
- **TUI 模式** - 交互式终端界面，方向键导航
- **自动备份** - 每次修改前自动备份配置
- **原子写入** - 安全更新配置（写临时文件 → fsync → 重命名）
- **守护hook** - 阻止误写正在使用的 Claude Code 配置文件

## 安装

从源码构建：

```bash
git clone https://github.com/YOUR_USERNAME/ccm.git
cd ccm
cmake -B build
cmake --build build
```

复制二进制：

```bash
sudo cp build/ccm /usr/local/bin/
```

### 依赖

- C++20 编译器（GCC 13+ 或 Clang 16+）
- CMake 3.16+

第三方库（已包含在 `third_party/` 中）：

- [CLI11](https://github.com/CLIUtils/CLI11) - 命令行解析
- [toml++](https://github.com/marzer/tomlplusplus) - TOML 配置
- [nlohmann/json](https://github.com/nlohmann/json) - JSON 处理
- [FTXUI](https://github.com/ArthurSonzogni/FTXUI) - 终端 UI

## 快速开始

```bash
# 添加 provider
ccm add-provider openrouter https://openrouter.ai/api/v1 sk-xxx

# 添加模型
ccm add-model r1 openrouter deepseek/deepseek-r1

# 切换模型（修改 ~/.claude/settings.json）
ccm use r1

# 或使用交互式 TUI
ccm tui
```

## 命令

| 命令 | 说明 |
|------|------|
| `ccm list` | 列出所有模型，当前模型标 `*` |
| `ccm current` | 显示当前模型和 provider |
| `ccm use <model>` | 切换模型，更新 Claude Code 配置 |
| `ccm tui` | 交互式 TUI（方向键选择，回车切换） |
| `ccm add-model <name> <provider> <model_id>` | 添加模型 |
| `ccm add-provider <name> <base_url> <auth_token>` | 添加 provider |
| `ccm remove <name>` | 删除模型或 provider |

## TUI 快捷键

| 按键 | 操作 |
|------|------|
| 上/下 | 导航 |
| 回车 | 切换到选中模型 |
| `a` | 添加模型表单 |
| `p` | 添加 provider 表单 |
| `r` | 删除选中模型 |
| `q` / Esc | 退出 |

## 配置文件

配置路径：`~/.config/ccm/config.toml`

```toml
[current]
model = "ClaudeSonnnet"

[providers.Anthropic]
base_url = "https://api.example.com/anthropic"
auth_token = "your-token-here"

[models.claudeSonnet]
provider = "Athropic"
model = "ClaudeSonnet"
```

## 工作原理

`ccm use <model>` 执行流程：

1. 读取 `~/.claude/settings.json`
2. 备份到 `~/.config/ccm/backups/`
3. 更新 `env` 块（`ANTHROPIC_AUTH_TOKEN`、`ANTHROPIC_BASE_URL`、`ANTHROPIC_MODEL`）
4. 原子写入 `~/.claude/settings.json`

## 安全机制

- **守护hook** 阻止除 `ccm use` / `ccm tui` 以外的代码路径写入 live 配置文件
- **原子写入**（临时文件 → fsync → 重命名）防止崩溃时数据损坏
- **自动备份** 在 `~/.config/ccm/backups/` 保留最近 20 个版本

## License

MIT
