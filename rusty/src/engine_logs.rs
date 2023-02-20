use std::ffi::CString;
use std::os::raw::{c_char, c_void};

#[link(name = "cppvault")]
extern "C" {
    pub fn vault_log(str: *mut c_char);
    pub fn vault_error(str: *mut c_char);
    pub fn vault_warning(str: *mut c_char);
}
