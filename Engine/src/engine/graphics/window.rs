use wgpu;
use winit;

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
pub enum WindowGraphicsApi {
    Dx11,
    Vulkan,
    OpenGL,
}

#[derive(Clone, Debug)]
pub struct Window {
    title: String,
    width: u32,
    height: u32,
    created: bool,
    api: WindowGraphicsApi,
}

impl Default for Window {
    fn default() -> Self {
        Self {
            title: String::new(),
            width: 0,
            height: 0,
            created: false,
            api: WindowGraphicsApi::Dx11,
        }
    }
}

impl Window {
    pub fn create_with_icon(
        &mut self,
        name: &str,
        icon_path: &str,
        width: u32,
        height: u32,
        api: WindowGraphicsApi,
    ) -> bool {
        self.title = name.to_owned();
        self.width = width;
        self.height = height;
        self.created = true;
        self.api = api;

        return true;
    }

    pub fn create(&mut self, name: &str, width: u32, height: u32, api: WindowGraphicsApi) -> bool {
        self.create_with_icon(name, "", width, height, api)
    }

    pub fn destroy(&mut self) {
        self.created = false;
    }

    pub fn should_close(&self) -> bool {
        !self.created
    }

    pub fn width(&self) -> u32 {
        self.width
    }

    pub fn height(&self) -> u32 {
        self.height
    }

    pub fn api(&self) -> WindowGraphicsApi {
        self.api
    }

    pub fn title(&self) -> &str {
        &self.title
    }
}
