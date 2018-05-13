#![crate_type = "dylib"]

extern crate libc;

mod bindings;

pub static plugin_is_GPL_compatible: ::libc::c_int = 0;

    
fn emacs_module_init(ert: *mut bindings::emacs_runtime) {
    unsafe {
        let env: *mut bindings::emacs_env = (*ert).get_environment.expect("Could not secure environment!")(ert);
        let l: *mut bindings::lua_State = bindings::lua_emacs_init(env);
    }
}

