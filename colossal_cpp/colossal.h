#pragma once
#include "data_buffer.h"
#include "imgui/imgui.h"
#include "link.h"
#include "mb_device.h"
#include <stdbool.h>
#include <threads.h>

#define CURL_STATICLIB
#define CL_EXPR_LEN 512
#define N_EVALS 100

/// Special struct to hold data shared between threads.
/// This will be protected by a mutex.
typedef struct ThreadData {
    MbDevice device;
} ThreadData;

typedef struct ThreadArg {
    int           id;
    Buffer       *buf_ptr;
    Link          link;
    ConfigUpdate *config_update_ptr;
} ThreadArg;

typedef struct LoggingThreadArg {
    int           id;
    Link         *links;
    mtx_t         mtx;
    bool          is_error;
    char          error_msg[ERR_MSG_BUF_LEN];
    ConfigUpdate *config_update_ptr;
} LoggingThreadArg;

typedef struct UiMenuState {
    bool devices_menu;
    bool tag_menu;
    bool help_menu;
    bool logging_menu;
    bool plot_menu;
    bool tag_displays;
    bool tag_export;
    bool evals_menu;
} UiMenuState;

typedef enum ClEvalType {
    CL_EXPR_EVAL,
    CL_RAND_EVAL,
    CL_ISO5167_EVAL,
    CL_LUA_EVAL,
    // Other calcs will be added.
} ClEvalType;

// TODO:
// FIX: Work on the ISO Rate calc
typedef struct ClIsoConfig {
} ClIsoConfig;

// Generate random values bound by min and max with seed.
typedef struct ClRandConfig {
    long min;
    long max;
    long seed;
} ClRandConfig;

// Evaluate a mathematical expression.
typedef struct ClExprConfig {
    char expr[CL_EXPR_LEN];
} ClExprConfig;

typedef struct ClLuaConfig {
    char script[2048];
} ClLuaConfig;

typedef struct ClEvalConfig {
    ClRandConfig rand_config;
    ClIsoConfig  iso_config;
    ClExprConfig expr_config;
    ClLuaConfig  lua_config;
} ClEvalConfig;

typedef struct ClEval {
    bool         enabled;
    char         name[TAG_NAME_BUF_LEN];
    char         tk[16];
    int          value_type;
    int          eval_type;
    ClEvalConfig eval_config;
    TagValue     result;
    bool         is_error;
    char         err_msg[ERR_MSG_BUF_LEN];
} ClEval;

void cl_eval_init(ClEval *eval);
void cl_evaluate(ClEval *eval, ClEval list_of_evals[], Link list_of_links[]);
