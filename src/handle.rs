use std::{ffi, ptr};
use std::os::raw;
use std::str::Utf8Error;

use crate::{handler_credentials, qdb_build, qdb_close, qdb_connect, qdb_get_tagged, qdb_get_tags, qdb_handle_t, qdb_open, qdb_option_get_client_max_in_buf_size, qdb_option_get_client_max_parallelism, qdb_option_get_cluster_max_in_buf_size, qdb_option_set_client_max_in_buf_size, qdb_option_set_client_max_parallelism, qdb_option_set_cluster_public_key, qdb_option_set_compression, qdb_option_set_encryption, qdb_option_set_max_cardinality, qdb_option_set_timeout, qdb_option_set_user_credentials, qdb_prefix_count, qdb_prefix_get, qdb_release, qdb_size_t, qdb_version, utils_ptr};
use crate::error::{Errorable, ErrorType, makeErrorNone};
use crate::handle_const::{Compression, Encryption, Protocol, PROTOCOL_DEFAULT};
use crate::handler_credentials::{ClusterKey, JSONCredentialsConfig};
use crate::utils;

/// HandleType : An opaque handle to internal API-allocated structures needed for maintaining connection to a cluster.
pub struct HandleType {
    handle: qdb_handle_t,
}

// Handles constructors

/// NewHandle : Create a new handle, return error if needed
///    The handle is already opened (not connected) with tcp protocol
pub fn new_handle() -> Result<HandleType, ErrorType> {
    unsafe {
        // Don't know if this is correct and safe...
        let mut handle: qdb_handle_t = ptr::null_mut();

        let err = qdb_open(&mut handle, PROTOCOL_DEFAULT);

        return match makeErrorNone(err) {
            None => Ok(HandleType { handle }),
            Some(e) => Err(e)
        };
    }
}

/// SetupHandle : Setup a handle, return error if needed
///    The handle is already opened with tcp protocol
///    The handle is already connected with the clusterURI string
pub fn setup_handle(cluster_uri: &str, timeout: i32) -> Result<HandleType, ErrorType> {
    unsafe {
        // create new handle
        let handle = match new_handle() {
            Ok(h) => h,
            Err(e) => return Err(e)
        };

        // set timeout for the handle
        let err = handle.set_timeout(timeout);
        match err {
            Some(e) => return Err(e),
            None => {}
        }

        // connect handle to the cluster
        return match handle.connect(cluster_uri) {
            None => Ok(handle),
            Some(e) => Err(e)
        };
    }
}

/// MustSetupHandle : Setup a handle, panic on error
///    The handle is already opened with tcp protocol
///    The handle is already connected with the clusterURI string
///
///    Panic on error
pub fn must_setup_handle(cluster_uri: &str, timeout: i32) -> Result<HandleType, ErrorType> {
    return match setup_handle(cluster_uri, timeout){
        Ok(h) => Ok(h),
        Err(e) => panic!("Unable to setup handle: {:?}", e)
    }
}

/// SetupSecuredHandle : Setup a secured handle, return error if needed
///    The handle is already opened with tcp protocol
///    The handle is already secured with the cluster public key and the user credential files provided
///    (Note: the filenames are needed, not the content of the files)
///    The handle is already connected with the clusterURI string
pub fn setup_secured_handle(
    cluster_uri: &str,
    cluster_key_path: &str,
    user_credential_path: &str,
    timeout: i32,
)
    -> Result<HandleType, ErrorType>
{
    unsafe {
        let cluster_key = match handler_credentials::cluster_key_from_file(cluster_key_path) {
            Some(k) => k,
            None => return Err(ErrorType::ErrClusterKeyNotFound)
        };

        let user_credentials = match handler_credentials::user_credentials_from_file(user_credential_path) {
            Some(u) => u,
            None => return Err(ErrorType::ErrUserCredentialsNotFound)
        };

        // create new handle
        let handle = match new_handle() {
            Ok(h) => h,
            Err(e) => return Err(e)
        };

        // set cluster pub key
        match handle.set_cluster_public_key(cluster_key) {
            None => {}
            Some(e) => return Err(e)
        }

        // set user credentials
        match handle.add_user_credentials(user_credentials) {
            None => {}
            Some(e) => return Err(e)
        }

        // set timeout for the handle
        let err = handle.set_timeout(timeout);
        match err {
            Some(e) => return Err(e),
            None => {}
        }

        // connect handle to the cluster
        return match handle.connect(cluster_uri) {
            None => Ok(handle),
            Some(e) => Err(e)
        };
    }
}

    /// MustSetupSecuredHandle : Setup a secured handle, panic on error
    ///	The handle is already opened with tcp protocol
    ///	The handle is already secured with the cluster public key and the user credential files provided
    ///	(Note: the filenames are needed, not the content of the files)
    ///	The handle is already connected with the clusterURI string
    ///
    ///    Panic on error
