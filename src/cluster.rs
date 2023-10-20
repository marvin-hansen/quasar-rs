use std::ffi;
use std::ptr;

use crate::{qdb_cluster_endpoints, qdb_handle_t, qdb_purge_all, qdb_purge_cache, qdb_remote_node_t, qdb_size_t, qdb_trim_all, qdb_wait_for_stabilization};
use crate::cluster_endpoint::Endpoint;
use crate::error::{Errorable, ErrorType, makeErrorNone, RawPointerError};

// Cluster : An object permitting calls to a cluster
#[derive(Debug)]
pub struct Cluster {
    handle: qdb_handle_t,
}

impl Cluster {
    pub fn new(handle_tpe: qdb_handle_t) -> Cluster {
        Cluster { handle: handle_tpe }
    }

    /// PurgeAll : Removes irremediably all data from all the nodes of the cluster.
    /// This function is useful when quasardb is used as a cache and is not the golden source.
    /// This call is not atomic: if the command cannot be dispatched on the whole cluster,
    /// it will be dispatched on as many nodes as possible and the function will return with a qdb_e_ok code.
    /// By default cluster does not allow this operation and the function returns a qdb_e_operation_disabled error.
    pub fn PurgeAll(&self) -> Option<ErrorType> {
        unsafe {
            let err = qdb_purge_all(self.handle, 60 * 1000);
            makeErrorNone(err)
        }
    }


    /// PurgeCache : Removes all cached data from all the nodes of the cluster.
    ///    This function is disabled on a transient cluster.
    ///    Prefer purge_all in this case.
    ///
    /// This call is not atomic: if the command cannot be dispatched on the whole cluster,
    /// it will be dispatched on as many nodes as possible and the function will return with a qdb_e_ok code.
    pub fn PurgeCache(&self) -> Option<ErrorType> {
        unsafe {
            let err = qdb_purge_cache(self.handle, 60 * 1000);
            makeErrorNone(err)
        }
    }

    /// TrimAll : Trims all data on all the nodes of the cluster.
    ///    Quasardb uses Multi-Version Concurrency Control (MVCC) as a foundation of its transaction engine.
    ///    It will automatically clean up old versions as entries are accessed.
    ///    This call is not atomic: if the command cannot be dispatched on the whole cluster,
    ///    it will be dispatched on as many nodes as possible and the function will return with a qdb_e_ok code.
    ///    Entries that are not accessed may not be cleaned up, resulting in increasing disk usage.
    ///
    ///    This function will request each nodes to trim all entries, release unused memory and compact files on disk.
    ///    Because this operation is I/O and CPU intensive it is not recommended to run it when the cluster is heavily used.
    pub fn TrimAll(&self) -> Option<ErrorType> {
        unsafe {
            let err = qdb_trim_all(self.handle, 60 * 60 * 1000);
            makeErrorNone(err)
        }
    }

    /// WaitForStabilization : Wait for all nodes of the cluster to be stabilized.
    /// Takes a timeout value, in milliseconds.
    pub fn WaitForStabilization(&self, timeout_ms: i32) -> Option<ErrorType> {
        unsafe {
            let err = qdb_wait_for_stabilization(self.handle, timeout_ms);
            makeErrorNone(err)
        }
    }

    /// Endpoints : Retrieve all endpoints accessible to this handle.
    pub fn Endpoints(&self) -> Result<Vec<Endpoint>, ErrorType> {
        unsafe {
            // create a new mutable raw pointer
            let endpoints_raw_pointer = ptr::null_mut();
            let mut endpoints_count: qdb_size_t = 0;

            let err = qdb_cluster_endpoints(
                self.handle,
                endpoints_raw_pointer,
                &mut endpoints_count,
            );

            if err == 0 {
                let endpoints = raw_pointer_to_vector(
                    endpoints_raw_pointer,
                    endpoints_count,
                );

                return match endpoints {
                    Ok(endpoints) => {
                        // drop the raw pointer https://doc.rust-lang.org/stable/std/ptr/fn.drop_in_place.html
                        ptr::drop_in_place(endpoints_raw_pointer);

                        Ok(endpoints)
                    }
                    Err(err) => {
                        // Convert RawPointer Error to ErrorType. This will lead to a the following ErrorType
                        // ErrSystemLocal / qdb_error_t_qdb_e_system_local =  qdb_error_t = -486539263
                        // System error on local system (client-side).\n! Please check `errno` or `GetLastError()` for actual error.

                        // println!("Error: {}", err.error_message());
                        Err(ErrorType::from_qdb_error_origin_t(err.error_code()))
                    }
                };
            }

            // extract error code from response and return
            return Err(ErrorType::from_qdb_error_origin_t(err));
        }
    }
}

/// Converts  an FFI raw pointer to Vec<Endpoint>
/// Returns Result type of either Vec<Endpoint> or an RawPointerError.
/// RawPointerError maps to ErrorType: ErrSystemLocal / qdb_error_t = -486539263
fn raw_pointer_to_vector(endpoints_ref: *mut *mut qdb_remote_node_t, endpoints_count: usize) -> Result<Vec<Endpoint>, RawPointerError> {

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

/// Converts an FFI raw pointer into an Endpoint struct
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

    let c_str = unsafe { ffi::CStr::from_ptr(endpoint_ref.address) };

    // ffi::c_char is a mutable raw pointer with zero guarantees and const can't be enforced across the FFI boundary anyways
    // so we need to make a to-owned copy because the raw pointer may change after first access
    let result_string = c_str.to_str().map(|s| s.to_owned());

    // error check the result string since there is no guarantee that the string is correct or proper UTF-8
    let address = match result_string {
        Ok(address) => address,
        Err(e) => {
            return Err(RawPointerError("[raw_pointer_to_endpoint]: Failed to convert C string to Rust string: ".to_string() + &*e.to_string()));
        }
    };

    // u32 and all primitive types in Rust are guaranteed to clone by value
    // hence we don't need to call to_owned() or clone() on port.
    // Also, because of the previous conversion to a Rust reference, we can safely access the field without unsafe block
    let port = endpoint_ref.port as u32;

    Ok(Endpoint::new(address, port))
}
