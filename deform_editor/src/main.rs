mod app;

use deform;
use app::DeformEditorApp;
use deform_core::logger;

fn main() {
    println!("Editor starting...");
    deform::engine_init();

    logger::log("Starting Deform Editor.");

    let options = eframe::NativeOptions::default();
    let result = eframe::run_native(
        "Deform Editor - Rust",
        options,
        Box::new(|_cc| Ok(Box::new(DeformEditorApp::default()))),
    );

    if let Err(error) = result {
        logger::fatal_error(format!("Failed to start editor: {error}"));
    }
}
