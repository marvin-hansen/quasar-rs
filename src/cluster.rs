use std::ptr;

use crate::{qdb_cluster_endpoints, qdb_handle_t, qdb_purge_all, qdb_purge_cache, qdb_size_t, qdb_trim_all, qdb_wait_for_stabilization, utils_ptr};
use crate::cluster_endpoint::Endpoint;
use crate::error::{Errorable, ErrorType, makeErrorNone};

// Cluster : An object permitting calls to a cluster
#[derive(Debug)]
pub struct Cluster {
    handle: qdb_handle_t,
}

impl Cluster {

    /// Creates a new cluster object permitting calls to a cluster
    pub fn new(handle: qdb_handle_t) -> Cluster {
        Cluster { handle }
    }

    /// PurgeAll : Removes irremediably all data from all the nodes of the cluster.
    /// This function is useful when quasardb is used as a cache and is not the golden source.
    ///
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
    /// This function is disabled on a transient cluster.
    /// Prefer purge_all in this case.
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
    ///
    /// Takes a timeout value, in milliseconds.
    pub fn WaitForStabilization(&self, timeout_ms: i32) -> Option<ErrorType> {
        unsafe {
            let err = qdb_wait_for_stabilization(self.handle, timeout_ms);
            makeErrorNone(err)
        }
    }

    /// Endpoints : Retrieve all endpoints accessible to this handle.
    ///
    /// Returns Result type of either Vec<Endpoint> or an ErrorType.
    pub fn Endpoints(&self) -> Result<Vec<Endpoint>, ErrorType> {
        unsafe {
            // create a new mutable raw pointer to pass to the underlying FFI function
            let endpoints_raw_pointer = ptr::null_mut();

            // create a new mutable counter to pass to the underlying FFI function
            let mut endpoints_count: qdb_size_t = 0;

            let err = qdb_cluster_endpoints(
                self.handle,
                endpoints_raw_pointer,
                &mut endpoints_count,
            );

            // no error
            if err == 0 {
                let endpoints = utils_ptr::raw_pointer_to_vector(
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
                        // drop the raw pointer https://doc.rust-lang.org/stable/std/ptr/fn.drop_in_place.html
                        ptr::drop_in_place(endpoints_raw_pointer);

                        // Convert RawPointer Error to ErrorType. This will lead to a the following ErrorType
                        // ErrSystemLocal / qdb_error_t_qdb_e_system_local =  qdb_error_t = -486539263
                        // System error on local system (client-side).\n! Please check `errno` or `GetLastError()` for actual error.

                        // println!("Error: {}", err.error_message());
                        Err(ErrorType::from_qdb_error_origin_t(err.error_code()))
                    }
                };
            }

            // drop the raw pointer https://doc.rust-lang.org/stable/std/ptr/fn.drop_in_place.html
            ptr::drop_in_place(endpoints_raw_pointer);

            // extract error code from response and return
            return Err(ErrorType::from_qdb_error_origin_t(err));
        }
    }
}
