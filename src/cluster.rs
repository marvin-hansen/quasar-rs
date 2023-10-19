use crate::{qdb_cluster_endpoints, qdb_handle_t, qdb_purge_all, qdb_purge_cache, qdb_size_t, qdb_trim_all, qdb_wait_for_stabilization};
use crate::cluster_endpoint::Endpoint;
use crate::error::{ErrorType, makeErrorNone};

// Cluster : An object permitting calls to a cluster
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
    /// It will automatically clean up old versions as entries are accessed.
    ///    This call is not atomic: if the command cannot be dispatched on the whole cluster,
    /// it will be dispatched on as many nodes as possible and the function will return with a qdb_e_ok code.
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
    ///    Takes a timeout value, in milliseconds.
    pub fn WaitForStabilization(&self, timeout_ms: i32) -> Option<ErrorType> {
        unsafe {
            let err = qdb_wait_for_stabilization(self.handle, timeout_ms);
            makeErrorNone(err)
        }
    }

    /// Endpoints : Retrieve all endpoints accessible to this handle.
    pub fn Endpoints(&self) -> Option<Vec<Endpoint>> {
        unsafe {
            let endpoints = std::ptr::null_mut();
            let mut endpoints_count: qdb_size_t = 0;

            let err = qdb_cluster_endpoints(self.handle,
                                            endpoints,
                                            &mut endpoints_count);
            if err == 0 {
                // convert pointer into vector
            }

            return None;
        }
    }
}



