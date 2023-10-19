#include <qdb/client.h>
#include <stdlib.h>

int main()
{
    // doc-start-open
    qdb_handle_t handle;
    qdb_error_t error = qdb_open(&handle, qdb_p_tcp);
    if (error != qdb_e_ok) return EXIT_FAILURE;
    // doc-end-open

    // doc-start-connect
    qdb_error_t error_connect = qdb_connect(handle, "qdb://[::1]:2836");
    if (error_connect)
    {
        // error management
    }
    // doc-end-connect

    // doc-start-close
    qdb_error_t error_close = qdb_close(handle);
    if (error_close)
    {
        // error management
    }
    // doc-end-close

    return error_connect ? EXIT_FAILURE : EXIT_SUCCESS;
}
