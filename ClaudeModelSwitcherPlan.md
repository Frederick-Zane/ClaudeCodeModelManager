Claude Code Model Manager（ccm）开发计划
项目目标

开发一个 Linux/WSL 原生 CLI 工具：

ccm

用于：

管理 Claude Code 使用的第三方模型
快速切换 provider / model
自动修改 Claude Code 配置
无 GUI
无 Node.js 依赖
单二进制部署
后续可扩展 TUI

本项目不是 Agent 系统。

本项目不是自动 Router。

本项目是：

Linux-native LLM profile switcher

技术栈
语言
C++20
CLI

使用：

CLI11

用于：

子命令
参数解析
help
validation
配置解析

使用：

toml++

配置格式统一使用 TOML。

JSON 处理

使用：

nlohmann/json

用于修改 Claude Code 配置文件。

TUI（后续阶段）

使用：

FTXUI

当前阶段先不实现。

MVP 范围（严格限制）

当前只实现：

ccm list
ccm current
ccm use <model>

以及：

配置读取
配置写入
配置备份
原子化更新
provider/model 注册

不实现：

benchmark
auto routing
agent
telemetry
插件系统
网络测速
AI 自动选择
目标用户体验
切换模型
ccm use r1

输出：

Switched model:
deepseek-r1 -> openrouter/deepseek/deepseek-r1
查看当前模型
ccm current

输出：

Current model: deepseek-r1
Provider: openrouter
查看所有模型
ccm list

输出：

* sonnet
  r1
  gpt41
配置文件设计

路径：

~/.config/ccm/config.toml
示例
[current]
model = "r1"

[providers.openrouter]
base_url = "https://openrouter.ai/api/v1"
api_key = "sk-xxx"

[providers.anthropic]
base_url = "https://api.anthropic.com"
api_key = "sk-ant-xxx"

[models.r1]
provider = "openrouter"
model = "deepseek/deepseek-r1"

[models.sonnet]
provider = "anthropic"
model = "claude-sonnet-4"
Claude Code 配置适配器

实现：

ClaudeCodeAdapter

职责：

定位 Claude Code 配置
读取 JSON
patch model/provider
backup
原子写入
原子写入要求

必须：

1. read
2. validate
3. backup
4. write temp file
5. fsync
6. rename

禁止直接覆盖。

Backup 设计

备份目录：

~/.config/ccm/backups/

格式：

claude-config-20260510-120301.json

仅保留最近：

20

份。

项目目录结构
ccm/
├── CMakeLists.txt
├── src/
│
├── include/
│
├── third_party/
│
├── src/main.cpp
│
├── src/commands/
│   ├── list.cpp
│   ├── current.cpp
│   └── use.cpp
│
├── src/config/
│   ├── app_config.cpp
│   ├── model_registry.cpp
│   └── claude_config.cpp
│
├── src/providers/
│   └── provider.cpp
│
├── src/utils/
│   ├── fs.cpp
│   ├── atomic_write.cpp
│   ├── backup.cpp
│   └── paths.cpp
│
└── tests/
类设计
ModelConfig
struct ModelConfig {
    std::string name;
    std::string provider;
    std::string model;
};
ProviderConfig
struct ProviderConfig {
    std::string name;
    std::string base_url;
    std::string api_key;
};
AppConfig

负责：

加载 TOML
提供 model/provider 查询

接口：

class AppConfig {
public:
    bool load();

    const ModelConfig* get_model(
        const std::string& name
    ) const;

    const ProviderConfig* get_provider(
        const std::string& name
    ) const;

    std::string current_model() const;

    void set_current_model(
        const std::string& name
    );
};
ClaudeCodeAdapter
class ClaudeCodeAdapter {
public:
    bool load();

    bool switch_model(
        const ModelConfig& model,
        const ProviderConfig& provider
    );

    bool backup();

    bool save_atomic();
};
CLI 命令实现
ccm list

功能：

列出所有模型
当前模型加 *
ccm current

功能：

输出当前模型
provider
real model id
ccm use <model>

功能：

1. 校验 model 是否存在
2. 读取 provider
3. backup Claude 配置
4. patch JSON
5. 原子写入
6. 更新 current model
7. 保存 TOML
错误处理要求

禁止：

silent failure

必须输出：

ERROR: model not found: xxx
ERROR: Claude config missing
ERROR: invalid JSON

返回：

EXIT_FAILURE
Linux/WSL 要求

必须支持：

WSL2
Ubuntu
Arch
Debian

路径统一使用：

std::filesystem

禁止硬编码：

/home/user
构建系统

使用：

CMake

要求：

Debug/Release
clang/gcc
static analysis friendly
推荐编译选项
set(CMAKE_CXX_STANDARD 20)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)
开发阶段
Phase 1（MVP）

实现：

config.toml
list
current
use
backup
atomic write

完成标准：

ccm use r1

能够真实切换 Claude Code 模型。

Phase 2

实现：

ccm add
ccm remove

以及：

TOML 更新
provider 管理
Phase 3

实现：

ccm tui

基于：

FTXUI

实现：

模型列表
回车切换
provider 信息显示
非目标（明确禁止）

不要实现：

AI 自动选择模型
benchmark
latency test
token/s test
plugin marketplace
云同步
Web UI
Electron
浏览器前端
telemetry
analytics
自动下载模型
代码风格

要求：

RAII
禁止裸 new/delete
优先 smart pointer
使用 std::expected / optional
使用 filesystem
错误显式处理
最终目标

最终产物：

/usr/local/bin/ccm

用户可以：

ccm use sonnet

在 1 秒内完成：

配置切换
Claude Code 生效
安全备份
无 GUI
无 Node.js runtime
无额外依赖环境

即可。