
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

include!(concat!("/Users/marvin/CLionProjects/quasar-rs/gen", "/bindings.rs"));

pub mod error;
pub mod cluster;
pub mod cluster_endpoint;
pub mod handle;
pub mod handle_const;
mod utils;
pub mod handler_credentials;
mod utils_ptr;
