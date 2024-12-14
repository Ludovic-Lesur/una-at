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
| [sw1.0](https://github.com/Ludovic-Lesur/una-at/releases/tag/sw1.0) | >= [sw1.0](https://github.com/Ludovic-Lesur/una-lib/releases/tag/sw1.0) | >= [sw6.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw6.0) |

# Compilation flags

| **Flag name** | **Value** | **Description** |
|:---:|:---:|:---:|
| `UNA_AT_DISABLE_FLAGS_FILE` | `defined` / `undefined` | Disable the `una_at_flags.h` header file inclusion when compilation flags are given in the project settings or by command line. |
| `UNA_AT_DISABLE` | `defined` / `undefined` | Disable the UNA library. |
| `UNA_AT_DELAY_ERROR_BASE_LAST` | `<value>` | Last error base of the low level delay driver. |
| `UNA_AT_MODE_MASTER` | `defined` / `undefined` | Enable master operating mode. |
| `UNA_AT_MODE_SLAVE` | `defined` / `undefined` | Enable slave operating mode. |
| `UNA_AT_TERMINAL_INSTANCE` | `<value>` | Instance of the terminal used to transmit and receive data. |
| `UNA_AT_NODE_ADDRESS_LAST` | `<value>` | Last address value of the AT nodes. |
