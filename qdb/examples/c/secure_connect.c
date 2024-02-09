#include <qdb/client.h>
#include <qdb/option.h>
#include <stdlib.h>

int main(void)
{
    qdb_handle_t handle;
    qdb_error_t error = qdb_open(&handle, qdb_p_tcp);
    if (QDB_FAILURE(error))
    {
        // error management
    }
    // doc-start-secure-connect
    error = qdb_option_set_cluster_public_key(handle, "PZMBhqk43w+HNr9lLGe+RYq+qWZPrksFWMF1k1UG/vwc=");
    if (QDB_FAILURE(error))
    {
        // error management
    }

    error = qdb_option_set_user_credentials(handle, "user_name", "SL8sm9dM5xhPE6VNhfYY4ib4qk3vmAFDXCZ2FDi8AuJ4=");
    if (QDB_FAILURE(error))
    {
        // error management
    }
    // doc-end-secure-connect

    // doc-start-set-encryption
    error = qdb_option_set_encryption(handle, qdb_crypt_aes_gcm_256);
    if (QDB_FAILURE(error))
    {
        // error management
    }
    // doc-end-set-encryption

    error = qdb_connect(handle, "qdb://localhost:2836");
    if (QDB_FAILURE(error))
    {
        // error management
    }

    return error ? EXIT_FAILURE : EXIT_SUCCESS;
}
