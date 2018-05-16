use bindings::*;
use emacs::*;

#[macro_use]
use lua::LuaState;

/*
pub fn create_elisp_functions_table(L: LuaState, env: *mut emacs_env) {
    //NOTE: THIS FUNCTION ASSUMES THE TOP OF THE STACK IS THE TABLE WHICH WILL BE THE
    //ELISP GLOBAL TABLE. IF IT IS NOT, THIS FUNCTION WILL NOT WORK.
    //first, get the list of functions from emacs
    //NOTE: this assumes that list-functions and list-symbols are defined already
    let function_list: emacs_value = emacs::slow_arbitrary_funcall(env, "list-functions", []);
    let function_list_length: i64 = to_int(env, emacs::slow_arbitrary_funcall(env, "length", [function_list]));

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
*/

pub fn lua_emacs_init(env: *mut emacs_env) -> LuaState {
    LuaState L = LuaState::new();
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