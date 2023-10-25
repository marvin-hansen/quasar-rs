use quasar_rs::handle;

fn main() {
    let cluster_uri = "qdb://127.0.0.1:2836";
    let timeout = 1000;

    let handle = handle::setup_handle(cluster_uri, timeout);

    assert!(handle.is_ok());

    let handle = handle.unwrap();

    let max_buffer = handle.get_client_max_int_buffer_size();
    assert!(max_buffer.is_ok());
}