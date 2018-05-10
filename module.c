#include "emacs-module.h"

#include <stdlib.h>

#include <luajit-2.1/lua.h>
#include <luajit-2.1/lauxlib.h>
#include <luajit-2.1/lualib.h>

#define FUCK_GPL int plugin_is_GPL_compatible
FUCK_GPL;

static emacs_value Flua(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data) {
    lua_State *L = lua_open();
    luaL_openlibs(L);
    
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
    
    emacs_value defalias = env->intern(env, "defalias");
    emacs_value defalias_args[] = {func_sym, emacsfunc};
    env->funcall(env, defalias, 2, defalias_args);
    
    // provide the feature `module`
    emacs_value module_sym = env->intern(env, "module");
    emacs_value provide = env->intern(env, "provide");
    emacs_value provide_args[] = { module_sym };
    env->funcall(env, provide, 1, provide_args);
    
    return 0;
}