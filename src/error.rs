use std::fmt;

use crate::{qdb_error_origin_t, qdb_error_t_qdb_e_access_denied, qdb_error_t_qdb_e_alias_already_exists, qdb_error_t_qdb_e_alias_not_found, qdb_error_t_qdb_e_alias_too_long, qdb_error_t_qdb_e_buffer_too_small, qdb_error_t_qdb_e_clock_skew, qdb_error_t_qdb_e_column_not_found, qdb_error_t_qdb_e_conflict, qdb_error_t_qdb_e_connection_refused, qdb_error_t_qdb_e_connection_reset, qdb_error_t_qdb_e_container_empty, qdb_error_t_qdb_e_container_full, qdb_error_t_qdb_e_data_corruption, qdb_error_t_qdb_e_element_already_exists, qdb_error_t_qdb_e_element_not_found, qdb_error_t_qdb_e_entry_too_large, qdb_error_t_qdb_e_host_not_found, qdb_error_t_qdb_e_incompatible_type, qdb_error_t_qdb_e_internal_local, qdb_error_t_qdb_e_internal_remote, qdb_error_t_qdb_e_interrupted, qdb_error_t_qdb_e_invalid_argument, qdb_error_t_qdb_e_invalid_crypto_key, qdb_error_t_qdb_e_invalid_handle, qdb_error_t_qdb_e_invalid_iterator, qdb_error_t_qdb_e_invalid_protocol, qdb_error_t_qdb_e_invalid_query, qdb_error_t_qdb_e_invalid_regex, qdb_error_t_qdb_e_invalid_reply, qdb_error_t_qdb_e_invalid_version, qdb_error_t_qdb_e_iterator_end, qdb_error_t_qdb_e_login_failed, qdb_error_t_qdb_e_network_error, qdb_error_t_qdb_e_network_inbuf_too_small, qdb_error_t_qdb_e_no_memory_local, qdb_error_t_qdb_e_no_memory_remote, qdb_error_t_qdb_e_no_space_left, qdb_error_t_qdb_e_not_connected, qdb_error_t_qdb_e_not_implemented, qdb_error_t_qdb_e_ok, qdb_error_t_qdb_e_ok_created, qdb_error_t_qdb_e_operation_disabled, qdb_error_t_qdb_e_operation_not_permitted, qdb_error_t_qdb_e_out_of_bounds, qdb_error_t_qdb_e_overflow, qdb_error_t_qdb_e_query_too_complex, qdb_error_t_qdb_e_quota_exceeded, qdb_error_t_qdb_e_reserved_alias, qdb_error_t_qdb_e_resource_locked, qdb_error_t_qdb_e_skipped, qdb_error_t_qdb_e_system_local, qdb_error_t_qdb_e_system_remote, qdb_error_t_qdb_e_tag_already_set, qdb_error_t_qdb_e_tag_not_set, qdb_error_t_qdb_e_timeout, qdb_error_t_qdb_e_transaction_partial_failure, qdb_error_t_qdb_e_try_again, qdb_error_t_qdb_e_underflow, qdb_error_t_qdb_e_uninitialized, qdb_error_t_qdb_e_unknown_user, qdb_error_t_qdb_e_unmatched_content, qdb_error_t_qdb_e_unstable_cluster};

// qdb_error_origin_t is ffi::c_int  which is equivalent to C’s signed int (int) type.
// This type will almost always be i32, but may differ on some esoteric systems.
// https://do = qdb_error_t_rust-lang.org/stable/std/ffi/type.c_int.html

