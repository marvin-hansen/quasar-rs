use std::fs::File;
use std::io::Read;
use serde::{Deserialize, Serialize};

#[derive(Debug, Serialize, Deserialize, Clone)]
pub struct JSONCredentialsConfig {
    username: String,
    secret_key: String,
}

impl JSONCredentialsConfig {
    pub fn new(username: String, secret: String) -> Self {
        Self { username, secret_key: secret }
    }
    pub fn username(&self) -> &str {
        &self.username
    }
    pub fn secret(&self) -> &str {
        &self.secret_key
    }
}

pub fn user_credentials_from_file(file_path: &str) -> Option<JSONCredentialsConfig > {

    let content = match parse_file(file_path) {
        Some(res) => res,
        None => return None,
    };

    let credentials: JSONCredentialsConfig = serde_json::from_str(&content)
        .expect("Unable to parse JSON from file");

    Some(credentials)
}

#[derive(Debug, Serialize, Deserialize, Clone)]
pub struct ClusterKey {
    cluster_key: String,
}

pub fn cluster_key_from_file(file_path: &str) -> Option<ClusterKey > {

    let content = match parse_file(file_path) {
        Some(res) => res,
        None => return None,
    };

    let cluster_key: ClusterKey = serde_json::from_str(&content)
        .expect("Unable to parse JSON from file");

    Some(cluster_key)
}

impl ClusterKey {
    pub fn new(cluster_key: String) -> Self {
        Self { cluster_key }
    }
    pub fn cluster_key(&self) -> &str {
        &self.cluster_key
    }

}


fn parse_file(file_path: &str) -> Option<String>{

    let mut file = File::open(file_path)
        .expect("Unable to open file");

    let mut contents = String::new();

    file.read_to_string(&mut contents)
        .expect("Unable to read file");

    Some(contents)
}