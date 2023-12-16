use libc;
use std::ffi::CString;
use std::os::raw::c_char;

extern "C" {
    fn vault_log(str: *mut *mut c_char);
}

#[no_mangle]
pub unsafe extern "C" fn rusty_call_log(str: *mut *mut c_char) {
    vault_log(str);
}
