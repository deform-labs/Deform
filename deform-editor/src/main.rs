use deform_core::{logger, render::NullRenderer, scene::Scene};

fn main() {
    // Initialize logger
    logger::init();
    logger::info("Starting Deform Editor...");

    // Create a scene
    let mut scene = Scene::new();
    let entity_id = scene.create_entity("Test Entity");
    logger::info(&format!("Created entity with id: {}", entity_id));

    // Create a renderer
    let renderer = NullRenderer;
    renderer.clear();

    logger::info("Deform Editor initialized successfully!");
    println!("Welcome to Deform Editor v{}", deform_core::VERSION);
}
