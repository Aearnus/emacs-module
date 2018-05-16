use bindings::*;

use std::ffi::CString;


pub fn to_string(env: *mut emacs_env, value: emacs_value) -> String {
    let mut buf_len: isize = 0;
    let buf_len_ptr = &mut buf_len as *mut isize;
    unsafe {
        (*env)
            .copy_string_contents
            .expect("Invalid ptr to copy_string_contents.")(env, value, 0 as *mut i8, buf_len_ptr);
    }
    let buf: Vec<u8> = Vec::with_capacity(buf_len as usize);
    unsafe {
        (*env)
            .copy_string_contents
            .expect("Invalid ptr to copy_string_contents.")(env, value, buf.as_ptr() as *mut i8, buf_len_ptr);
    }
    String::from_utf8(buf)
        .expect("Invalid UTF-8 in emacs string!")
}

pub fn to_int(env: *mut emacs_env, value: emacs_value) -> i64 {
    unsafe {
        (*env)
            .extract_integer
            .expect("Invalid ptr to extract_integer.")(env, value)
    }
}

pub fn safe_intern(env: *mut emacs_env, name: String) -> emacs_value {
    assert!(env != 0 as *mut emacs_env_26, "Emacs env is null ptr.");
    unsafe {
        (*env).intern
            .expect("Invalid ptr to intern.")(env, CString::new(name).unwrap().as_ptr())
    }
}

pub fn slow_arbitrary_funcall(env: *mut emacs_env, function_name: String, args: &mut [emacs_value]) -> emacs_value {
    let f_sym = safe_intern(env, function_name);
    unsafe {
        (*env)
            .funcall
            .expect("Invalid ptr to fun call.")(env, f_sym, args.len() as isize, args.as_mut_ptr())
    }
} 

pub fn nth_in_list(env: *mut emacs_env, arg: emacs_value, index: intmax_t) -> emacs_value {
    unsafe {
        slow_arbitrary_funcall(env, "nth".to_string(), &mut [(*env).make_integer.unwrap()(env, index)])
    }
}
