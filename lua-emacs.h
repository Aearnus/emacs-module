#ifndef LUA_EMACS_H
#define LUA_EMACS_H

#include "emacs-module.h"

#include <luajit-2.1/lua.h>

lua_State *lua_emacs_init(emacs_env *env);

#endif