pub fn must_setup_secured_handle(
    cluster_uri: &str,
    cluster_key_path: &str,
    user_credential_path: &str,
    timeout: i32,
)
    -> Result<HandleType, ErrorType>
{
    return match setup_secured_handle(cluster_uri, cluster_key_path, user_credential_path, timeout){
        Ok(h) => Ok(h),
        Err(e) => panic!("Unable to setup handle: {:?}", e)
    }
}

impl HandleType {
    /// Connect : connect a previously opened handle
    ///    Binds the client instance to a quasardb cluster and connect to at least one node within.
    ///    Quasardb URI are in the form qdb://<address>:<port> where <address> is either an IPv4 or IPv6 (surrounded with square brackets), or a domain name. It is recommended to specify multiple addresses should the designated node be unavailable.
    ///
    ///    URI examples:
    ///    	qdb://myserver.org:2836 - Connects to myserver.org on the port 2836
    ///    	qdb://127.0.0.1:2836 - Connects to the local IPv4 loopback on the port 2836
    ///    	qdb://myserver1.org:2836,myserver2.org:2836 - Connects to myserver1.org or myserver2.org on the port 2836
    //		qdb://[::1]:2836 - Connects to the local IPv6 loopback on the port 2836
    pub fn connect(&self, cluster_uri: &str) -> Option<ErrorType> {
        unsafe {
            let uri = utils::str_to_cstr(cluster_uri);
            let err = qdb_connect(self.handle, uri);
            makeErrorNone(err)
        }
    }

    /// Open : Creates a handle.
    ///    No connection will be established.
    ///    Not needed if you created your handle with NewHandle.
    pub fn open(&mut self, protocol: Protocol) -> Option<ErrorType> {
        unsafe {
            let err = qdb_open(&mut self.handle, protocol);
            makeErrorNone(err)
        }
    }

    /// Close : Closes the handle previously opened.
    ///    This results in terminating all connections and releasing all internal buffers,
    ///    including buffers which may have been allocated as or a result of batch operations or get operations.
    pub fn close(&self) -> Option<ErrorType> {
        unsafe {
            let err = qdb_close(self.handle);
            makeErrorNone(err)
        }
    }
}

impl HandleType {
    /// Release : Releases an API-allocated buffer.
    ///    Failure to properly call this function may result in excessive memory usage.
    ///    Most operations that return a content (e.g. batch operations, qdb_blob_get, qdb_blob_get_and_update, qdb_blob_compare_and_swap...)
    ///    will allocate a buffer for the content and will not release the allocated buffer until you either call this function or close the handle.
    ///
    ///    The function will be able to release any kind of buffer allocated
    /// by a quasardb API call, whether it’s a single buffer, an array or an array of buffers.
    pub fn release(&self, buffer: *const raw::c_void) -> () {
        unsafe {
            qdb_release(self.handle, buffer);
        }
    }

    /// APIVersion : Returns a string describing the API version.
    pub fn api_version() -> Result<String, Utf8Error> {
        // https://stackoverflow.com/questions/24145823/how-do-i-convert-a-c-string-into-a-rust-string-and-back-via-ffi
        let char_ptr = unsafe { qdb_version() };
        let c_str = unsafe { ffi::CStr::from_ptr(char_ptr) };
        c_str.to_str().map(|s| s.to_owned())
    }

    /// APIBuild : Returns a string describing the exact API build.
    pub fn api_build() -> Result<String, Utf8Error> {
        let char_ptr = unsafe { qdb_build() };
        let c_str = unsafe { ffi::CStr::from_ptr(char_ptr) };
        c_str.to_str().map(|s| s.to_owned())
    }


    /// SetTimeout : Sets the timeout of all network operations.
    ///    The lower the timeout, the higher the risk of having timeout errors.
    ///    Keep in mind that the server-side timeout might be shorter.
    pub fn set_timeout(&self, timeout_ms: i32) -> Option<ErrorType> {
        unsafe {
            let err = qdb_option_set_timeout(self.handle, timeout_ms);
            makeErrorNone(err)
        }
    }

