use libc;
use std::ffi::CString;
use std::os::raw::{c_char, c_void};

extern "C" {
    fn cpp_play_audio(audio_file: *mut c_char) -> isize;
}

#[no_mangle]
pub unsafe extern "C" fn rusty_play_audio(audio_file: *mut c_char) -> isize {
    cpp_play_audio(audio_file);
    println!("Playing audio");
    0
}
