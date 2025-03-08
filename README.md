# Description

This repository contains the **Unified Node Access (UNA)** implementation based on AT commands.

# Dependencies

The driver relies on:

* An external `types.h` header file defining the **standard C types** of the targeted MCU.
* The **UNA library** provided in the [una-lib](https://github.com/Ludovic-Lesur/una-lib) repository.
* The **embedded utility functions** defined in the [embedded-utils](https://github.com/Ludovic-Lesur/embedded-utils) repository.

Here is the versions compatibility table:

| **una-at** | **una-lib** | **embedded-utils** |
|:---:|:---:|:---:|
| [sw2.2](https://github.com/Ludovic-Lesur/una-at/releases/tag/sw2.2) | >= [sw2.1](https://github.com/Ludovic-Lesur/una-lib/releases/tag/sw2.1) | >= [sw7.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw7.0) |
| [sw2.1](https://github.com/Ludovic-Lesur/una-at/releases/tag/sw2.1) | [sw2.0](https://github.com/Ludovic-Lesur/una-lib/releases/tag/sw2.0) | [sw6.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw6.0) to [sw6.2](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw6.2) |
| [sw2.0](https://github.com/Ludovic-Lesur/una-at/releases/tag/sw2.0) | [sw2.0](https://github.com/Ludovic-Lesur/una-lib/releases/tag/sw2.0) | [sw6.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw6.0) to [sw6.2](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw6.2) |
| [sw1.0](https://github.com/Ludovic-Lesur/una-at/releases/tag/sw1.0) | [sw1.0](https://github.com/Ludovic-Lesur/una-lib/releases/tag/sw1.0) | [sw6.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw6.0) to [sw6.2](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw6.2) |

# Compilation flags

| **Flag name** | **Value** | **Description** |
|:---:|:---:|:---:|
| `UNA_AT_DISABLE_FLAGS_FILE` | `defined` / `undefined` | Disable the `una_at_flags.h` header file inclusion when compilation flags are given in the project settings or by command line. |
| `UNA_AT_DISABLE` | `defined` / `undefined` | Disable the UNA AT functions. |
| `UNA_AT_DELAY_ERROR_BASE_LAST` | `<value>` | Last error base of the low level delay driver. |
| `UNA_AT_MODE_MASTER` | `defined` / `undefined` | Enable master operating mode. |
| `UNA_AT_MODE_SLAVE` | `defined` / `undefined` | Enable slave operating mode. |
| `UNA_AT_USE_REGISTER_ACCESS` | `defined` / `undefined` | Enable register access definitions. |
| `UNA_AT_USE_REGISTER_ACCESS_TIMEOUT` | `defined` / `undefined` | Enable register access timeout definitions. |
| `UNA_AT_USE_REGISTER_ERROR_VALUE` | `defined` / `undefined` | Enable register error values definitions. |
| `UNA_AT_TERMINAL_INSTANCE` | `<value>` | Instance of the terminal used to transmit and receive commands. |
| `UNA_AT_CUSTOM_COMMANDS` | `defined` / `undefined` | Enable additional commands registering. |
