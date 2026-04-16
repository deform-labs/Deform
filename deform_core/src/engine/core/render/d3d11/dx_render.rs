use crate::engine::{
    core::render::i_renderer::IRenderer, engine_api::DeformResult, graphics::window::Window,
};

#[derive(Debug, Default)]
pub struct DXRender {
    initialized: bool,
    pub width: u32,
    pub height: u32,
    pub clear_color: [f32; 4],
}

impl IRenderer for DXRender {
    fn initialize(&mut self, window: &Window) -> DeformResult<()> {
        self.width = window.width();
        self.height = window.height();
        self.clear_color = [0.1, 0.1, 0.1, 1.0];
        self.initialized = true;
        Ok(())
    }

    fn shutdown(&mut self) {
        self.initialized = false;
    }

    fn is_initialized(&self) -> bool {
        self.initialized
    }

    fn begin_frame(&mut self, clear_color: [f32; 4]) {
        self.clear_color = clear_color;
    }

    fn present(&mut self) {}

    fn resize(&mut self, width: u32, height: u32) {
        self.width = width;
        self.height = height;
    }
}
