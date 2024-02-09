#include <qdb/client.h>
#include <stdlib.h>

int main(void)
{
    // doc-start-open
    qdb_handle_t handle;
    qdb_error_t error = qdb_open(&handle, qdb_p_tcp);
    if (error != qdb_e_ok) return EXIT_FAILURE;
    // doc-end-open

    // doc-start-connect
    const char * remote_nodes = "qdb://192.168.1.1:2836,192.168.1.2:2836,192.168.1.3:2836";

    // The call to qdb_connect will be successful if any of the connections succeeds.
    qdb_error_t error_connect = qdb_connect(handle, remote_nodes);
    if (error_connect)
    {
        // error management
    }
    // doc-end-connect

    // doc-start-close
    error = qdb_close(handle);
    if (error != qdb_e_ok) return EXIT_FAILURE;
    // doc-end-close

    return EXIT_SUCCESS;
}
