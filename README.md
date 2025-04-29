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
| [sw5.0](https://github.com/Ludovic-Lesur/una-at/releases/tag/sw5.0) | >= [sw2.2](https://github.com/Ludovic-Lesur/una-lib/releases/tag/sw2.2) | >= [sw7.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw7.0) |
| [sw4.0](https://github.com/Ludovic-Lesur/una-at/releases/tag/sw4.0) | >= [sw2.2](https://github.com/Ludovic-Lesur/una-lib/releases/tag/sw2.2) | >= [sw7.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw7.0) |
| [sw3.0](https://github.com/Ludovic-Lesur/una-at/releases/tag/sw3.0) | [sw2.1](https://github.com/Ludovic-Lesur/una-lib/releases/tag/sw2.1) | >= [sw7.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw7.0) |
| [sw2.4](https://github.com/Ludovic-Lesur/una-at/releases/tag/sw2.4) | [sw2.1](https://github.com/Ludovic-Lesur/una-lib/releases/tag/sw2.1) | >= [sw7.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw7.0) |
| [sw2.3](https://github.com/Ludovic-Lesur/una-at/releases/tag/sw2.3) | [sw2.1](https://github.com/Ludovic-Lesur/una-lib/releases/tag/sw2.1) | >= [sw7.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw7.0) |
| [sw2.2](https://github.com/Ludovic-Lesur/una-at/releases/tag/sw2.2) | [sw2.1](https://github.com/Ludovic-Lesur/una-lib/releases/tag/sw2.1) | >= [sw7.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw7.0) |
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
| `UNA_AT_TERMINAL_INSTANCE` | `<value>` | Instance of the terminal used to transmit and receive commands (master mode only). |
| `UNA_AT_SCAN_REGISTER_ADDRESS` | `<value>` | Address of the common register containing the nodes address and board ID (master mode only). |
| `UNA_AT_SCAN_REGISTER_MASK_NODE_ADDRESS` | `<value>` | Mask of the node address field of the common register (master mode only). |
| `UNA_AT_SCAN_REGISTER_MASK_BOARD_ID` | `<value>` | Mask of the board ID field of the common register (master mode only). |
| `UNA_AT_CUSTOM_COMMANDS` | `defined` / `undefined` | Enable additional commands registering (slave mode only). |
