#include "lua-emacs.h"

#include "emacs-module.h"

#include "emacs-interaction.h"

#include <luajit-2.1/lua.h>
#include <luajit-2.1/lauxlib.h>
#include <luajit-2.1/lualib.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//here's a test case:
//(lua "elisp.functions['hello-world'](); return 'done!'")

#define FUCK_GPL int plugin_is_GPL_compatible
FUCK_GPL;

struct lua_funcall_userdata {
    emacs_env *env;
    char* function_name;
};
static int slow_arbitrary_lua_funcall(lua_State *L) {
    struct lua_funcall_userdata *userdata = lua_touserdata(L, lua_upvalueindex(1));
    emacs_value f_sym; 
    emacs_env *env = userdata->env;
    const char* function_name = userdata->function_name;
    emacs_value (*intern) (emacs_env*, const char*) = env->intern;
    f_sym = intern(env, function_name);
    if (f_sym != NULL) {
        intmax_t nargs = 0;
        emacs_value *args = NULL;
        //TODO: YOU CANNOT CALL FUNCTIONS WITH MULTIPLE ARGUMENTS RIGHT NOW
        userdata->env->funcall(userdata->env, f_sym, nargs, args);
    } else {
        printf("Woah -- tried to call a null symbol function named %s from Lua.\n", userdata->function_name);
    }
    
    //TODO: change this to 1 -- the 1 emacs_value returned by the funcall
    return 0;
}

static void compile_elisp_table(lua_State *L) {
    //NOTE: THIS FUNCTION ASSUMES THE TOP OF THE STACK IS THE ELISP TABLE
    lua_getglobal(L, "_G");
    lua_insert(L, -2);
    lua_pushstring(L, "elisp");
    lua_insert(L, -2);
    lua_settable(L, -3);
}
static void create_elisp_functions_table(lua_State *L, emacs_env *env) {
    //NOTE: THIS FUNCTION ASSUMES THE TOP OF THE STACK IS THE TABLE WHICH WILL BE THE
    //ELISP GLOBAL TABLE. IF IT IS NOT, THIS FUNCTION WILL NOT WORK.
    //first, get the list of functions from emacs
    //NOTE: this assumes that list-functions and list-symbols are defined already
    emacs_value function_list = slow_arbitrary_funcall(env, "list-functions", 0);
    intmax_t function_list_length = env->extract_integer(env, slow_arbitrary_funcall(env, "length", 1, function_list));

    lua_newtable(L);
    for (intmax_t function_index = 0; function_index < function_list_length; function_index++) {
        char *function_name_buf;
        ptrdiff_t function_name_len;
        emacs_value function_value = nth_in_list(env, function_list, function_index);
        env->copy_string_contents(env, function_value, NULL, &function_name_len);
        function_name_buf = malloc(function_name_len);
        env->copy_string_contents(env, function_value, function_name_buf, &function_name_len);
        printf("Pushing the lua function name string %s\n", function_name_buf);
        lua_pushstring(L, function_name_buf);
        //here's where we actually create the function itself
        struct lua_funcall_userdata *userdata = malloc(sizeof(struct lua_funcall_userdata));
        userdata->env = malloc(sizeof(emacs_env));
        memcpy(userdata->env, env, sizeof(emacs_env));
        userdata->function_name = function_name_buf;
        //the userdata is now created and at the top of the stack. thus, it'll
        //automatically be used as the following C closure's upvalue
        lua_pushlightuserdata (L, (void*)userdata);
        lua_pushcclosure(L, &slow_arbitrary_lua_funcall, 1);
        lua_settable(L, -3);
    }
    //add the function table to the elisp table
    lua_pushstring(L, "functions");
    lua_insert(L, -2);
    lua_settable(L, -3);
}

static void create_elisp_symbols_table(lua_State *L, emacs_env *env) {
    //NOTE: THIS FUNCTION ASSUMES THE TOP OF THE STACK IS THE TABLE WHICH WILL BE THE
    //ELISP GLOBAL TABLE. IF IT IS NOT, THIS FUNCTION WILL NOT WORK.
    //first, get the list of symbols from emacs
    //NOTE: this assumes that list-functions and list-symbols are defined already
    emacs_value symbol_list = slow_arbitrary_funcall(env, "list-symbols", 0);
    intmax_t symbol_list_length = env->extract_integer(env, slow_arbitrary_funcall(env, "length", 1, symbol_list));

    lua_newtable(L);
    for (intmax_t symbol_index = 0; symbol_index < symbol_list_length; symbol_index++) {
        char *symbol_name_buf;
        ptrdiff_t symbol_name_len;
        emacs_value symbol_value = nth_in_list(env, symbol_list, symbol_index);
        env->copy_string_contents(env, symbol_value, NULL, &symbol_name_len);
        symbol_name_buf = malloc(symbol_name_len);
        env->copy_string_contents(env, symbol_value, symbol_name_buf, &symbol_name_len);
        lua_pushstring(L, symbol_name_buf);
        free(symbol_name_buf);
        lua_pushstring(L, "TODO");
        lua_settable(L, -3);
    }
    //add the symbol table to the elisp table
    lua_pushstring(L, "symbols");
    lua_insert(L, -2);
    lua_settable(L, -3);
}

lua_State *lua_emacs_init(emacs_env *env) {
    lua_State *L = lua_open();
    luaL_openlibs(L);
        
    //the elisp table
    lua_newtable(L);
    //the elisp function table
    create_elisp_functions_table(L, env);
    //the elisp symbol table
    create_elisp_symbols_table(L, env);
    //add the elisp table to the global table
    compile_elisp_table(L);
    
    return L;
}
