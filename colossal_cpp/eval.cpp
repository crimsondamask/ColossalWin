#include "colossal.h"
#include "link.h"
#include "lua/lua.hpp"
#include "lua/lualib.h"
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <limits.h>
#include <string.h>

extern "C"
{
#include "tinyexpr/tinyexpr.h"
}

void cl_eval_init(ClEval *eval)
{

    ClExprConfig exp_config = {};
    ClLuaConfig lua_config = {};
    snprintf(exp_config.expr, sizeof(exp_config.expr), "5.0 + 5.0");
    snprintf(lua_config.script, sizeof(lua_config.script), "out = 5.0 + 5.0");

    ClRandConfig rand_config = {};
    rand_config.max = 100;
    rand_config.min = 0;
    rand_config.seed = 1;

    ClIsoConfig iso_config = {};

    ClEvalConfig eval_config = {
        .rand_config = rand_config, .iso_config = iso_config, .expr_config = exp_config, .lua_config = lua_config};

    snprintf(eval->name, sizeof(eval->name), "--");
    snprintf(eval->err_msg, sizeof(eval->err_msg), "Initial");
    snprintf(eval->tk, sizeof(eval->tk), "EV");
    eval->eval_config = eval_config;
    eval->enabled = false;
    eval->is_error = true;
    eval->value_type = CL_VALUE_REAL;
    eval->eval_type = CL_EXPR_EVAL;
    eval->result = TagValue{
        .int_value = 0,
        .real_value = 0.0f,
        .bool_value = 0,
    };
}