    /// SetEncryption : Creates a handle.
    ///        No connection will be established.
    ///        Not needed if you created your handle with NewHandle.
    pub fn set_encryption(&mut self, encryption: Encryption) -> Option<ErrorType> {
        unsafe {
            let err = qdb_option_set_encryption(self.handle, encryption);
            makeErrorNone(err)
        }
    }

    /// AddUserCredentials : add a username and key from a user name and secret.
    pub fn add_user_credentials(&self, credentials: JSONCredentialsConfig) -> Option<ErrorType> {
        unsafe {
            let user_name = utils::str_to_cstr(credentials.username());
            let private_key = utils::str_to_cstr(credentials.secret());
            let err = qdb_option_set_user_credentials(self.handle, user_name, private_key);
            makeErrorNone(err)
        }
    }

    /// AddClusterPublicKey : add the cluster public key from a cluster config file.
    pub fn set_cluster_public_key(&self, cluster_public_key: ClusterKey) -> Option<ErrorType> {
        unsafe {
            let public_key = utils::str_to_cstr(cluster_public_key.cluster_key());
            let err = qdb_option_set_cluster_public_key(self.handle, public_key);
            makeErrorNone(err)
        }
    }

    /// SetMaxCardinality : Sets the maximum allowed cardinality of a quasardb query.
    ///    The default value is 10,007. The minimum allowed values is 100.
    pub fn set_max_cardinality(&self, max_cardinality: u64) -> Option<ErrorType> {
        unsafe {
            let err = qdb_option_set_max_cardinality(self.handle, max_cardinality);
            makeErrorNone(err)
        }
    }

    /// SetCompression : Set the compression level for all future messages emitted by the specified handle.
    ///
    /// Compression values:
    ///    CompNone : No compression.
    ///    CompFast : Maximum compression speed, potentially minimum compression ratio. This is currently the default.
    ///    CompBest : Maximum compression ratio, potentially minimum compression speed. This is currently not implemented.
    ///
    /// Regardless of this parameter, the API will be able to read whatever compression the server uses.
    pub fn set_compression(&self, compression: Compression) -> Option<ErrorType> {
        unsafe {
            let err = qdb_option_set_compression(self.handle, compression);
            makeErrorNone(err)
        }
    }

    /// SetClientMaxInBufSize : Set the Sets the maximum incoming buffer size for all network operations of the client.
    ///  Only modify this setting if you expect to receive very large answers from the server.
    pub fn set_client_max_int_buffer_size(&self, size: usize) -> Option<ErrorType> {
        unsafe {
            let err = qdb_option_set_client_max_in_buf_size(self.handle, size);
            makeErrorNone(err)
        }
    }

    /// GetClientMaxInBufSize : Gets the maximum incoming buffer size for all network operations of the client.
    pub fn get_client_max_int_buffer_size(&self) -> Result<usize, ErrorType> {
        unsafe {
            let mut size = 0;
            let err = qdb_option_get_client_max_in_buf_size(self.handle, &mut size);

            return match makeErrorNone(err) {
                None => Ok(size),
                Some(err) => Err(err),
            };
        }
    }

    /// GetClusterMaxInBufSize : Gets the maximum incoming buffer size for all network operations of the client.
    pub fn get_cluster_max_int_buffer_size(&self) -> Result<usize, ErrorType> {
        unsafe {
            let mut size = 0;
            let err = qdb_option_get_cluster_max_in_buf_size(self.handle, &mut size);

            return match makeErrorNone(err) {
                None => Ok(size),
                Some(err) => Err(err),
            };
        }
    }

    /// GetClientMaxParallelism : Gets the maximum parallelism option of the client.
    pub fn get_client_max_parallelism(&self) -> Result<usize, ErrorType> {
        unsafe {
            let mut size = 0;
            let err = qdb_option_get_client_max_parallelism(self.handle, &mut size);

            return match makeErrorNone(err) {
                None => Ok(size),
                Some(err) => Err(err),
            };
        }
    }

    /// SetClientMaxParallelism : Gets the maximum parallelism option of the client.
    pub fn set_client_max_parallelism(&self, threat_count: usize) -> Option<ErrorType> {
        unsafe {
            let err = qdb_option_set_client_max_parallelism(self.handle, threat_count);
            makeErrorNone(err)
        }
    }

