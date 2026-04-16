use crate::engine::{engine_api::DeformResult, graphics::window::Window};

pub trait IRenderer {
    fn initialize(&mut self, window: &Window) -> DeformResult<()>;
    fn shutdown(&mut self);
    fn is_initialized(&self) -> bool;
    fn begin_frame(&mut self, clear_color: [f32; 4]);
    fn present(&mut self);
    fn resize(&mut self, width: u32, height: u32);
}
