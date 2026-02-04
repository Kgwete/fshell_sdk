/**
 * FShell SDK - Hello World Example
 *
 * This example demonstrates the basics of embedding FShell into your
 * application:
 * - Creating a shell instance
 * - Registering custom commands
 * - Running an interactive shell
 *
 * Compile: See CMakeLists.txt in this directory
 * Run:     ./hello_world or .\hello_world.exe
 */

#include <cstdint>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "fn_api.h"

#ifdef __cplusplus
}
#endif

/* ============================================================================
 * Custom Command Handlers
 * ============================================================================
 */

/**
 * Simple "hello" command - demonstrates basic command registration
 */
FnResult cmd_hello(const FnCommandData *cmd, void *user_data) {
  FnAPI *api = (FnAPI *)user_data;

  // Check if user provided a name parameter
  const char *name = FN_GET_PARAM(cmd, "name");

  if (name) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "Hello, %s! Welcome to FShell.\n", name);
    fn_print(api, buffer);
  } else {
    fn_print(api, "Hello, World! Welcome to FShell.\n");
    fn_print(api, "Tip: Try 'hello name=YourName'\n");
  }

  return FN_OK;
}

/**
 * "greet" command - demonstrates accessing command flags
 */
FnResult cmd_greet(const FnCommandData *cmd, void *user_data) {
  FnAPI *api = (FnAPI *)user_data;

  const char *name = FN_GET_PARAM(cmd, "name");
  int formal = FN_HAS_FLAG(cmd, "formal");
  int excited = FN_HAS_FLAG(cmd, "excited");

  char buffer[256];

  if (!name) {
    name = "Friend";
  }

  // Build greeting based on flags
  if (formal && excited) {
    snprintf(buffer, sizeof(buffer),
             "Good day, %s! It is truly a pleasure to meet you!\n", name);
  } else if (formal) {
    snprintf(buffer, sizeof(buffer), "Good day, %s. A pleasure to meet you.\n",
             name);
  } else if (excited) {
    snprintf(buffer, sizeof(buffer), "Hey %s! Great to see you!!!\n", name);
  } else {
    snprintf(buffer, sizeof(buffer), "Hi %s, nice to meet you.\n", name);
  }

  fn_print(api, buffer);

  return FN_OK;
}

/**
 * "stats" command - demonstrates displaying formatted information
 */
FnResult cmd_stats(const FnCommandData *cmd, void *user_data) {
  FnAPI *api = (FnAPI *)user_data;

  fn_print(api, "\n");
  fn_print(api, "=== Application Statistics ===\n");
  fn_print(api, "Shell Version:    4.8.0\n");
  fn_print(api, "Commands Loaded:  20+ \n");
  fn_print(api, "Platform:         ");

#ifdef _WIN32
  fn_print(api, "Windows\n");
#elif __linux__
  fn_print(api, "Linux\n");
#elif __APPLE__
  fn_print(api, "macOS\n");
#else
  fn_print(api, "Unknown\n");
#endif

  fn_print(api, "Status:           Running\n");
  fn_print(api, "==============================\n\n");

  return FN_OK;
}

/* ============================================================================
 * Main Application
 * ============================================================================
 */

int main(int argc, char **argv) {
  printf("FShell SDK - Hello World Example\n");
  printf("=================================\n\n");

  // Create the shell instance
  FnAPI *api = fn_create("HelloWorld");
  if (!api) {
    fprintf(stderr, "ERROR: Failed to create FShell instance\n");
    return 1;
  }

  printf("FShell instance created successfully.\n");
  printf("API Version: 0x%08X\n", fn_api_version(api));

  // Check capabilities
  uint32_t caps = fn_api_capabilities();
  printf("Capabilities: ");
  if (caps & FN_CAP_COMMAND_REGISTRATION)
    printf("COMMANDS ");
  if (caps & FN_CAP_INTERACTIVE_SHELL)
    printf("INTERACTIVE ");
  if (caps & FN_CAP_PLUGIN_API)
    printf("PLUGINS ");
  if (caps & FN_CAP_DAEMON_MODE)
    printf("DAEMON");
  printf("\n\n");

  // Register custom commands
  printf("Registering custom commands...\n");

  FnResult result;

  result =
      fn_cmd_register(api, "hello", cmd_hello, api, "Say hello to someone");
  if (result != FN_OK) {
    fprintf(stderr, "ERROR: Failed to register 'hello' command\n");
    fn_destroy(api);
    return 1;
  }

  result = fn_cmd_register(api, "poke", cmd_greet, api,
                           "Poke someone with style (try -formal or -excited)");
  if (result != FN_OK) {
    fprintf(stderr, "ERROR: Failed to register 'poke' command\n");
    fn_destroy(api);
    return 1;
  }

  result = fn_cmd_register(api, "stats", cmd_stats, api,
                           "Display application statistics");
  if (result != FN_OK) {
    fprintf(stderr, "ERROR: Failed to register 'stats' command\n");
    fn_destroy(api);
    return 1;
  }

  printf("Commands registered successfully!\n\n");

  // Set a custom welcome header
  const char *welcome_header =
      "Welcome to HelloWorld Shell!\n"
      "Powered by FShell SDK\n"
      "\n"
      "Try these commands:\n"
      "  hello                  - Basic greeting\n"
      "  hello name=John        - Personalized greeting\n"
      "  greet name=Jane        - Casual greeting\n"
      "  greet name=Jane -formal - Formal greeting\n"
      "  greet -excited         - Enthusiastic greeting\n"
      "  stats                  - Show app statistics\n"
      "  fhelp                  - List all commands\n"
      "  exit                   - Quit the shell\n";

  fn_register_header(api, welcome_header);

  // Set to interactive mode (this is the default, but shown for clarity)
  fn_set_execution_mode(api, FN_MODE_INTERACTIVE, NULL);

  printf("Starting interactive shell...\n");
  printf("Type 'exit' or press Ctrl+C to quit.\n");
  printf("=====================================\n\n");

  // Run the shell - this blocks until the user exits
  result = fn_run(api);

  if (result != FN_OK) {
    fprintf(stderr, "\nERROR: Shell execution failed with code %d\n", result);
    fn_destroy(api);
    return 1;
  }

  // Clean up
  printf("\nShutting down shell...\n");
  fn_destroy(api);

  printf("Goodbye!\n");
  return 0;
}