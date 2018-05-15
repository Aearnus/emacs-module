#![crate_type = "dylib"]
extern crate libc;
mod bindings;
mod emacs_interaction;
mod lua;

use emacs_interaction::*;
use lua::LuaState;

use std::ffi::CString;
use std::ffi::CStr;


pub static plugin_is_GPL_compatible: std::os::raw::c_int = 0;

static mut L: LuaState = LuaState { raw_ptr: 0 as *mut bindings::lua_State, };

#[no_mangle]
pub unsafe extern "C" fn Flua(env: *mut bindings::emacs_env, nargs: libc::ptrdiff_t, args: *mut bindings::emacs_value, data: *mut std::os::raw::c_void) -> bindings::emacs_value {
    assert!(nargs == 1, "Wrong number of arguments passed to `lua`");
    let lua_string: String = emacs_value_to_string(env, *args);
    
    L.lloadstring(lua_string);
    L.pcall(0, 1, 0);
        
    let out_string = L.tostring(-1);
    let out_c_string = CString::new(out_string).expect("Couldn't allocate out string");
    (*env).make_string.unwrap()(env, out_c_string.as_ptr(), out_c_string.as_bytes_with_nul().len() as isize)
}

#[no_mangle]
pub extern "C" fn emacs_module_init(ert: *mut bindings::emacs_runtime) {
    unsafe {
        let env: *mut bindings::emacs_env = (*ert).get_environment.expect("Could not secure environment!")(ert);
        let L_ptr = bindings::lua_emacs_init(env);
        L = LuaState { raw_ptr: L_ptr };
        //let l: *mut bindings::lua_State = bindings::lua_emacs_init(env);
        
        let func: bindings::emacs_value = (*env).make_function.unwrap()(
            env, 
            1,1, 
            Some(Flua),
            CString::new("Run a lua function and return the result as a string.").unwrap().as_ptr(),
            0 as *mut std::os::raw::c_void
        );
        let func_sym: bindings::emacs_value = safe_intern(env, "lua".to_string());
        // provide the lua function
        bindings::slow_arbitrary_funcall(env, CString::new("defalias").unwrap().as_ptr(), 2, func_sym, func);
        
        // provide the feature `mefa`
        let module_sym: bindings::emacs_value = safe_intern(env, "mefa".to_string());
        bindings::slow_arbitrary_funcall(env, CString::new("provide").unwrap().as_ptr(), 1, module_sym);

    }
}

