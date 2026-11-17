# VS Code + Keil 使用说明

本项目面向 STM32F407ZG（Cortex-M4F），使用 FreeRTOS 和 STM32 标准外设库。
VS Code 负责编辑、补全和调用构建；Keil uVision / ARM Compiler 5 负责实际编译、汇编和链接。

## 本机配置

- VS Code 扩展：Microsoft C/C++、Keil Assistant。
- Keil：`G:\keil_MDK\UV4\UV4.exe`。
- 编译器：`G:\keil_MDK\ARM\ARMCC\bin\armcc.exe`，5.06 update 7 (build 960)。
- 芯片包：`G:\keil_MDK\Keil\STM32F4xx_DFP\2.14.0`。
- 工程：`USER/FreeRTOS.uvprojx`，目标：`FreeRTOS`。
- 源文件编码：UTF-8；C 标准：C99。

Keil Assistant 的 `KeilAssistant.MDK.Uv4Path` 是机器级设置，已写入 VS Code **用户设置**。
更换电脑或 Keil 安装目录时，应修改该设置，并核对 `c_cpp_properties.json` 中的 ARMCC 头文件路径。

## 日常使用

**仅编译当前文件：Ctrl+F5。** 打开工程中的 `.c` 文件并让编辑器获得焦点后按此快捷键，
VS Code 会先保存文件，再运行 `Keil: Compile Current C File` 任务。该快捷键仅在本项目的
C 编辑器中生效；此时替代 VS Code 原来的“不调试运行”。头文件不能单独编译。
单文件编译直接调用 ARM Compiler 5，输出在 `OBJ/single-file/`，不链接、不生成 HEX，
也不覆盖完整工程使用的对象文件。生成固件仍使用 `Ctrl+Shift+B`。
快捷键已写入本机 VS Code 用户 `keybindings.json`，不需要安装额外扩展。

从项目根目录仅编译指定文件：

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .vscode/keil-compile-file.ps1 -SourceFile USER/main.c
```

1. 在 VS Code 打开整个 `DR-FreeRTOS` 文件夹，执行一次 `Developer: Reload Window`。
2. 按 `Ctrl+Shift+B`，运行默认任务 `Keil: Build FreeRTOS`。
3. 如需全部重编译，通过 `Terminal > Run Task` 选择 `Keil: Rebuild FreeRTOS`。
4. 也可在 Keil Assistant 工程视图选择 `FreeRTOS` 并执行 Build / Rebuild。
   若插件未自动发现工程，使用 `Open keil uVision project` 打开 `USER/FreeRTOS.uvprojx`。
5. 错误和警告显示在终端与 Problems 面板；默认任务的完整日志在 `OBJ/vscode-build.log`。

也可以从项目根目录运行：

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .vscode/keil-build.ps1 -Action Rebuild
```

Keil Assistant 会依据工程同步头文件路径和宏定义。增加源文件或修改目标配置时，
在 Keil 中保存工程后再返回 VS Code。不要用单文件的“运行 C 文件”命令编译该固件。
固件运行在 STM32 上；本次未配置或执行硬件烧录和调试。

## 2026-11-16 验证结果

完成 Keil 授权后，已重新运行完整重编译：编译、汇编、链接及 HEX 生成全部成功。
源码有 56 条警告，主要涉及文件末尾换行、未使用变量和隐式函数声明，本次未修改业务代码。

构建结果：

```text
Program Size: Code=33920 RO-data=1072 RW-data=432 ZI-data=29944
"..\OBJ\LED.axf" - 0 Error(s), 56 Warning(s).
```

已生成新的 `OBJ/LED.axf` 和 `OBJ/LED.hex`，此前的 Lite 32 KB 链接限制已不再阻止构建。
日志保存在 `OBJ/vscode-build.log`。此结果验证了编译环境，尚未进行硬件烧录或运行验证。

为匹配已安装的编译器，只把工程的版本选择从 5.06 update 6 改为 5.06 update 7，
保留了原有 ARMCC 工具链、源码、优化等级、链接布局和其他工程选项。
原工程文件备份位于 `.vscode/backups/FreeRTOS.uvprojx.before-toolchain`；
Keil 配置备份为 `G:\keil_MDK\TOOLS.INI.before-armcc5`。