// Success : Success.
// Created : Success. A new entry has been created.
// ErrUninitialized : Uninitialized error.
// ErrAliasNotFound : Entry alias/key was not found.
// ErrAliasAlreadyExists : Entry alias/key already exists.
// ErrOutOfBounds : Index out of bounds.
// ErrSkipped : Skipped operation. Used in batches and transactions.
// ErrIncompatibleType : Entry or column is incompatible with the operation.
// ErrContainerEmpty : Container is empty.
// ErrContainerFull : Container is full.
// ErrElementNotFound : Element was not found.
// ErrElementAlreadyExists : Element already exists.
// ErrOverflow : Arithmetic operation overflows.
// ErrUnderflow : Arithmetic operation underflows.
// ErrTagAlreadySet : Tag is already set.
// ErrTagNotSet : Tag is not set.
// ErrTimeout : Operation timed out.
// ErrConnectionRefused : Connection was refused.
// ErrConnectionReset : Connection was reset.
// ErrUnstableCluster : Cluster is unstable.
// ErrTryAgain : Please retry.
// ErrConflict : There is another ongoing conflicting operation.
// ErrNotConnected : Handle is not connected.
// ErrResourceLocked : Resource is locked.
// ErrSystemRemote : System error on remote node (server-side). Please check errno or GetLastError() for actual error.
// ErrSystemLocal : System error on local system (client-side). Please check errno or GetLastError() for actual error.
// ErrInternalRemote : Internal error on remote node (server-side).
// ErrInternalLocal : Internal error on local system (client-side).
// ErrNoMemoryRemote : No memory on remote node (server-side).
// ErrNoMemoryLocal : No memory on local system (client-side).
// ErrInvalidProtocol : Protocol is invalid.
// ErrHostNotFound : Host was not found.
// ErrBufferTooSmall : Buffer is too small.
// ErrNotImplemented : Operation is not implemented.
// ErrInvalidVersion : Version is invalid.
// ErrInvalidArgument : Argument is invalid.
// ErrInvalidHandle : Handle is invalid.
// ErrReservedAlias : Alias/key is reserved.
// ErrUnmatchedContent : Content did not match.
// ErrInvalidIterator : Iterator is invalid.
// ErrEntryTooLarge : Entry is too large.
// ErrTransactionPartialFailure : Transaction failed partially.
// ErrOperationDisabled : Operation has not been enabled in cluster configuration.
// ErrOperationNotPermitted : Operation is not permitted.
// ErrIteratorEnd :	Iterator reached the end.
// ErrInvalidReply : Cluster sent an invalid reply.
// ErrNoSpaceLeft : No more space on disk.
// ErrQuotaExceeded : Disk space quota has been reached.
// ErrAliasTooLong : Alias is too long.
// ErrClockSkew : Cluster nodes have important clock differences.
// ErrAccessDenied : Access is denied.
// ErrLoginFailed : Login failed.
// ErrColumnNotFound : Column was not found.
// ErrQueryTooComplex : Find is too complex.
// ErrInvalidCryptoKey : Security key is invalid.
// ErrInvalidQuery : Query is invalid.
// ErrInvalidRegex : Regular expression is invalid.
// ErrUnknownUser : Unknown user.
// ErrInterrupted : Operation has been interrupted.
// ErrNetworkInbufTooSmall : Network input buffer is too small to complete the operation.
// ErrNetworkError : Network error.
// ErrDataCorruption : Data corruption has been detected.

pub fn makeErrorNone(err: qdb_error_origin_t) -> Option<ErrorType> {
    if err != qdb_error_t_qdb_e_ok && err != qdb_error_t_qdb_e_ok_created {
        Some(ErrorType::from_qdb_error_origin_t(err))
    } else {
        None
    }
}

impl fmt::Display for ErrorType {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:?}", self)
    }
}

