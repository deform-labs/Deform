use std::sync::{Mutex, OnceLock};

static LOGS: OnceLock<Mutex<Vec<String>>> = OnceLock::new();

pub struct Logger;

impl Logger {
    fn storage() -> &'static Mutex<Vec<String>> {
        LOGS.get_or_init(|| Mutex::new(Vec::new()))
    }

    pub fn log(message: impl Into<String>) {
        let message = message.into();
        println!("[LOG] {message}");

        if let Ok(mut guard) = Self::storage().lock() {
            guard.push(message);
        }
    }

    pub fn fatal_error(message: impl Into<String>) -> ! {
        let message = message.into();
        eprintln!("[FATAL] {message}");

        if let Ok(mut guard) = Self::storage().lock() {
            guard.push(format!("FATAL: {message}"));
        }

        panic!("{message}");
    }

    pub fn get_logs() -> Vec<String> {
        match Self::storage().lock() {
            Ok(guard) => guard.clone(),
            Err(_) => vec!["Logger history unavailable".to_owned()],
        }
    }
}
