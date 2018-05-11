#include "lua-emacs.h"

#include "emacs-module.h"

#include "emacs-interaction.h"

#include <luajit-2.1/lua.h>
#include <luajit-2.1/lauxlib.h>
#include <luajit-2.1/lualib.h>

#include <stdlib.h>

lua_State *lua_emacs_init(emacs_env *env) {
    //first, get the list of functions from emacs
    //NOTE: this assumes that list-functions and list-symbols are defined already
    emacs_value function_list = slow_arbitrary_funcall(env, "list-functions", 0);
    intmax_t function_list_length = env->extract_integer(env, slow_arbitrary_funcall(env, "length", 1, function_list));

    lua_State *L = lua_open();
    luaL_openlibs(L);
    
    //the global table, at the bottom of the stack until the end
    lua_getglobal(L, "_G");
    
        //the elisp table
        lua_newtable(L);
        //the elisp function table
            lua_newtable(L);
            for (intmax_t function_index = 0; function_index < function_list_length; function_index++) {
                char *function_name_buf;
                ptrdiff_t function_name_len;
                emacs_value function_value = nth_in_list(env, function_list, function_index);
                env->copy_string_contents(env, function_value, NULL, &function_name_len);
                function_name_buf = malloc(function_name_len);
                env->copy_string_contents(env, function_value, function_name_buf, &function_name_len);
                lua_pushstring(L, function_name_buf);
                free(function_name_buf);
                lua_pushstring(L, "todo");
                lua_settable(L, -3);
            }
            //add the function table to the elisp table
            lua_pushstring(L, "functions");
            lua_insert(L, -2);
            lua_settable(L, -3);
        //the elisp symbol table
    
    //add the elisp table to the global table
    lua_pushstring(L, "elisp");
    lua_insert(L, -2);
    lua_settable(L, -3);
    
    return L;
}