    /// GetTags : Retrieves all the tags of an entry.
    ///    Tagging an entry enables you to search for entries based on their tags. Tags scale across nodes.
    ///    The entry must exist.
    pub fn get_tags(&self, entry_alias: &str) -> Result<Vec<String>, ErrorType> {
        unsafe {
            let mut result: Vec<String> = Vec::new();
            let alias = utils::str_to_cstr(entry_alias);
            let mut tag_count: qdb_size_t = 0;
            let tags_raw_ptr = ptr::null_mut();

            let err = qdb_get_tags(self.handle, alias, tags_raw_ptr, &mut tag_count);

            if err == 0 && tag_count > 0 {
                result = match utils_ptr::raw_pointer_to_string_vector(tags_raw_ptr, tag_count) {
                    Ok(v) => v,
                    Err(e) => return Err(ErrorType::from_qdb_error_origin_t(e.error_code())),
                };
            }

            ptr::drop_in_place(alias);
            ptr::drop_in_place(tags_raw_ptr);

            return match makeErrorNone(err) {
                None => Ok(result),
                Some(err) => Err(err),
            };
        }
    }

    /// GetTagged : Retrieves all entries that have the specified tag.
    ///    Tagging an entry enables you to search for entries based on their tags. Tags scale across nodes.
    ///    The tag must exist.
    ///    The complexity of this function is constant.
    pub fn get_tagged(&self, tag: &str) -> Result<Vec<String>, ErrorType> {
        unsafe {
            let mut result: Vec<String> = Vec::new();
            let alias = utils::str_to_cstr(tag);
            let mut count: qdb_size_t = 0;
            let raw_ptr = ptr::null_mut();

            let err = qdb_get_tagged(self.handle, alias, raw_ptr, &mut count);

            if err == 0 && count > 0 {
                result = match utils_ptr::raw_pointer_to_string_vector(raw_ptr, count) {
                    Ok(v) => v,
                    Err(e) => return Err(ErrorType::from_qdb_error_origin_t(e.error_code())),
                };
            }

            ptr::drop_in_place(alias);
            ptr::drop_in_place(raw_ptr);

            return match makeErrorNone(err) {
                None => Ok(result),
                Some(err) => Err(err),
            };
        }
    }

    /// PrefixGet : Retrieves the list of all entries matching the provided prefix.
    /// A prefix-based search will enable you to find all entries matching a provided prefix.
    /// This function returns the list of aliases.
    /// It’s up to the user to query the content associated with every entry, if needed.
    pub fn prefix_get(&self, prefix: &str, limit: i64) -> Result<Vec<String>, ErrorType> {
        unsafe {
            let prefix = utils::str_to_cstr(prefix);
            let raw_ptr = ptr::null_mut();
            let mut result_count: usize = 0;
            let mut result: Vec<String> = Vec::new();

            let err = qdb_prefix_get(
                self.handle,
                prefix,
                limit,
                raw_ptr,
                &mut result_count,
            );

            if err == 0 && result_count > 0 {
                result = match utils_ptr::raw_pointer_to_string_vector(raw_ptr, result_count) {
                    Ok(v) => v,
                    Err(e) => return Err(ErrorType::from_qdb_error_origin_t(e.error_code())),
                };
            }

            ptr::drop_in_place(prefix);
            ptr::drop_in_place(raw_ptr);

            return match makeErrorNone(err) {
                None => Ok(result),
                Some(err) => Err(err),
            };
        }
    }

    /// PrefixCount : Retrieves the count of all entries matching the provided prefix.
    /// A prefix-based count counts all entries matching a provided prefix.
    pub fn prefix_count(&self, prefix: &str) -> Result<u64, ErrorType> {
        unsafe {
            let prefix = utils::str_to_cstr(prefix);
            let mut result: u64 = 0;

            let err = qdb_prefix_count(self.handle, prefix, &mut result);

            ptr::drop_in_place(prefix);

            return match makeErrorNone(err) {
                None => Ok(result),
                Some(err) => Err(err),
            };
        }
    }
}

// Entries creators

// impl HandleType{
//     // Blob : Create a blob entry object
//     pub fn blob(&self, alias: &str) -> Result<BlobEntry, ErrorType> {}
//
// }