use engine_logs::{vault_error, vault_log};
use libc;
use std::ffi::CString;
use std::os::raw::{c_char, c_void};

mod engine_logs;

#[link(name = "cppvault")]
extern "C" {
    fn hyper_log(str: *mut c_char);
    fn play_audio(file: *mut c_char);
}

#[no_mangle]
pub unsafe extern "C" fn rusty_play_audio(audio_file: *mut c_char) -> isize {
    // let str = CString::new("Rusty Vault").expect("!");
    vault_log(audio_file);
    play_audio(audio_file);
    0
}
