#![crate_type = "dylib"]
extern crate libc;
mod bindings;
mod emacs_interaction;

use emacs_interaction::*;

use std::ffi::CString;


pub static plugin_is_GPL_compatible: ::std::os::raw::c_int = 0;

static mut L: *mut bindings::lua_State = 0 as *mut bindings::lua_State;

#[no_mangle]
pub unsafe extern "C" fn Flua(env: *mut bindings::emacs_env, nargs: libc::ptrdiff_t, args: *mut bindings::emacs_value, data: *mut std::os::raw::c_void) -> bindings::emacs_value {
    assert!(nargs == 1, "Wrong number of arguments passed to `lua`");
    let lua_string: String = emacs_value_to_string(env, *args);
    
    bindings::luaL_loadstring(L, CString::new(lua_string).unwrap().as_ptr());
    bindings::lua_pcall(L, 0, 1, 0);
    
    let mut out_size: usize = 0;
    let out_size_ptr = &mut out_size as *mut usize;
    let out_string: *const std::os::raw::c_char = bindings::lua_tolstring(L, -1, out_size_ptr);
    
    (*env).make_string.unwrap()(env, out_string, out_size as isize)
}

#[no_mangle]
pub extern "C" fn emacs_module_init(ert: *mut bindings::emacs_runtime) {
    unsafe {
        let env: *mut bindings::emacs_env = (*ert).get_environment.expect("Could not secure environment!")(ert);
        L = bindings::lua_emacs_init(env);
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

