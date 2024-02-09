use std::env;
use std::path::{Path, PathBuf};
use bindgen::CargoCallbacks;

fn main() {
    // Tried this to no avail. https://github.com/rust-lang/cargo/issues/4421
    // install_name_tool -change @rpath/libqdb_api.dylib "/Users/marvin/CLionProjects/quasar-rs/qdb/lib/libqdb_api.dylib" target/debug/libqdb_api.dylib

    // Set LLVM config & libclang path https://rust-lang.github.io/rust-bindgen/requirements.html#clang
    let key = "LLVM_CONFIG_PATH";
    env::set_var(key, env::var(key).unwrap_or("/opt/homebrew/opt/llvm/bin/llvm-config".to_string()));

    let key = "LIBCLANG_PATH";
    env::set_var(key, env::var(key).unwrap_or("/opt/homebrew/opt/llvm/lib".to_string()));

    // linking c to the underlying dylib library
    // https://www.reddit.com/r/rust/comments/885t1h/bindgen_linking_question/
    // println!("cargo:rustc-link-lib=dylib=c++");
    // println!("cargo:rustc-link-lib=dylib=c++abi");
    println!("cargo:rustc-link-lib=libqdb_api");

    // Set the dylib search path relative to the current crate
    // https://stackoverflow.com/questions/41917096/how-do-i-make-rustc-link-search-relative-to-the-project-location
    let dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    println!("cargo:rustc-link-search=native={}", Path::new(&dir).join("qdb/lib").display());

    // Tell rustc to link the qdb library.
    println!("cargo:rustc-link-lib=libqdb_api");

    // Tell cargo to invalidate the built crate whenever the wrapper changes
    println!("cargo:rerun-if-changed=wrapper.h");

    // The bindgen::Builder is the main entry point to bindgen,
    // and lets you build up options for the resulting bindings.
    let bindings = bindgen::Builder::default()
        // The input header we would like to generate bindings for.
        .header("wrapper.h")
        // Derive debug implementation for structs and enums
        .derive_debug(true)
        // Tell cargo to invalidate the built crate whenever any of the header files changed.
        .parse_callbacks(Box::new(CargoCallbacks::new()))
        // Finish the builder and generate the bindings.
        .generate()
        // Unwrap the Result and panic on failure.
        .expect("Unable to generate bindings");

    // Write the bindings to the $OUT_DIR/bindings.rs file.
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}