#[repr(i32)]
#[derive(Debug)]
pub enum ErrorType {
    // We need this as a catch-all for (future) error codes that we don't know how to handle.
    ErrUnknownError = -42,
    #[doc = "! Success."]
    Success = qdb_error_t_qdb_e_ok,
    #[doc = "! Success. A new entry has been created."]
    Created = qdb_error_t_qdb_e_ok_created,
    #[doc = "! Uninitialized error."]
    ErrUninitialized = qdb_error_t_qdb_e_uninitialized,
    #[doc = "! Entry alias/key was not found."]
    ErrAliasNotFound = qdb_error_t_qdb_e_alias_not_found,
    #[doc = "! Entry alias/key already exists."]
    ErrAliasAlreadyExists = qdb_error_t_qdb_e_alias_already_exists,
    #[doc = "! Index out of bounds."]
    ErrOutOfBounds = qdb_error_t_qdb_e_out_of_bounds,
    #[doc = "! Skipped operation. Used in batches and transactions."]
    ErrSkipped = qdb_error_t_qdb_e_skipped,
    #[doc = "! Entry or column is incompatible with the operation."]
    ErrIncompatibleType = qdb_error_t_qdb_e_incompatible_type,
    #[doc = "! Container is empty."]
    ErrContainerEmpty = qdb_error_t_qdb_e_container_empty,
    #[doc = "! Container is full."]
    ErrContainerFull = qdb_error_t_qdb_e_container_full,
    #[doc = "! Element was not found."]
    ErrElementNotFound = qdb_error_t_qdb_e_element_not_found,
    #[doc = "! Element already exists."]
    ErrElementAlreadyExists = qdb_error_t_qdb_e_element_already_exists,
    #[doc = "! Arithmetic operation overflows."]
    ErrOverflow = qdb_error_t_qdb_e_overflow,
    #[doc = "! Arithmetic operation underflows."]
    ErrUnderflow = qdb_error_t_qdb_e_underflow,
    #[doc = "! Tag is already set."]
    ErrTagAlreadySet = qdb_error_t_qdb_e_tag_already_set,
    #[doc = "! Tag is not set."]
    ErrTagNotSet = qdb_error_t_qdb_e_tag_not_set,
    #[doc = "! Operation timed out."]
    ErrTimeout = qdb_error_t_qdb_e_timeout,
    #[doc = "! Connection was refused."]
    ErrConnectionRefused = qdb_error_t_qdb_e_connection_refused,
    #[doc = "! Connection was reset."]
    ErrConnectionReset = qdb_error_t_qdb_e_connection_reset,
    #[doc = "! Cluster is unstable."]
    ErrUnstableCluster = qdb_error_t_qdb_e_unstable_cluster,
    #[doc = "! Please retry."]
    ErrTryAgain = qdb_error_t_qdb_e_try_again,
    #[doc = "! There is another ongoing conflicting operation."]
    ErrConflict = qdb_error_t_qdb_e_conflict,
    #[doc = "! Handle is not connected."]
    ErrNotConnected = qdb_error_t_qdb_e_not_connected,
    #[doc = "! Resource is locked."]
    ErrResourceLocked = qdb_error_t_qdb_e_resource_locked,
    #[doc = "! System error on remote node (server-side).\n! Please check `errno` or `GetLastError()` for actual error."]
    ErrSystemRemote = qdb_error_t_qdb_e_system_remote,
    #[doc = "! System error on local system (client-side).\n! Please check `errno` or `GetLastError()` for actual error."]
    ErrSystemLocal = qdb_error_t_qdb_e_system_local,
    #[doc = "! Internal error on remote node (server-side)."]
    ErrInternalRemote = qdb_error_t_qdb_e_internal_remote,
    #[doc = "! Internal error on local system (client-side)."]
    ErrInternalLocal = qdb_error_t_qdb_e_internal_local,
    #[doc = "! No memory on remote node (server-side)."]
    ErrNoMemoryRemote = qdb_error_t_qdb_e_no_memory_remote,
    #[doc = "! No memory on local system (client-side)."]
    ErrNoMemoryLocal = qdb_error_t_qdb_e_no_memory_local,
    #[doc = "! Protocol is invalid."]
    ErrInvalidProtocol = qdb_error_t_qdb_e_invalid_protocol,
    #[doc = "! Host was not found."]
    ErrHostNotFound = qdb_error_t_qdb_e_host_not_found,
    #[doc = "! Buffer is too small."]
    ErrBufferTooSmall = qdb_error_t_qdb_e_buffer_too_small,
    #[doc = "! Operation is not implemented."]
    ErrNotImplemented = qdb_error_t_qdb_e_not_implemented,
    #[doc = "! Version is invalid."]
    ErrInvalidVersion = qdb_error_t_qdb_e_invalid_version,
    #[doc = "! Argument is invalid."]
    ErrInvalidArgument = qdb_error_t_qdb_e_invalid_argument,
    #[doc = "! Handle is invalid."]
    ErrInvalidHandle = qdb_error_t_qdb_e_invalid_handle,
    #[doc = "! Alias/key is reserved."]
    ErrReservedAlias = qdb_error_t_qdb_e_reserved_alias,
    #[doc = "! Content did not match."]
    ErrUnmatchedContent = qdb_error_t_qdb_e_unmatched_content,
    #[doc = "! Iterator is invalid."]
    ErrInvalidIterator = qdb_error_t_qdb_e_invalid_iterator,
    #[doc = "! Entry is too large."]
    ErrEntryTooLarge = qdb_error_t_qdb_e_entry_too_large,
    #[doc = "! Transaction failed partially.\n! \\warning This may provoke failures until the transaction has not been rolled back.\n! \\see Cluster configuration parameter `global/cluster/max_transaction_duration`."]
    ErrTransactionPartialFailure = qdb_error_t_qdb_e_transaction_partial_failure,
    #[doc = "! Operation has not been enabled in cluster configuration."]
    ErrOperationDisabled = qdb_error_t_qdb_e_operation_disabled,
    #[doc = "! Operation is not permitted."]
    ErrOperationNotPermitted = qdb_error_t_qdb_e_operation_not_permitted,
    #[doc = "! Iterator reached the end."]
    ErrIteratorEnd = qdb_error_t_qdb_e_iterator_end,
    #[doc = "! Cluster sent an invalid reply."]
    ErrInvalidReply = qdb_error_t_qdb_e_invalid_reply,
    #[doc = "! No more space on disk."]
    ErrNoSpaceLeft = qdb_error_t_qdb_e_no_space_left,
    #[doc = "! Disk space quota has been reached."]
    ErrQuotaExceeded = qdb_error_t_qdb_e_quota_exceeded,
    #[doc = "! Alias is too long.\n! \\see \\ref qdb_l_max_alias_length"]
    ErrAliasTooLong = qdb_error_t_qdb_e_alias_too_long,
    #[doc = "! Cluster nodes have important clock differences."]
    ErrClockSkew = qdb_error_t_qdb_e_clock_skew,
    #[doc = "! Access is denied."]
    ErrAccessDenied = qdb_error_t_qdb_e_access_denied,
    #[doc = "! Login failed."]
    ErrLoginFailed = qdb_error_t_qdb_e_login_failed,
    #[doc = "! Column was not found."]
    ErrColumnNotFound = qdb_error_t_qdb_e_column_not_found,
    #[doc = "! Query is too complex."]
    ErrQueryTooComplex = qdb_error_t_qdb_e_query_too_complex,
    #[doc = "! Security key is invalid."]
    ErrInvalidCryptoKey = qdb_error_t_qdb_e_invalid_crypto_key,
    #[doc = "! Malformed query"]
    ErrInvalidQuery = qdb_error_t_qdb_e_invalid_query,
    #[doc = "! Malformed regex"]
    ErrInvalidRegex = qdb_error_t_qdb_e_invalid_regex,
    #[doc = "! Unknown user"]
    ErrUnknownUser = qdb_error_t_qdb_e_unknown_user,
    #[doc = "! Task has been interrupted"]
    ErrInterrupted = qdb_error_t_qdb_e_interrupted,
    #[doc = "! Client network buffer is too small"]
    ErrNetworkInbufTooSmall = qdb_error_t_qdb_e_network_inbuf_too_small,
    #[doc = "! Generic network error"]
    ErrNetworkError = qdb_error_t_qdb_e_network_error,
    #[doc = "! Corrupted data"]
    ErrDataCorruption = qdb_error_t_qdb_e_data_corruption,
}

