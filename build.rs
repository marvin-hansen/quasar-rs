use std::env;
use std::path::PathBuf;

fn main() {

    // let key = "LLVM_CONFIG_PATH";
    // env::set_var(key, env::var(key).unwrap_or("/opt/homebrew/opt/llvm/bin/llvm-config".to_string()));

    // Tell cargo to look for shared libraries in the specified directory
    println!("cargo:rustc-link-search=/Users/marvin/CLionProjects/quasar-rs/qdb/lib/");

    // Tell rustc to link the qdb library.
    println!("cargo:rustc-link-lib=libqdb_api");

    // Tell cargo to invalidate the built crate whenever the wrapper changes
    println!("cargo:rerun-if-changed=wrapper.h");

    // The bindgen::Builder is the main entry point to bindgen,
    // and lets you build up options for the resulting bindings.
    let bindings = bindgen::Builder::default()
        // Derive debug implementation for structs and enums
        .derive_debug(true)
        // The input header we would like to generate bindings for.
        .header("wrapper.h")
        // Tell cargo to invalidate the built crate whenever any of the
        // included header files changed.
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        // Finish the builder and generate the bindings.
        .generate()
        // Unwrap the Result and panic on failure.
        .expect("Unable to generate bindings");

    // Write the bindings to the $OUT_DIR/bindings.rs file.
    // The resulting bindings will be written to $OUT_DIR/bindings.rs where $OUT_DIR is chosen by cargo and is something like
    // ./target/debug/build/quasar-rs..../out/.
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());

    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}