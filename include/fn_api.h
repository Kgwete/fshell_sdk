/**
 * \file fn_api.h
 * \brief Function command API definitions.
 * ============================================================================
 * @author Author: Mr M.C. Kgwete
 * @brief email: ifnet@florenet.co.za
 * @date  Initial release: Mar 13, 2012
 */

// IMPORTANT: DO NOT MODIFY THIS FILE

#ifndef FN_API_H
#define FN_API_H

#include <stddef.h>
#include <stdint.h>

// ============================================================================
// API VERSIONING
// ============================================================================
/**
 * \brief Packed API version number.
 *
 * Layout: 0xMMMMmmpp
 *  - MMMM = major
 *  - mm   = minor
 *  - pp   = patch
 */
#define FN_API_VERSION                                                         \
  ((FN_API_VERSION_MAJOR << 16) | (FN_API_VERSION_MINOR << 8) |                \
   (FN_API_VERSION_PATCH))

// ============================================================================
// SHARED LIBRARY VISIBILITY / EXPORTS
// ============================================================================
#ifdef _WIN32
#ifdef FN_BUILD_DLL
#define ENGINE_API __declspec(dllexport)
#else
// Static build - no import/export
#define ENGINE_API
#endif
#else
#define ENGINE_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// ERROR HANDLING
// ============================================================================
/**
 * \brief Result codes returned by API functions and command handlers.
 *
 * FN_OK indicates success. All other values indicate failure.
 *
 * \note This enum is part of the ABI. Do not reorder existing values.
 *       Only append new error codes.
 * \note There is a fn_result_string() helper to convert codes to strings
 *       that will not know appended error codes unless recompilation is
 * requested.
 */
typedef enum {
  FN_OK = 0,                 ///< Operation completed successfully
  FN_ERR_INVALID_ARGUMENT,   ///< One or more arguments were NULL or invalid
  FN_ERR_NOT_INITIALIZED,    ///< API or subsystem not initialized
  FN_ERR_ALREADY_REGISTERED, ///< Command already registered
  FN_ERR_INTERNAL,           ///< Internal error occurred
  FN_ERR_UNSUPPORTED,        ///< Feature not supported by this build
  FN_ERR_NOT_FOUND,          ///< IO error
  FN_ERR_NOT_AUTHENTICATED,  ///< Authentication error
  FN_ERR_PERMISSION_DENIED,  ///< Permission error
  FN_ERR_NOT_IMPLEMENTED     ///< Feature not implemented
} FnResult;

// ============================================================================
// CAPABILITY FLAGS
// ============================================================================
/**
 * \brief Capability flags exposed by the runtime.
 *
 * These flags allow plugins to query which optional features
 * are supported by the host application at runtime.
 */
typedef enum {
  FN_CAP_COMMAND_REGISTRATION = 1u << 0, ///< Supports fn_cmd_register()
  FN_CAP_INTERACTIVE_SHELL = 1u << 1,    ///< Interactive shell available
  FN_CAP_PLUGIN_API = 1u << 2,           ///< Plugin API enabled
  FN_CAP_SIGNAL_SAFE_STOP = 1u << 3,     ///< fn_stop() is signal-safe
  FN_CAP_DAEMON_MODE = 1u << 4           ///< Daemon mode available
} FnCapability;

// ============================================================================
// EXECUTION MODES
// ============================================================================
/**
 * \brief Execution modes for the shell.
 *
 * These modes control how the shell runs when fn_run() is called.
 */
typedef enum {
  FN_MODE_INTERACTIVE = 0, ///< Interactive shell with prompt (default)
  FN_MODE_DAEMON = 1       ///< Background daemon with IPC server
} FnExecutionMode;

// ============================================================================
// DATA STRUCTURES
// ============================================================================
/**
 * \brief A simple key-value pair for string data.
 *
 * This structure represents a named parameter or subcommand value.
 * For example: {"filename", "document.txt"}
 *
 * \note Both key and value are null-terminated C strings.
 * \note The memory for these strings is managed by the command system.
 *       Do not free or modify the pointers.
 */
typedef struct {
  const char *key;   ///< Parameter name (e.g., "filename")
  const char *value; ///< Parameter value (e.g., "document.txt")
} FnKeyValue;

/**
 * \brief A boolean flag with a name.
 *
 * Represents command-line flags like `-v` or `--verbose`.
 * The `value` field is 1 for true (flag present) or 0 for false (flag absent).
 */
typedef struct {
  const char *name; ///< Flag name without dashes (e.g., "verbose")
  uint8_t value;    ///< 1 if flag is present, 0 if absent
} FnFlag;

/**
 * \brief ABI-safe view of parsed command data.
 *
 * This structure provides read-only access to a parsed command in a way that
 * is safe across DLL/ABI boundaries.
 *
 * \note All arrays are contiguous and can be iterated using their count fields.
 * \note The data is valid only for the duration of the command handler call.
 *       Do not store pointers to this data.
 */
typedef struct {
  const char *main_command; ///< Primary command name

  const FnKeyValue *subcommands; ///< Array of key-value parameters
  size_t subcommand_count;       ///< Number of elements in subcommands array

  const FnFlag *flags; ///< Array of boolean flags
  size_t flag_count;   ///< Number of elements in flags array
} FnCommandData;

// ============================================================================
// CALLBACK TYPES
// ============================================================================
/**
 * \brief Command handler function type.
 *
 * This callback is invoked when a registered command is executed.
 *
 * \param cmd       Parsed command data. Valid only during this call.
 * \param user_data User-provided context passed during registration.
 * \return          FN_OK on success, or an error code on failure.
 *
 * \warning Do not store pointers from `cmd` beyond this call.
 */
