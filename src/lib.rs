// Deform Core Library
// Main exports

pub mod logger;
pub mod render;
pub mod scene;

// Re-export commonly used items
pub use logger::{info, error, debug};
pub use render::{Renderer, NullRenderer};
pub use scene::Scene;

// Version information
pub const VERSION: &str = env!("CARGO_PKG_VERSION");
