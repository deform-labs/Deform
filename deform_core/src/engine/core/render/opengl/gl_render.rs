use crate::engine::{
    core::render::i_renderer::IRenderer, engine_api::DeformResult, graphics::window::Window,
};

#[derive(Debug, Default)]
pub struct GLRender {
    initialized: bool,
}

impl IRenderer for GLRender {
    fn initialize(&mut self, _window: &Window) -> DeformResult<()> {
        self.initialized = true;
        Ok(())
    }

    fn shutdown(&mut self) {
        self.initialized = false;
    }

    fn is_initialized(&self) -> bool {
        self.initialized
    }

    fn begin_frame(&mut self, _clear_color: [f32; 4]) {}

    fn present(&mut self) {}

    fn resize(&mut self, _width: u32, _height: u32) {}
}
