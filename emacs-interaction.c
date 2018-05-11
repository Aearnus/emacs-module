#include "emacs-interaction.h"

#include "emacs-module.h"

#include <stdlib.h>
#include <stdarg.h>

emacs_value slow_arbitrary_funcall(emacs_env *env, const char* function_name, ptrdiff_t nargs, ...) {
    emacs_value f_sym = env->intern(env, function_name);
    emacs_value *f_args = malloc(sizeof(emacs_value) * nargs);
    va_list args;
    va_start(args, nargs);
    for (int i = 0; i < nargs; i++) {
        f_args[i] = va_arg(args, emacs_value);
    }
    va_end(args);
    return env->funcall(env, f_sym, nargs, f_args);
}

emacs_value nth_in_list(emacs_env *env, emacs_value arg, intmax_t index) {
    return slow_arbitrary_funcall(env, "nth", 2, env->make_integer(env, index), arg);
}