use bindings::*;
use std::os::raw::*;
use std::ffi::CString;
use std::ffi::CStr;

pub struct LuaState {
    pub raw_ptr: *mut lua_State,
}

impl LuaState {
    pub fn new() -> LuaState {
        unsafe {
            let L: *mut lua_State = luaL_newstate();
            luaL_openlibs(L);
            LuaState { raw_ptr: L }
        }
    }
    
    pub fn lloadstring(&self, s: String) -> c_int{
        unsafe {
            luaL_loadstring(self.raw_ptr, CString::new(s).expect("Could not slurp Lua.").as_ptr())
        }
    }
    
    pub fn pcall(&self, nargs: c_int, nresults: c_int, errorfunc: c_int) -> c_int{
        unsafe {
            lua_pcall(self.raw_ptr, nargs, nresults, errorfunc)
        }
    }
    
    pub fn tostring(&self, index: c_int) -> String {
        let mut out_size: usize = 0;
        let out_size_ptr = &mut out_size as *mut usize;
        unsafe {
            let tolstring: *const c_char = lua_tolstring(self.raw_ptr, index, out_size_ptr);
            CStr::from_ptr(tolstring)
                .to_string_lossy()
                .into_owned()
        }
    }
}