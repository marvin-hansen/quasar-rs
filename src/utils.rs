use rug::Integer;
use rug::rand::RandState;
use std::ffi::CString;
use std::ffi::CStr;
use std::os::raw::c_char;

// utils.go
// https://github.com/bureau14/qdb-api-go/blob/master/utils.go

const letterBytes: &[u8] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ".as_bytes();
const letterIdxBits: u32 = 6;
// 6 bits to represent a letter index
const letterIdxMask: u32 = 1 << letterIdxBits - 1;
// All 1-bits, as many as letterIdxBits
const letterIdxMax: u32 = 63 / letterIdxBits;   // # of letter indices fitting in 63 bits

fn generate_alias<const n: u32>() -> String {
    let nanos = std::time::UNIX_EPOCH.elapsed().unwrap().as_nanos() as u64;
    let seed = Integer::from(nanos);

    let mut rand = RandState::new();
    rand.seed(&seed);

    // generates 63 random bits, enough for letterIdxMax letters!
    let mut cache: u32 = 0;
    let mut remain = n - 1;

    let mut b: Vec<u8> = Vec::with_capacity(n as usize);

    for _ in (0..n).rev() {

        cache = rand.bits(63);

        if remain == 0 {
            cache = rand.bits(63);
            remain = letterIdxMax
        }

        let idx = cache & letterIdxMask;

        if idx < letterBytes.len() as u32 {
            b.push(letterBytes[idx as usize]);
        }

        cache >>= letterIdxBits;
        remain -= 1;
    }

    // There should be no invalid utf-8 characters in letter bytes, but better check
    // https://stackoverflow.com/questions/19076719/how-do-i-convert-a-vector-of-bytes-u8-to-a-string
    let s = match String::from_utf8(b) {
        Ok(v) => v,
        Err(e) => panic!("Invalid UTF-8 sequence: {}", e),
    };

    return s.to_string();
}

pub unsafe fn str_to_cstr(input: &str) -> *mut c_char {
    let c_str = CString::new(input).unwrap().into_raw();
    return c_str;
}

pub unsafe fn cstr_to_str(c_buf: *const i8) -> &'static str {
    let cstr = CStr::from_ptr(c_buf);
    return cstr.to_str().expect("success");
}