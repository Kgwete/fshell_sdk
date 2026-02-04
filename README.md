# FShell SDK

**The embeddable command shell library for modern applications**

FShell is a proprietory software developed by Florenet, (Sephaku. Groblersdal. Limpopo Province, South Africa) from 
March 2012 initially only used for private use and never released to the public until now. 
FShell SDK provides a professional, feature-rich command shell that you can embed into any C/C++ application. 
Stop reinventing command interfaces – use FShell and focus on your application logic.

---

## 🚀 Quick Start

### 5-Minute Integration

```c
#include "fn_api.h"

FnResult my_command(const FnCommandData* cmd, void* user_data) {
    FnAPI* api = (FnAPI*)user_data;
    fn_print(api, "Hello from my custom command!\n");
    return FN_OK;
}

int main() {
    FnAPI* shell = fn_create("MyApp");
    fn_cmd_register(shell, "mycommand", my_command, shell, "My custom command");
    fn_run(shell);  // Start interactive shell
    fn_destroy(shell);
    return 0;
}
```

**Build it:**

Linux/macOS:
```bash
gcc main.c -I fshell_sdk/include -L fshell_sdk/lib -lfnshell_r -o myapp
```

Windows (MinGW):
```powershell
gcc main.c -I fshell_sdk/include -L fshell_sdk/lib -lfnshell_r -lws2_32 -o myapp.exe
```

**That's it!** You now have a professional command shell in your application.

---

## ✨ Features

### Core Features
- ✅ **On Windows** - It is recommended you use PowerShell over CMD to enjoy fshell.
- ✅ **Interactive Mode** - Full-featured command shell with history and auto-completion
- ✅ **Event system** - fshell uses its own low level events system to detect and process mouse and keyboard signals
- ✅ **Daemon Mode** - Background service with IPC for remote control
- ✅ **Multi-Session** - Designed out of the box to handle multiple users simultaneously
- ✅ **Plugin Architecture** - Extend with loadable plugins
- ✅ **Clean C API** - Works with C, C++, Python, Rust, Go, Java, .NET, and more
- ✅ **Cross-Platform** - Windows and Linux (macOS coming soon)

### User Interface
- 📦 Beautiful box-drawn layouts
- 🎨 ANSI color support
- 🔄 Command history with arrow keys
- ⌨️ Tab or right arrow completion for commands
- 🖱️ Mouse scroll support (UI mode)
- 📜 Scrollable output buffer

### Built-in Commands
- `help` / `fhelp` - List all available commands
- `version` / `info` - Display version and system information
- `clear` / `cls` - Clear screen
- `exit` / `quit` - Exit the shell
- `history` - Show command history
- `calc` - Simple calculator
- `timer` - Countdown timer utility

---

## 📦 What's Included

```
fshell_sdk/
├── README.md                    # This file
├── LICENSE.txt                  # License terms
├── include/
│   └── fn_api.h                 # C API header (all you need to include)
├── lib/
	├──	win
│   ├──── libfnshell_d.a           # Debug build (with symbols)
│   ├──── libfnshell_r.a           # Release build (optimized)
	├── linux
	├──── libfnshell_d.a           # Debug build (with symbols)
	└──── libfnshell_r.a           # Release build (optimized)
├── examples/
│   └── 01_hello_world/          # Complete working example
│       ├── main.c
│       ├── CMakeLists.txt
│       └── README.md
```

---

## 🎯 Use Cases

### Perfect For:
- **Database Tools** - MySQL Workbench, pgAdmin-style admin consoles
- **Network Appliances** - Router/firewall configuration interfaces
- **Game Servers** - Admin consoles (like Minecraft server)
- **IoT Devices** - Device management shells
- **Development Tools** - CLI utilities and debugging tools
- **Embedded Systems** - Admin interfaces for embedded Linux
- **Monitoring Tools** - System administration dashboards

### Real-World Examples:
```c
// Database Admin Tool
fn_cmd_register(api, "query", execute_sql_query, ctx, "Execute SQL query");
fn_cmd_register(api, "tables", list_tables, ctx, "List all tables");

// Game Server Console
fn_cmd_register(api, "kick", kick_player, ctx, "Kick a player");
fn_cmd_register(api, "spawn", spawn_entity, ctx, "Spawn an entity");

// IoT Device Shell
fn_cmd_register(api, "status", device_status, ctx, "Show device status");
fn_cmd_register(api, "wifi", configure_wifi, ctx, "Configure WiFi");
```

---

## 📚 API Overview

### Core Functions

```c
// Create/destroy shell instance
FnAPI* fn_create(const char* app_name);
void fn_destroy(FnAPI* api);

// Configure execution mode
FnResult fn_set_execution_mode(FnAPI* api, FnExecutionMode mode,
                               const char* pipe_name);

// Register commands
FnResult fn_cmd_register(FnAPI* api, const char* command_name,
                        FnCommandHandler handler, void* user_data,
                        const char* help_text);

// Run the shell
FnResult fn_run(FnAPI* api);
FnResult fn_stop(FnAPI* api);

// Execute commands programmatically
FnResult fn_cmd_execute(FnAPI* api, const char* cmd);

// Output functions
void fn_print(FnAPI* api, const char* text);
```

### Command Handler Signature

```c
FnResult my_handler(const FnCommandData* cmd, void* user_data) {
    // Access parameters
    const char* value = FN_GET_PARAM(cmd, "key");

    // Check flags
    int verbose = FN_HAS_FLAG(cmd, "verbose");

    // Print output
    FnAPI* api = (FnAPI*)user_data;
    fn_print(api, "Command executed\n");

    return FN_OK;
}
```

---

## 🔧 Building Your Application

### GCC Using CMake (Recommended)

