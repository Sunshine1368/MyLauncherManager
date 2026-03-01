# LaunchPad Manager

**LaunchPad Manager** 是一款 Linux 桌面环境下的 .desktop 文件管理工具，提供类似 macOS 11 Big Sur 风格的图形界面。它能够扫描系统（`/usr/share/applications`）和用户（`~/.local/share/applications`）目录下的 .desktop 启动器文件，并仅展示包含有效图标的条目。您可以在一个清爽的左右分栏界面中查看、编辑、删除或新建启动器，右侧功能区采用类似 VSCode 的标签页切换（信息/编辑/新建），左侧列表直观显示图标、名称和简介。

![主界面预览](docs/screenshot-main.png) <!-- 请替换为实际截图 -->

---

## ✨ 特性

- **双域扫描**：一键切换查看系统级或用户级 .desktop 文件。
- **智能过滤**：只列出包含有效图标（主题图标或绝对路径图标）的启动器，避免无效条目。
- **macOS 风格界面**：
  - 圆角窗口、毛玻璃效果按钮、列表项悬停高亮。
  - 左侧列表：图标 + 大标题 + 灰色简介，清晰直观。
  - 右侧顶部导航栏（信息 / 编辑 / 新建），类似 VSCode 标签设计。
- **完整 CRUD 操作**：
  - **查看**：只读页展示启动器所有字段。
  - **编辑**：修改名称、命令、图标、简介等，自动保存到对应文件。
  - **新建**：在用户目录下创建全新的 .desktop 文件。
  - **删除**：移除选中的 .desktop 文件（仅限用户目录，系统文件受保护）。
- **实时监控**：可选启用文件系统监控，当目录内容变化时自动刷新列表。
- **权限友好**：编辑系统文件时会提示需要 root 权限，并引导用户以管理员身份运行或使用 `pkexec`（可扩展实现）。

---

## 🖼️ 截图

> 左侧列表 | 右侧信息页  
> ![信息页](docs/screenshot-info.png)

> 编辑页 | 新建页  
> ![编辑页](docs/screenshot-edit.png) | ![新建页](docs/screenshot-add.png)

---

## 🔧 依赖

- **Qt 5.15 或 Qt 6.2+**（模块：Core, Widgets, Gui）
- **CMake 3.16+** 或 **qmake**
- 编译器支持 C++17（如 GCC 9+, Clang 10+）
- Linux 桌面环境（用于测试图标主题和目录结构）

---

## 🚀 构建与安装

### 使用 CMake

```bash
git clone https://github.com/yourusername/launchpad-manager.git
cd launchpad-manager
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
# 可选安装到系统
sudo make install
