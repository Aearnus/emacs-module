use bindings::*;
use std::os::raw::*;
use std::ffi::CString;

pub struct LuaState {
    raw_ptr: *mut lua_State,
}

impl LuaState {
    pub fn new() -> LuaState {
        let L: *mut lua_State = lua_open();
        luaL_openlibs(L);
        LuaState { raw_ptr: L }
    }
    
    pub fn lloadstring(&self, s: String) {
        //TODO: return type?
        luaL_loadstring(self.raw_ptr, CString::new(s).expect("Could not slurp Lua.").as_ptr());
    }
    
    pub fn pcall(&self, nargs: c_int, nresults: c_int, errorfunc: c_int) -> c_int{
        lua_pcall(self.raw_ptr, nargs, nresults, errorfunc)
    }
    
    pub fn tostring(&self, index: c_int) -> String {
        let mut out_size: usize = 0;
        let out_size_ptr = &mut out_size as *mut usize;
        CString::from_raw(lua_tolstring(self.raw_ptr, index, out_size_ptr)).into_string().expect("Couldn't cast string from Lua C string")
    }
}