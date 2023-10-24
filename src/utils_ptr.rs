use std::{ffi, ptr};
use std::os::raw;

use crate::cluster_endpoint::Endpoint;
use crate::error::RawPointerError;
use crate::qdb_remote_node_t;

/// raw_pointer_to_vector: Converts  an FFI raw pointer to Vec<Endpoint>
///
/// Returns Result type of either Vec<Endpoint> or an RawPointerError.
/// RawPointerError maps to ErrorType: ErrSystemLocal / qdb_error_t = -486539263
pub fn raw_pointer_to_vector(endpoints_ref: *mut *mut qdb_remote_node_t, endpoints_count: usize) -> Result<Vec<Endpoint>, RawPointerError> {

    // Null check raw pointer
    if endpoints_ref.is_null() {
        return Err(RawPointerError("[raw_pointer_to_vector]: C Raw pointer is NULL ".to_string()));
    }

    let mut output: Vec<Endpoint> = vec![];

    unsafe {
        if endpoints_count > 0 {
            // https://stackoverflow.com/questions/27150652/how-can-i-get-an-array-or-a-slice-from-a-raw-pointer
            let slice = std::slice::from_raw_parts(endpoints_ref, endpoints_count);

            let mut i = 0;
            while i < endpoints_count {
                match raw_pointer_to_endpoint(slice[i]) {
                    Ok(endpoint) => {
                        output.push(endpoint)
                    }
                    Err(err) => {
                        // drop the raw pointer to prevent memory leak https://doc.rust-lang.org/stable/std/ptr/fn.drop_in_place.html
                        ptr::drop_in_place(slice[i]);

                        return Err(err);
                    }
                }

                i += 1;
            }

            // drop the raw pointer https://doc.rust-lang.org/stable/std/ptr/fn.drop_in_place.html
            ptr::drop_in_place(slice[i]);
        }
    }

    Ok(output)
}

/// raw_pointer_to_endpoint: Converts an FFI raw pointer into an Endpoint struct
///
/// Returns Result type of either an Endpoint or an RawPointerError.
/// RawPointerError maps to ErrorType: ErrSystemLocal / qdb_error_t = -486539263
fn raw_pointer_to_endpoint(endpoint_ptr: *const qdb_remote_node_t) -> Result<Endpoint, RawPointerError> {

    // Converts an FFI raw pointer into a safe Rust reference: https://stackoverflow.com/questions/37466676/is-it-possible-to-match-against-a-null-pointer-in-rust
    let convert_ptr_to_ref = unsafe { endpoint_ptr.as_ref() };

    let endpoint_ref = match convert_ptr_to_ref {
        None => {
            return Err(RawPointerError("[raw_pointer_to_endpoint]: C Raw pointer is NULL ".to_string()));
        }
        Some(res) => res,
    };

    // u32 and all primitive types in Rust are guaranteed to clone by value hence we don't need to call to_owned() or clone().
    // Also, because of the previous conversion to a Rust reference, we can safely access the field without unsafe block
    let port = endpoint_ref.port as u32;

    // ffi::c_char is a mutable raw pointer with zero guarantees and const can't be enforced across the FFI boundary anyways
    // so we need to make a to-owned copy because the raw pointer may change after first access
    let c_str = unsafe { ffi::CStr::from_ptr(endpoint_ref.address) };

    let result_string = c_str.to_str().map(|s| s.to_owned());

    // error check the result string since there is no guarantee that the string is correct or proper UTF-8
    return match result_string {
        Ok(address) => Ok(Endpoint::new(address, port)),
        Err(e) => Err(RawPointerError("[raw_pointer_to_endpoint]: Failed to convert C string to Rust string: ".to_string() + &*e.to_string())),
    };
}

 fn raw_pointer_to_string(raw_ptr: *const raw::c_char) -> Result<String, RawPointerError> {
    let c_str = unsafe { ffi::CStr::from_ptr(raw_ptr) };

    let conv_string = c_str.to_str().map(|s| s.to_owned());

    return match conv_string {
        Ok(string) => Ok(string),
        Err(e) => Err(RawPointerError("[raw_pointer_to_endpoint]: Failed to convert C string to Rust string: ".to_string() + &*e.to_string())),
    };
}

pub fn raw_pointer_to_string_vector(
    raw_ptr: *mut *mut *const ffi::c_char,
    len: usize)
    -> Result<Vec<String>, RawPointerError>
{
    unsafe {
        let mut result: Vec<String> = Vec::new();

        let slice = std::slice::from_raw_parts(raw_ptr, len);
        for tag in slice {
            let string_tag = raw_pointer_to_string(tag.read())
                .expect("Failed to convert raw pointer to string");
            result.push(string_tag);
        }

        Ok(result)
    }
}
