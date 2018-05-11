#include "emacs-module.h"

#include "lua-emacs.h"
#include "emacs-interaction.h"

#include <stdlib.h>
#include <stdarg.h>

#include <luajit-2.1/lua.h>
#include <luajit-2.1/lauxlib.h>
#include <luajit-2.1/lualib.h>

#define FUCK_GPL int plugin_is_GPL_compatible
FUCK_GPL;


static emacs_value Flua(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data) {
    lua_State *L = lua_emacs_init(env);
    
    char *luabuffer;
    intmax_t luabuffer_size;
    env->copy_string_contents(env, args[0], NULL, &luabuffer_size);
    luabuffer = malloc(luabuffer_size);
    env->copy_string_contents(env, args[0], luabuffer, &luabuffer_size);
    
    luaL_loadstring(L, luabuffer);
    lua_pcall(L, 0, 1, 0);
    
    size_t luaout_size;
    const char *luaout = lua_tolstring(L, -1, &luaout_size);
    emacs_value out = env->make_string(env, luaout, luaout_size);
    lua_close(L);
    free(luabuffer);
    return out;
}


int emacs_module_init(struct emacs_runtime *ert) {
    emacs_env *env = ert->get_environment(ert);
    // bind the function `lua` into elisp
    emacs_value emacsfunc = env->make_function(
        env, 
        1,1, 
        Flua,
        "Run a lua function and return the result as a string.",
        NULL
    );
    emacs_value func_sym = env->intern(env, "lua");
    
    // provide the lua function
    slow_arbitrary_funcall(env, "defalias", 2, func_sym, emacsfunc);

    // provide the feature `module`
    emacs_value module_sym = env->intern(env, "luajit");
    slow_arbitrary_funcall(env, "provide", 1, module_sym);
    
    return 0;
}