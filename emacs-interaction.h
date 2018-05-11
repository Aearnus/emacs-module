#ifndef EMACS_INTERACTION_H
#define EMACS_INTERACTION_H

#include "emacs-module.h"

emacs_value slow_arbitrary_funcall(emacs_env *env, const char* function_name, ptrdiff_t nargs, ...);
emacs_value nth_in_list(emacs_env *env, emacs_value arg, intmax_t index);

#endif