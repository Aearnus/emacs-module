#include "emacs-module.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define FUCK_GPL int plugin_is_GPL_compatible
FUCK_GPL;

static emacs_value Flua(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data) {
    lua_State *L = lua_open();
    luaL_openlibs(L);
    
    char* luabuffer;
    intmax_t luabuffer_size;
    env->copy_string_contents(env, args[0], luabuffer, &luabuffer_size);
    
    luaL_loadbuffer(L, luabuffer, luabuffer_size, "line");
    lua_pcall(L, 0, 1, 0);
    
    size_t luaout_size;
    const char *luaout = lua_tolstring(L, -1, &luaout_size)
    emacs_value out = env->make_string(env, luaout, luaout_size);
}


int emacs_module_init(struct emacs_runtime *ert) {
    emacs_env *env = ert->get_environment(ert);
    
    emacs_value emacsfunc = env->make_function(
        env, 
        1,1, 
        Flua,
        "Run a lua function and return the result as a string.",
        NULL
    );
    emacs_value emacsfuncsymbol = env->intern(env, "lua");
    
    emacs_value defalias = env->intern(env, "defalias");
    emacs_value defaliasargs[] = {emacsfuncsymbol, emacsfunc};
    env->funcall(env, defalias, 2, defaliasargs);
    return 0;
}