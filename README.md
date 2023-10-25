# quasar.rs
Rust binding for quasar.ai DB

exporting these doesn't work at all; not sure why.

export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/Users/marvin/CLionProjects/quasar-rs/qdb/lib

export DYLD_FALLBACK_LIBRARY_PATH=DYLD_FALLBACK_LIBRARY_PATH:/Users/marvin/CLionProjects/quasar-rs/qdb/lib

## Start Quasar DB server via Docker

### On AMD/Intel

docker run -d --name qdb-server bureau14/qdb

### On Apple Silicon  (M1/M2)

docker run --platform linux/amd64 -d --name qdb-server bureau14/qdb

### Start Quasar Shell and connect to the server

docker run --platform linux/amd64 -ti --link qdb-server:qdb-server bureau14/qdbsh --cluster qdb://qdb-server:2836