impl ErrorType {
    pub fn from_qdb_error_origin_t(err: qdb_error_origin_t) -> ErrorType {
        match err {
            qdb_error_t_qdb_e_ok => ErrorType::Success,
            qdb_error_t_qdb_e_ok_created => ErrorType::Created,
            qdb_error_t_qdb_e_uninitialized => ErrorType::ErrUninitialized,
            qdb_error_t_qdb_e_alias_not_found => ErrorType::ErrAliasNotFound,
            qdb_error_t_qdb_e_alias_already_exists => ErrorType::ErrAliasAlreadyExists,
            qdb_error_t_qdb_e_out_of_bounds => ErrorType::ErrOutOfBounds,
            qdb_error_t_qdb_e_skipped  => ErrorType::ErrSkipped,
            qdb_error_t_qdb_e_incompatible_type => ErrorType::ErrIncompatibleType,
            qdb_error_t_qdb_e_container_empty => ErrorType::ErrContainerEmpty,
            qdb_error_t_qdb_e_container_full => ErrorType::ErrContainerFull,
            qdb_error_t_qdb_e_element_not_found => ErrorType::ErrElementNotFound,
            qdb_error_t_qdb_e_element_already_exists => ErrorType::ErrElementAlreadyExists,
            qdb_error_t_qdb_e_overflow => ErrorType::ErrOverflow,
            qdb_error_t_qdb_e_underflow => ErrorType::ErrUnderflow,
            qdb_error_t_qdb_e_tag_already_set => ErrorType::ErrTagAlreadySet,
            qdb_error_t_qdb_e_tag_not_set => ErrorType::ErrTagNotSet,
            qdb_error_t_qdb_e_timeout => ErrorType::ErrTimeout,
            qdb_error_t_qdb_e_connection_refused => ErrorType::ErrConnectionRefused,
            qdb_error_t_qdb_e_connection_reset => ErrorType::ErrConnectionReset,
            qdb_error_t_qdb_e_unstable_cluster => ErrorType::ErrUnstableCluster,
            qdb_error_t_qdb_e_try_again => ErrorType::ErrTryAgain,
            qdb_error_t_qdb_e_conflict => ErrorType::ErrConflict,
            qdb_error_t_qdb_e_not_connected => ErrorType::ErrNotConnected,
            qdb_error_t_qdb_e_resource_locked => ErrorType::ErrResourceLocked,
            qdb_error_t_qdb_e_system_remote => ErrorType::ErrSystemRemote,
            qdb_error_t_qdb_e_system_local => ErrorType::ErrSystemLocal,
            qdb_error_t_qdb_e_internal_remote => ErrorType::ErrInternalRemote,
            qdb_error_t_qdb_e_internal_local => ErrorType::ErrInternalLocal,
            qdb_error_t_qdb_e_no_memory_remote => ErrorType::ErrNoMemoryRemote,
            qdb_error_t_qdb_e_no_memory_local => ErrorType::ErrNoMemoryLocal,
            qdb_error_t_qdb_e_invalid_protocol => ErrorType::ErrInvalidProtocol,
            qdb_error_t_qdb_e_host_not_found => ErrorType::ErrHostNotFound,
            qdb_error_t_qdb_e_buffer_too_small => ErrorType::ErrBufferTooSmall,
            qdb_error_t_qdb_e_not_implemented => ErrorType::ErrNotImplemented,
            qdb_error_t_qdb_e_invalid_version => ErrorType::ErrInvalidVersion,
            qdb_error_t_qdb_e_invalid_argument => ErrorType::ErrInvalidArgument,
            qdb_error_t_qdb_e_invalid_handle => ErrorType::ErrInvalidHandle,
            qdb_error_t_qdb_e_reserved_alias => ErrorType::ErrReservedAlias,
            qdb_error_t_qdb_e_unmatched_content => ErrorType::ErrUnmatchedContent,
            qdb_error_t_qdb_e_invalid_iterator => ErrorType::ErrInvalidIterator,
            qdb_error_t_qdb_e_entry_too_large => ErrorType::ErrEntryTooLarge,
            qdb_error_t_qdb_e_transaction_partial_failure => ErrorType::ErrTransactionPartialFailure,
            qdb_error_t_qdb_e_operation_disabled => ErrorType::ErrOperationDisabled,
            qdb_error_t_qdb_e_operation_not_permitted => ErrorType::ErrOperationNotPermitted,
            qdb_error_t_qdb_e_iterator_end => ErrorType::ErrIteratorEnd,
            qdb_error_t_qdb_e_invalid_reply => ErrorType::ErrInvalidReply,
            qdb_error_t_qdb_e_no_space_left => ErrorType::ErrNoSpaceLeft,
            qdb_error_t_qdb_e_quota_exceeded => ErrorType::ErrQuotaExceeded,
            qdb_error_t_qdb_e_alias_too_long => ErrorType::ErrAliasTooLong,
            qdb_error_t_qdb_e_clock_skew => ErrorType::ErrClockSkew,
            qdb_error_t_qdb_e_access_denied => ErrorType::ErrAccessDenied,
            qdb_error_t_qdb_e_login_failed => ErrorType::ErrLoginFailed,
            qdb_error_t_qdb_e_column_not_found => ErrorType::ErrColumnNotFound,
            qdb_error_t_qdb_e_query_too_complex => ErrorType::ErrQueryTooComplex,
            qdb_error_t_qdb_e_invalid_crypto_key => ErrorType::ErrInvalidCryptoKey,
            qdb_error_t_qdb_e_invalid_query => ErrorType::ErrInvalidQuery,
            qdb_error_t_qdb_e_invalid_regex => ErrorType::ErrInvalidRegex,
            qdb_error_t_qdb_e_unknown_user => ErrorType::ErrUnknownUser,
            qdb_error_t_qdb_e_interrupted => ErrorType::ErrInterrupted,
            qdb_error_t_qdb_e_network_inbuf_too_small => ErrorType::ErrNetworkInbufTooSmall,
            qdb_error_t_qdb_e_network_error => ErrorType::ErrNetworkError,
            qdb_error_t_qdb_e_data_corruption => ErrorType::ErrDataCorruption,
            // Catch all for unknown or future error codes
            _ => ErrorType::ErrUnknownError,
        }
    }
}