typedef FnResult (*FnCommandHandler)(const FnCommandData *cmd, void *user_data);

// ============================================================================
// OPAQUE HANDLE
// ============================================================================
/**
 * \brief Opaque handle to the command service instance.
 *
 * Create with fn_create() and destroy with fn_destroy().
 */
typedef struct FnAPI FnAPI;

// ============================================================================
// API FUNCTIONS
// ============================================================================
/**
 * \brief Get the API version supported by the runtime.
 *
 * \return Packed API version number (FN_API_VERSION).
 */
ENGINE_API uint32_t fn_api_version(FnAPI *api);

/**
 * \brief Query runtime capabilities.
 *
 * \return Bitmask of FnCapability flags.
 */
ENGINE_API uint32_t fn_api_capabilities(void);

/**
 * \brief Create a new command service instance.
 *
 * \param app_name Name of the application (display purposes).
 * \return New FnAPI instance on success, NULL on failure.
 */
ENGINE_API FnAPI *fn_create(const char *app_name);

/**
 * \brief Destroy a command service instance.
 *
 * \param api Instance to destroy. Safe to pass NULL.
 */
ENGINE_API void fn_destroy(FnAPI *api);

/**
 * \brief Create a custom header/welcome display.
 *
 * \param header Welcome header to display at the top(display purposes).
 * \return FN_OK on success or an error code on failure.
 */
ENGINE_API FnResult fn_register_header(FnAPI *api, const char *header);

/**
 * \brief Set the execution mode for the shell.
 *
 * Controls whether fn_run() starts an interactive shell or a daemon server.
 * Must be called before fn_run().
 *
 * \param api       Command service instance.
 * \param mode      Execution mode (FN_MODE_INTERACTIVE or FN_MODE_DAEMON).
 * \param pipe_name IPC pipe name for daemon mode (NULL uses default
 * "fshell_ctrl"). Ignored in interactive mode.
 * \return FN_OK on success or an error code on failure.
 *
 * \note In daemon mode, fn_run() will start an IPC server and process commands
 *       via the named pipe. The FnAPI instance remains active throughout the
 *       daemon's lifetime, avoiding per-command initialization overhead.
 *
 * \example
 * \code
 * FnAPI* api = fn_create("MyApp");
 * fn_set_execution_mode(api, FN_MODE_DAEMON, "myapp_ctrl");
 * fn_run(api);  // Runs as daemon with IPC server
 * fn_destroy(api);
 * \endcode
 */
ENGINE_API FnResult fn_set_execution_mode(FnAPI *api, FnExecutionMode mode,
                                          const char *pipe_name);

/**
 * \brief Register a command handler.
 *
 * \param api          Command service instance.
 * \param command_name Command name (case-sensitive).
 * \param handler      Handler callback.
 * \param user_data    User context pointer passed to handler.
 * \return FN_OK on success or an error code on failure.
 */
ENGINE_API FnResult fn_cmd_register(FnAPI *api, const char *command_name,
                                    FnCommandHandler handler, void *user_data,
                                    const char *help_text);

/**
 * \brief Start the interactive command shell.
 *
 * \param api Command service instance.
 * \return FN_OK on success or an error code on failure.
 */
ENGINE_API FnResult fn_run(FnAPI *api);

/**
 * \brief Stop the running command shell.
 *
 * \param api Command service instance.
 * \return FN_OK on success or an error code on failure.
 */
ENGINE_API FnResult fn_stop(FnAPI *api);

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================
/**
 * \brief Execute a specific command.
 *
 * \param cmd Parsed command (must not be NULL).
 * \return    Value if found, NULL otherwise.
 */
ENGINE_API FnResult fn_cmd_execute(FnAPI *api, const char *cmd);
ENGINE_API void fn_print(FnAPI *api, const char *text);

/**
 * \brief Get the current session ID (useful in command handlers)
 */
ENGINE_API int fn_get_current_session_id(FnAPI *api);

/**
 * \brief Associate the current thread with a session for output capture.
 */
ENGINE_API void fn_set_thread_session_id(FnAPI *api, int session_id);

/**
 * \brief Clear the current thread's session association.
 */
ENGINE_API void fn_clear_thread_session_id(FnAPI *api);

/**
 * \brief Find a parameter value by key.
 *
 * \param cmd Parsed command data (may be NULL).
 * \param key Parameter key to search for (may be NULL).
 * \return    Value if found, NULL otherwise.
 */
ENGINE_API const char *fn_get_param(const FnCommandData *cmd, const char *key);

/**
 * \brief Check if a flag is present.
 *
 * \param cmd  Parsed command data (may be NULL).
 * \param flag Flag name to search for (may be NULL).
 * \return     1 if flag is present, 0 otherwise.
 */
ENGINE_API uint8_t fn_has_flag(const FnCommandData *cmd, const char *flag);

/**
 * \brief Retrieves result string.
 *
 * \param result Result code to query.
 * \param out_string Pointer to a string pointer that will be set on success.
 * \return   FN_OK on success, or an error code on failure.
 */
ENGINE_API void fn_result_string(FnResult result, const char **out_string);

// ============================================================================
// UTILITY MACROS (OPTIONAL CONVENIENCE ALIASES)
// ============================================================================

/**
 * \brief Helper to find a parameter value by key.
 *
 * Convenience macro that forwards to fn_get_param().
 */
#define FN_GET_PARAM(cmd, key) fn_get_param((cmd), (key))

/**
 * \brief Check if a flag is present.
 *
 * Convenience macro that forwards to fn_has_flag().
 */
#define FN_HAS_FLAG(cmd, flag) fn_has_flag((cmd), (flag))

#ifdef __cplusplus
}
#endif

#endif // FN_API_H