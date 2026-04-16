// Render module stub
pub trait Renderer {
    fn render(&self);
    fn clear(&self);
}

pub struct NullRenderer;

impl Renderer for NullRenderer {
    fn render(&self) {
        // Null renderer does nothing
    }

    fn clear(&self) {
        // Null renderer does nothing
    }
}

pub struct OpenGLRenderer;

impl OpenGLRenderer {
    pub fn new() -> Self {
        Self
    }
}

impl Renderer for OpenGLRenderer {
    fn render(&self) {
        println!("OpenGL rendering");
    }

    fn clear(&self) {
        println!("OpenGL clearing");
    }
}
