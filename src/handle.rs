use std::ffi::CStr;
use std::str::Utf8Error;

use crate::{qdb_build, qdb_handle_t, qdb_open, qdb_option_set_cluster_public_key, qdb_option_set_encryption, qdb_option_set_timeout, qdb_option_set_user_credentials, qdb_version};
use crate::error::{ErrorType, makeErrorNone};
use crate::handle_const::{Encryption, Protocol};
use crate::handler_credentials::{ClusterKey, JSONCredentialsConfig};
use crate::utils::str_to_cstr;

// HandleType : An opaque handle to internal API-allocated structures needed for maintaining connection to a cluster.
pub struct HandleType {
    handle: qdb_handle_t,
}

impl HandleType {
    /// APIVersion : Returns a string describing the API version.
    pub fn api_version() -> Result<String, Utf8Error> {
        // https://stackoverflow.com/questions/24145823/how-do-i-convert-a-c-string-into-a-rust-string-and-back-via-ffi
        let char_ptr = unsafe { qdb_version() };
        let c_str = unsafe { CStr::from_ptr(char_ptr) };
        c_str.to_str().map(|s| s.to_owned())
    }

    /// APIBuild : Returns a string describing the exact API build.
    pub fn api_build() -> Result<String, Utf8Error> {
        let char_ptr = unsafe { qdb_build() };
        let c_str = unsafe { CStr::from_ptr(char_ptr) };
        c_str.to_str().map(|s| s.to_owned())
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

    /// SetTimeout : Sets the timeout of all network operations.
    ///    The lower the timeout, the higher the risk of having timeout errors.
    ///    Keep in mind that the server-side timeout might be shorter.
    pub fn timeout(&self, timeout_ms: i32) -> Option<ErrorType> {
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
            let user_name = str_to_cstr(credentials.username());
            let private_key = str_to_cstr(credentials.secret());
            let err = qdb_option_set_user_credentials(self.handle, user_name, private_key);
            makeErrorNone(err)
        }
    }

    /// AddClusterPublicKey : add the cluster public key from a cluster config file.
    pub fn add_cluster_public_key(&self, cluster_public_key: ClusterKey) -> Option<ErrorType> {
        unsafe {
            let public_key = str_to_cstr(cluster_public_key.cluster_key());
            let err = qdb_option_set_cluster_public_key(self.handle, public_key);
            makeErrorNone(err)
        }
    }
}