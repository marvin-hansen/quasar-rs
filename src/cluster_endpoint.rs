
// Endpoint : A structure representing a qdb url endpoint
#[derive(Debug)]
pub struct Endpoint {
    address: String,
    port: u32,
}

impl Endpoint {
    pub fn new(address: String, port: u32) -> Endpoint {
        Endpoint { address, port}
    }
    pub fn uri(&self) -> String {
        format!("{}:{:?}", self.address, self.port)
    }
    pub fn address(&self) -> &str {
        &self.address
    }
    pub fn port(&self) -> u32 {
        self.port
    }
}
