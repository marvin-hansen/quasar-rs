use crate::{qdb_compression_t, qdb_compression_t_qdb_comp_best, qdb_compression_t_qdb_comp_fast, qdb_compression_t_qdb_comp_none, qdb_encryption_t, qdb_encryption_t_qdb_crypt_aes_gcm_256,
            qdb_encryption_t_qdb_crypt_none, qdb_protocol_t, qdb_protocol_t_qdb_p_tcp};


// Protocol : A network protocol.
pub type Protocol = qdb_protocol_t;

// Protocol values:
//	ProtocolTCP : Uses TCP/IP to communicate with the cluster. This is currently the only supported network protocol.
pub const PROTOCOL_DEFAULT: Protocol = qdb_protocol_t_qdb_p_tcp;

// Compression : compression parameter
pub type Compression = qdb_compression_t;

// Compression values:
//	CompNone : No compression.
//	CompFast : Maximum compression speed, potentially minimum compression ratio. This is currently the default.
//	CompBest : Maximum compression ratio, potentially minimum compression speed. This is currently not implemented.
pub const CompNone: Compression = qdb_compression_t_qdb_comp_none;
pub const CompFast: Compression = qdb_compression_t_qdb_comp_fast;
pub const CompBest: Compression = qdb_compression_t_qdb_comp_best;


// Encryption : encryption option
pub type Encryption = qdb_encryption_t;

pub const EncryptNone: Encryption = qdb_encryption_t_qdb_crypt_none;
pub const EncryptAES: Encryption = qdb_encryption_t_qdb_crypt_aes_gcm_256;
