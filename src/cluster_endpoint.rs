use std::ffi::CStr;

use crate::qdb_remote_node_t;

// Endpoint : A structure representing a qdb url endpoint
pub struct Endpoint {
    address: String,
    port: u32,
}

impl Endpoint {
    pub fn new(address: &str, port: u32) -> Endpoint {
        Endpoint {
            address: address.to_string(),
            port,
        }
    }

    pub fn URI(&self) -> String {
        format!("{}:{:?}", self.address, self.port)
    }
}

impl qdb_remote_node_t {
    pub fn to_structG(&self) -> Endpoint {
        let c_str = unsafe { CStr::from_ptr(self.address) };
        // c_char is basically a mutable raw pointer with no guarantees and const can't be enforced across the FFI boundary anyways
        // so we need to make a to-owned copy because the raw pointer may change after first access
        let s = c_str.to_str().map(|s| s.to_owned()).expect("Failed to convert C string to Rust string");
        Endpoint {
            address: s,
            port: self.port as u32,
        }
    }
}