# FShell SDK - Hello World Example

This is the simplest possible example of embedding FShell into your application.

## What This Example Demonstrates

- Creating an FShell instance with `fn_create()`
- Registering custom commands with `fn_cmd_register()`
- Accessing command parameters with `FN_GET_PARAM()`
- Checking command flags with `FN_GET_FLAG()`
- Printing output with `fn_print()`
- Running an interactive shell with `fn_run()`
- Cleaning up with `fn_destroy()`

## Custom Commands in This Example

1. **hello** - Basic greeting command
   ```
   hello                 # Generic greeting
   hello name=John       # Personalized greeting
   ```

2. **greet** - Greeting with style options
   ```
   greet name=Jane             # Casual greeting
   greet name=Jane -formal     # Formal greeting
   greet name=Jane -excited    # Enthusiastic greeting
   greet -formal -excited      # Both flags!
   ```

3. **stats** - Display application statistics
   ```
   info                 # Show platform and version info
   ```

## Building and Running

### Linux/macOS

```bash
mkdir build
cd build
cmake ..
make
./hello_world
```

### Windows (MSVC)

```powershell
mkdir build
cd build
cmake ..
cmake --build .
.\Debug\hello_world.exe
```

### Windows (MinGW)

```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
hello_world.exe
```

## Expected Output

When you run the program, you should see:

```
FShell SDK - Hello World Example
=================================

FShell instance created successfully.
API Version: 0x00010000
Capabilities: COMMANDS INTERACTIVE PLUGINS DAEMON

Registering custom commands...
Commands registered successfully!

Starting interactive shell...
Type 'exit' or press Ctrl+C to quit.
=====================================

Welcome to HelloWorld Shell!
Powered by FShell SDK

Try these commands:
  hello                  - Basic greeting
  hello name=John        - Personalized greeting
  greet name=Jane        - Casual greeting
  greet name=Jane -formal - Formal greeting
  greet -excited         - Enthusiastic greeting
  info                  - Show app statistics
  fhelp                  - List all commands
  exit                   - Quit the shell

HelloWorld>
```

Then you can try the commands:

```
HelloWorld> hello
Hello, World! Welcome to FShell.
Tip: Try 'hello name=YourName'

HelloWorld> hello name=Alice
Hello, Alice! Welcome to FShell.

HelloWorld> greet name=Bob -formal -excited
Good day, Bob! It is truly a pleasure to meet you!

HelloWorld> info

=== Application Info ===
Shell Version:    1.0.0
Commands Loaded:  6
Platform:         Windows
Status:           Running
==============================

HelloWorld> exit
Shutting down shell...
Goodbye!
```

## Next Steps

After understanding this example, check out:

- **02_custom_commands** - More advanced command handling
- **03_daemon_mode** - Running as a background service
- **04_plugin_system** - Creating loadable plugins

## Learn More

- Full API documentation: fn_api.h is a self documenting header file.
- Support: ifnet@florenet.co.za
