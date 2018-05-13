use bindings::*;
use std::os::raw::*;
use std::ffi::CString;

pub struct LuaState {
    raw_ptr: *mut bindings::lua_State,
}

pub impl LuaState {
    fn lloadstring(&self, s: String) {
        bindings::luaL_loadstring(self.raw_ptr, CString::new(s).expect("Could not slurp Lua.").as_ptr())
    }
    
    fn pcall(&self, nargs: c_int, nresults: c_int, errorfunc: c_int) -> std::os::raw::c_int{
        bindings::lua_pcall(self.raw_ptr, nargs, nresults, errorfunc)
    }
}