```cmake
cmake_minimum_required(VERSION 3.10)
project(MyApp)

# Include FShell SDK
set(FSHELL_SDK_DIR "path/to/fshell_sdk")
include_directories(${FSHELL_SDK_DIR}/include)

add_executable(myapp main.c)

# Link FShell (Debug or Release)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(myapp ${FSHELL_SDK_DIR}/lib/libfnshell_d.a)
else()
    target_link_libraries(myapp ${FSHELL_SDK_DIR}/lib/libfnshell_r.a)
endif()

# Platform-specific libraries
if(WIN32)
    target_link_libraries(myapp ws2_32)
elseif(UNIX)
    target_link_libraries(myapp pthread dl)
endif()
```

### Manual Compilation

**Linux:**
```bash
g++ main.cc -I fshell_sdk/include -L fshell_sdk/lib -lfnshell_r \
    -lpthread -ldl -o myapp
```

**Windows (MinGW/Clang):**
```bash
g++ main.cc -I fshell_sdk/include -L fshell_sdk/lib -lfnshell_r \
    -lws2_32 -o myapp.exe
```

---

## 🌐 Language Bindings

FShell's C API works with any language that supports C FFI:

### Python (via ctypes)
```python
from ctypes import *

fshell = CDLL("./libfnshell_r.so")
api = fshell.fn_create(b"PythonApp")
fshell.fn_run(api)
fshell.fn_destroy(api)
```

### Rust
```rust
use std::ffi::CString;
use std::os::raw::c_char;

#[link(name = "fshell_r")]
extern "C" {
    fn fn_create(name: *const c_char) -> *mut std::ffi::c_void;
    fn fn_run(api: *mut std::ffi::c_void) -> i32;
    fn fn_destroy(api: *mut std::ffi::c_void);
}

let name = CString::new("RustApp").unwrap();
let api = unsafe { fn_create(name.as_ptr()) };
unsafe { fn_run(api); }
unsafe { fn_destroy(api); }
```

### Go (via cgo)
```go
// #cgo LDFLAGS: -L. -lfnshell_r
// #include "fn_api.h"
import "C"

func main() {
    api := C.fn_create(C.CString("GoApp"))
    C.fn_run(api)
    C.fn_destroy(api)
}
```

---

## 📖 Examples

### Example 1: Simple Hello World
See `examples/01_hello_world/` for a complete working example with:
- Custom command registration
- Parameter handling
- Flag handling
- Formatted output

### Example 2: Daemon Mode (Coming Soon)
Run FShell as a background service with IPC:
```c
fn_set_execution_mode(api, FN_MODE_DAEMON, "myapp_ctrl");
fn_run(api);  // Runs as daemon, accepts commands via named pipe
```

### Example 3: Plugin System (Coming Soon)
Create loadable plugins that extend functionality without recompiling.

---

## 🛠️ System Requirements

### Minimum Requirements
- **Windows:** Windows 10 or later, MSVC 2019+, or MinGW-w64
- **Linux:** Kernel 3.10+, glibc 2.17+, GCC 9+ or Clang 10+
- **Build System:** CMake 3.10 or later
- **C/C++ Standard:** C17 / C++23

### Tested Configurations
- ✅ Windows 10/11 with MSVC 2019, 2022
- ✅ Windows 10/11 with MinGW-w64 GCC 11
- ✅ Ubuntu 20.04, 22.04 with GCC 9, 11, 13
- ✅ Debian 11, 12 with GCC 10, 12

---

## 📄 License

FShell SDK is distributed under a proprietary license that is **free for commercial use**.

**You CAN:**
- ✅ Use FShell in commercial applications
- ✅ Distribute applications that include FShell
- ✅ Create and sell plugins
- ✅ Use for both open source and proprietary projects

**You CANNOT:**
- ❌ Reverse engineer or decompile the library
- ❌ Create competing command shell products
- ❌ Redistribute the SDK itself (link to official source)

See [LICENSE.txt](LICENSE.txt) for complete terms.

---

## 🔌 Plugin Marketplace (Coming Soon)

Premium plugins will be available for purchase:
- **Database Manager** - SQL shell extensions
- **Cloud Connect** - AWS/Azure/GCP management
- **Monitoring Suite** - Metrics and alerting
- **FNetServ** - Secure collaboration server
- **etc**

Stay tuned or contact us for early access.

---

## 💬 Support & Community

### Getting Help
- 📧 **Email:** ifnet@florenet.co.za
- 🌐 **Website:** https://florenet.co.za/fshell
- 🐛 **Bug Reports:** Email with detailed reproduction steps

### Commercial Support
Professional and Enterprise support tiers available. Contact us for pricing.

---

## 🚦 Version History

### v4.8.0 (January 2025) - Initial public Release
- Interactive command shell
- Daemon mode with IPC
- Plugin architecture
- Multi-session support
- Cross-platform (Windows/Linux)
- Professional UI with boxes and colors

---

## 🙏 Attribution

Applications using FShell should include the following attribution:

```
Powered by FShell SDK - https://florenet.co.za/fshell
```

This requirement may be waived for small or internal applications.

---

## 🎓 Learn More

**Getting Started Guide:** https://florenet.co.za/fshell/tutorial
**API Reference:** https://florenet.co.za/fshell/api
**Best Practices:** https://florenet.co.za/fshell/best-practices
**FAQ:** https://florenet.co.za/fshell/faq

---

## 📞 Contact

**Author:** M.C. Kgwete (Florenet)
**Email:** ifnet@florenet.co.za
**Website:** https://florenet.co.za

For licensing inquiries, custom development, or partnership opportunities, please reach out via email.

---

**FShell SDK - Embed powerful command shells in any application**

*Copyright © 2025 Florenet. All rights reserved.*
