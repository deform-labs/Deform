// Logger module stub
use std::sync::Once;

static INIT: Once = Once::new();

pub fn init() {
    INIT.call_once(|| {
        println!("Logger initialized");
    });
}

pub fn info(msg: &str) {
    println!("[INFO] {}", msg);
}

pub fn error(msg: &str) {
    eprintln!("[ERROR] {}", msg);
}

pub fn debug(msg: &str) {
    #[cfg(debug_assertions)]
    println!("[DEBUG] {}", msg);
}