void cl_evaluate(ClEval *eval, ClEval *list_of_evals, Link *list_of_links)
{
    if (!eval->enabled)
    {
        eval->is_error = true;
        snprintf(eval->err_msg, sizeof(eval->err_msg), "The calculation is not enabled.");
    }
    else
    {

        int err = 0;
        bool parse_failed = false;
        // te_expr *expr;
        char original_exp_str[2048] = {};
        char *found_substr;
        char *end_of_full_lk_tk;
        char *end_of_full_tag_tk;
        char prefix[2048] = {};
        char inter_buffer[2048] = {};
        char val_str_buf[32];
        long link_id;
        long tag_id;
        float tag_float_value;
        // int      tag_int_value;
        // bool     tag_bool_value;

        lua_State *L = luaL_newstate();

        switch (eval->eval_type)
        {
        // case CL_EXPR_EVAL:

        //     eval->is_error = false;

        //     strcpy(original_exp_str, eval->eval_config.expr_config.expr);
        //     snprintf(prefix, sizeof(prefix), " ");

        //     while ((found_substr = strstr(original_exp_str, "LK")) != NULL)
        //     {
        //         errno = 0;

        //         link_id = strtol(found_substr + 2, &end_of_full_lk_tk, 10);

        //         if ((errno != 0) || (end_of_full_lk_tk == found_substr + 2))
        //         {
        //             eval->is_error = true;
        //             snprintf(eval->err_msg, sizeof(eval->err_msg), "Parsing Tag token failed near: %s",
        //                      end_of_full_lk_tk);
        //             parse_failed = true;
        //             break;
        //         }

        //         if ((link_id > INT_MAX) || (link_id < INT_MIN))
        //         {
        //             eval->is_error = true;
        //             snprintf(eval->err_msg, sizeof(eval->err_msg),
        //                      "OUTSIDE OF RANGE. "
        //                      "failed near: %s",
        //                      found_substr);
        //             parse_failed = true;
        //             break;
        //         }

        //         if (link_id > N_DEVICES - 1)
        //         {
        //             eval->is_error = true;
        //             snprintf(eval->err_msg, sizeof(eval->err_msg),
        //                      "No device with such token. Parsing Tag token "
        //                      "failed near: %s",
        //                      found_substr);
        //             parse_failed = true;
        //             break;
        //         }

        //         errno = 0;
        //         tag_id = strtol(end_of_full_lk_tk + 1, &end_of_full_tag_tk, 10);

        //         if ((errno != 0) || (end_of_full_tag_tk == found_substr + 3))
        //         {
        //             eval->is_error = true;
        //             snprintf(eval->err_msg, sizeof(eval->err_msg), "Parsing Tag token failed near: %s",
        //                      end_of_full_tag_tk);
        //             parse_failed = true;
        //             break;
        //         }

        //         if ((tag_id > INT_MAX) || (link_id < INT_MIN))
        //         {
        //             eval->is_error = true;
        //             snprintf(eval->err_msg, sizeof(eval->err_msg),
        //                      "OUTSIDE OF RANGE. "
        //                      "failed near: %s",
        //                      found_substr);
        //             parse_failed = true;
        //             break;
        //         }

        //         if (tag_id > N_CHANNELS - 1)
        //         {
        //             eval->is_error = true;
        //             snprintf(eval->err_msg, sizeof(eval->err_msg),
        //                      "No Tag with such token. Parsing Tag token "
        //                      "failed near: %s",
        //                      found_substr);
        //             parse_failed = true;
        //             break;
        //         }

        //         // Look for the tag in the links data and copy its value.
        //         for (int i = 0; i < N_DEVICES; i++)
        //         {
        //             if (i == link_id)
        //             {
        //                 for (int j = 0; j < N_CHANNELS; j++)
        //                 {
        //                     if (j == tag_id)
        //                     {
        //                         switch (list_of_links[i].tags[j].value_type)
        //                         {
        //                         case CL_VALUE_REAL:
        //                             tag_float_value = list_of_links[i].tags[j].tag_value.real_value;
        //                             break;
        //                         case CL_VALUE_INT:
        //                             tag_float_value = (float)list_of_links[i].tags[j].tag_value.int_value;
        //                             break;
        //                         default:
        //                             tag_float_value = list_of_links[i].tags[j].tag_value.real_value;
        //                             break;
        //                         }
        //                     }
        //                 }
        //             }
        //         }

        //         snprintf(val_str_buf, sizeof(val_str_buf), " %.3f ", tag_float_value);

        //         // int  val_str_len = strlen(val_str_buf);
        //         // char full_tk_buf[16];

        //         int n = found_substr - original_exp_str;
        //         strncpy(prefix, original_exp_str, n);
        //         // printf("PREFIX: %s\n", prefix);

        //         snprintf(inter_buffer, sizeof(inter_buffer), "");
        //         strcat(inter_buffer, prefix);
        //         strcat(inter_buffer, val_str_buf);

        //         strcat(inter_buffer, end_of_full_tag_tk);
        //         // printf("FINAL INTER: %s\n", inter_buffer);

        //         // Update the expression.
        //         snprintf(original_exp_str, sizeof(original_exp_str), "%s", inter_buffer);
        //     }

        //     while ((found_substr = strstr(original_exp_str, "EV")) != NULL)
        //     {
        //         errno = 0;

        //         link_id = strtol(found_substr + 2, &end_of_full_lk_tk, 10);

        //         if ((errno != 0) || (end_of_full_lk_tk == found_substr + 2))
        //         {
        //             eval->is_error = true;
        //             snprintf(eval->err_msg, sizeof(eval->err_msg), "Parsing Tag token failed near: %s",
        //                      end_of_full_lk_tk);
        //             parse_failed = true;
        //             break;
        //         }

        //         if ((link_id > INT_MAX) || (link_id < INT_MIN))
        //         {
        //             eval->is_error = true;
        //             snprintf(eval->err_msg, sizeof(eval->err_msg),
        //                      "OUTSIDE OF RANGE. "
        //                      "failed near: %s",
        //                      found_substr);
        //             parse_failed = true;
        //             break;
        //         }

        //         if (link_id > N_EVALS - 1)
        //         {
        //             eval->is_error = true;
        //             snprintf(eval->err_msg, sizeof(eval->err_msg),
        //                      "No device with such token. Parsing Tag token "
        //                      "failed near: %s",
        //                      found_substr);
        //             parse_failed = true;
        //             break;
        //         }

        //         // Look for the eval in the evals data and copy its value.
        //         for (int i = 0; i < N_EVALS; i++)
        //         {
        //             if (i == link_id)
        //             {
        //                 switch (list_of_evals[i].value_type)
        //                 {
        //                 case CL_VALUE_REAL:
        //                     tag_float_value = list_of_evals[i].result.real_value;
        //                     break;
        //                 case CL_VALUE_INT:
        //                     tag_float_value = list_of_evals[i].result.int_value;
        //                     break;
        //                 default:
        //                     tag_float_value = list_of_evals[i].result.real_value;
        //                     break;
        //                 }
        //             }
        //         }

        //         snprintf(val_str_buf, sizeof(val_str_buf), " %.3f ", tag_float_value);

        //         int n = found_substr - original_exp_str;
        //         strncpy(prefix, original_exp_str, n);
        //         // printf("PREFIX: %s\n", prefix);

        //         snprintf(inter_buffer, sizeof(inter_buffer), "");
        //         strcat(inter_buffer, prefix);
        //         strcat(inter_buffer, val_str_buf);

        //         strcat(inter_buffer, end_of_full_lk_tk);
        //         // printf("FINAL INTER: %s\n", inter_buffer);

        //         // Update the expression.
        //         snprintf(original_exp_str, sizeof(original_exp_str), "%s", inter_buffer);
        //     }

        //     if (parse_failed)
        //     {
        //         break;
        //     }
        //     switch (eval->value_type)
        //     {
        //     case CL_VALUE_REAL:
        //         eval->is_error = false;
        //         expr = te_compile(original_exp_str, NULL, 0, &err);
        //         if (!expr)
        //         {
        //             eval->is_error = true;
        //             snprintf(eval->err_msg, sizeof(eval->err_msg), "Expression evaluation error near: %s",
        //                      eval->eval_config.expr_config.expr + err - 1);
        //             te_free(expr);
        //         }
        //         else
        //         {
        //             eval->is_error = false;
        //             eval->result.real_value = (float)te_eval(expr);
        //             te_free(expr);
        //         }
        //         break;
        //     case CL_VALUE_INT:
        //         eval->is_error = true;
        //         snprintf(eval->err_msg, sizeof(eval->err_msg),
        //                  "Expression evaluation only applied to REAL "
        //                  "values.");
        //         break;
        //     case CL_VALUE_BOOL:
        //         eval->is_error = true;
        //         snprintf(eval->err_msg, sizeof(eval->err_msg),
        //                  "Expression evaluation only applied to REAL "
        //                  "values.");
        //         break;
        //     default:
        //         break;
        //     }
        //     break;
        case CL_RAND_EVAL:
            switch (eval->value_type)
            {
            case CL_VALUE_REAL:
                eval->is_error = false;
                // Seed with the current timestamp;
                srand(time(NULL));
                eval->result.real_value =
                    (eval->eval_config.rand_config.min) +
                    ((float)rand() / (float)RAND_MAX *
                     (float)(eval->eval_config.rand_config.max - eval->eval_config.rand_config.min));
                break;
            case CL_VALUE_INT:
                eval->is_error = false;
                // Seed with the current timestamp;
                srand(time(NULL));
                eval->result.int_value =
                    (rand() % (eval->eval_config.rand_config.max - eval->eval_config.rand_config.min)) +
                    eval->eval_config.rand_config.min;
                break;
            case CL_VALUE_BOOL:
                eval->is_error = true;
                snprintf(eval->err_msg, sizeof(eval->err_msg),
                         "Random value generator only applied to INT and REAL "
                         "values.");
                break;
            default:
                break;
            }
            break;
        case CL_ISO5167_EVAL:
            break;
        case CL_LUA_EVAL:
            eval->is_error = false;

            strcpy(original_exp_str, eval->eval_config.lua_config.script);
            snprintf(prefix, sizeof(prefix), " ");

            while ((found_substr = strstr(original_exp_str, "LK")) != NULL)
            {
                errno = 0;

                link_id = strtol(found_substr + 2, &end_of_full_lk_tk, 10);

                if ((errno != 0) || (end_of_full_lk_tk == found_substr + 2))
                {
                    eval->is_error = true;
                    snprintf(eval->err_msg, sizeof(eval->err_msg), "Parsing Tag token failed near: %s",
                             end_of_full_lk_tk);
                    parse_failed = true;
                    break;
                }

                if ((link_id > INT_MAX) || (link_id < INT_MIN))
                {
                    eval->is_error = true;
                    snprintf(eval->err_msg, sizeof(eval->err_msg),
                             "OUTSIDE OF RANGE. "
                             "failed near: %s",
                             found_substr);
                    parse_failed = true;
                    break;
                }

                if (link_id > N_DEVICES - 1)
                {
                    eval->is_error = true;
                    snprintf(eval->err_msg, sizeof(eval->err_msg),
                             "No device with such token. Parsing Tag token "
                             "failed near: %s",
                             found_substr);
                    parse_failed = true;
                    break;
                }

                errno = 0;
                tag_id = strtol(end_of_full_lk_tk + 1, &end_of_full_tag_tk, 10);

                if ((errno != 0) || (end_of_full_tag_tk == found_substr + 3))
                {
                    eval->is_error = true;
                    snprintf(eval->err_msg, sizeof(eval->err_msg), "Parsing Tag token failed near: %s",
                             end_of_full_tag_tk);
                    parse_failed = true;
                    break;
                }

                if ((tag_id > INT_MAX) || (link_id < INT_MIN))
                {
                    eval->is_error = true;
                    snprintf(eval->err_msg, sizeof(eval->err_msg),
                             "OUTSIDE OF RANGE. "
                             "failed near: %s",
                             found_substr);
                    parse_failed = true;
                    break;
                }

                if (tag_id > N_CHANNELS - 1)
                {
                    eval->is_error = true;
                    snprintf(eval->err_msg, sizeof(eval->err_msg),
                             "No Tag with such token. Parsing Tag token "
                             "failed near: %s",
                             found_substr);
                    parse_failed = true;
                    break;
                }

                // Look for the tag in the links data and copy its value.
                for (int i = 0; i < N_DEVICES; i++)
                {
                    if (i == link_id)
                    {
                        for (int j = 0; j < N_CHANNELS; j++)
                        {
                            if (j == tag_id)
                            {
                                switch (list_of_links[i].tags[j].value_type)
                                {
                                case CL_VALUE_REAL:
                                    tag_float_value = list_of_links[i].tags[j].tag_value.real_value;
                                    break;
                                case CL_VALUE_INT:
                                    tag_float_value = (float)list_of_links[i].tags[j].tag_value.int_value;
                                    break;
                                default:
                                    tag_float_value = list_of_links[i].tags[j].tag_value.real_value;
                                    break;
                                }
                            }
                        }
                    }
                }

                snprintf(val_str_buf, sizeof(val_str_buf), " %.3f ", tag_float_value);

                // int  val_str_len = strlen(val_str_buf);
                // char full_tk_buf[16];

                int n = found_substr - original_exp_str;
                strncpy(prefix, original_exp_str, n);
                // printf("PREFIX: %s\n", prefix);

                snprintf(inter_buffer, sizeof(inter_buffer), "");
                strcat(inter_buffer, prefix);
                strcat(inter_buffer, val_str_buf);

                strcat(inter_buffer, end_of_full_tag_tk);
                // printf("FINAL INTER: %s\n", inter_buffer);

                // Update the expression.
                snprintf(original_exp_str, sizeof(original_exp_str), "%s", inter_buffer);
            }

            while ((found_substr = strstr(original_exp_str, "EV")) != NULL)
            {
                errno = 0;

                link_id = strtol(found_substr + 2, &end_of_full_lk_tk, 10);

                if ((errno != 0) || (end_of_full_lk_tk == found_substr + 2))
                {
                    eval->is_error = true;
                    snprintf(eval->err_msg, sizeof(eval->err_msg), "Parsing Tag token failed near: %s",
                             end_of_full_lk_tk);
                    parse_failed = true;
                    break;
                }

                if ((link_id > INT_MAX) || (link_id < INT_MIN))
                {
                    eval->is_error = true;
                    snprintf(eval->err_msg, sizeof(eval->err_msg),
                             "OUTSIDE OF RANGE. "
                             "failed near: %s",
                             found_substr);
                    parse_failed = true;
                    break;
                }

                if (link_id > N_EVALS - 1)
                {
                    eval->is_error = true;
                    snprintf(eval->err_msg, sizeof(eval->err_msg),
                             "No device with such token. Parsing Tag token "
                             "failed near: %s",
                             found_substr);
                    parse_failed = true;
                    break;
                }

                // Look for the eval in the evals data and copy its value.
                for (int i = 0; i < N_EVALS; i++)
                {
                    if (i == link_id)
                    {
                        switch (list_of_evals[i].value_type)
                        {
                        case CL_VALUE_REAL:
                            tag_float_value = list_of_evals[i].result.real_value;
                            break;
                        case CL_VALUE_INT:
                            tag_float_value = list_of_evals[i].result.int_value;
                            break;
                        default:
                            tag_float_value = list_of_evals[i].result.real_value;
                            break;
                        }
                    }
                }

                snprintf(val_str_buf, sizeof(val_str_buf), " %.3f ", tag_float_value);

                int n = found_substr - original_exp_str;
                strncpy(prefix, original_exp_str, n);
                // printf("PREFIX: %s\n", prefix);

                snprintf(inter_buffer, sizeof(inter_buffer), "");
                strcat(inter_buffer, prefix);
                strcat(inter_buffer, val_str_buf);

                strcat(inter_buffer, end_of_full_lk_tk);
                // printf("FINAL INTER: %s\n", inter_buffer);

                // Update the expression.
                snprintf(original_exp_str, sizeof(original_exp_str), "%s", inter_buffer);
            }

            if (parse_failed)
            {
                break;
            }
            switch (eval->value_type)
            {
            case CL_VALUE_REAL:
                eval->is_error = false;

                luaL_openlibs(L);
                if (luaL_loadbuffer(L, original_exp_str, strlen(original_exp_str), "eval") || lua_pcall(L, 0, 0, 0))
                {
                    eval->is_error = true;
                    snprintf(eval->err_msg, sizeof(eval->err_msg), "%s", lua_tostring(L, -1));
                    // fprintf(stderr, "%s", lua_tostring(L, -1));
                    lua_pop(L, 1);
                }

                lua_getglobal(L, "out");

                if (!lua_isnumber(L, -1))
                {
                    eval->is_error = true;
                    snprintf(eval->err_msg, sizeof(eval->err_msg), "out should be a number.");
                }

                eval->result.real_value = (float)lua_tonumber(L, -1);

                lua_close(L);
                break;
            case CL_VALUE_INT:
                eval->is_error = true;
                snprintf(eval->err_msg, sizeof(eval->err_msg),
                         "Expression evaluation only applied to REAL "
                         "values.");
                break;
            case CL_VALUE_BOOL:
                eval->is_error = true;
                snprintf(eval->err_msg, sizeof(eval->err_msg),
                         "Expression evaluation only applied to REAL "
                         "values.");
                break;
            default:
                break;
            }

            break;
        default:
            break;
        }
    }
}
