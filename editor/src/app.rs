use deform_core::{
    logger,
    render::{NullRenderer, Renderer},
    scene::Scene,
};
use eframe::egui;

#[derive(Debug)]
pub struct UiState {
    show_hierarchy: bool,
    show_inspector: bool,
    show_scene: bool,
    show_console: bool,
    clear_color: [f32; 4],
    selected_entity: Option<u64>,
}

impl Default for UiState {
    fn default() -> Self {
        Self {
            show_hierarchy: true,
            show_inspector: true,
            show_scene: true,
            show_console: true,
            clear_color: [0.1, 0.1, 0.1, 1.0],
            selected_entity: None,
        }
    }
}

pub struct DeformEditorApp {
    state: UiState,
    scene: Scene,
    renderer: NullRenderer,
}

impl Default for DeformEditorApp {
    fn default() -> Self {
        let mut scene = Scene::default();
        scene.create_entity("Main Camera");
        scene.create_entity("Directional Light");
        scene.create_entity("Cube");

        let mut renderer = NullRenderer::default();
        if let Err(err) = renderer.initialize() {
            logger::fatal_error(format!("Renderer initialization failed: {err}"));
        }

        Self {
            state: UiState::default(),
            scene,
            renderer,
        }
    }
}

impl DeformEditorApp {
    fn draw_menu_bar(&mut self, ctx: &egui::Context) {
        egui::TopBottomPanel::top("main_menu").show(ctx, |ui| {
            egui::MenuBar::new().ui(ui, |ui| {
                ui.menu_button("File", |ui| {
                    if ui.button("New Scene").clicked() {
                        logger::log("New Scene Created");
                        ui.close();
                    }
                    if ui.button("Open").clicked() {
                        logger::log("Open clicked");
                        ui.close();
                    }
                    if ui.button("Save").clicked() {
                        logger::log("Save clicked");
                        ui.close();
                    }
                });

                ui.menu_button("View", |ui| {
                    ui.checkbox(&mut self.state.show_hierarchy, "Hierarchy");
                    ui.checkbox(&mut self.state.show_inspector, "Inspector");
                    ui.checkbox(&mut self.state.show_scene, "Scene");
                    ui.checkbox(&mut self.state.show_console, "Console");
                });

                ui.menu_button("Render", |ui| {
                    ui.color_edit_button_rgba_unmultiplied(&mut self.state.clear_color);
                });
            });
        });
    }

    fn draw_scene_panel(&mut self, ctx: &egui::Context) {
        egui::CentralPanel::default().show(ctx, |ui| {
            ui.painter().rect_filled(
                ui.max_rect(),
                0.0,
                egui::Color32::from_rgba_unmultiplied(
                    (self.state.clear_color[0] * 255.0) as u8,
                    (self.state.clear_color[1] * 255.0) as u8,
                    (self.state.clear_color[2] * 255.0) as u8,
                    (self.state.clear_color[3] * 255.0) as u8,
                ),
            );
        });

        if self.state.show_scene {
            egui::Window::new("Scene")
                .resizable(true)
                .show(ctx, |ui| {
                    ui.label("Scene viewport placeholder");
                    ui.label("Wire your world rendering here.");
                });
        }
    }

    fn draw_hierarchy(&mut self, ctx: &egui::Context) {
        if !self.state.show_hierarchy {
            return;
        }

        egui::Window::new("Hierarchy").show(ctx, |ui| {
            for entity in self.scene.entities() {
                let selected = self.state.selected_entity == Some(entity.id);
                if ui.selectable_label(selected, &entity.name).clicked() {
                    self.state.selected_entity = Some(entity.id);
                }
            }
        });
    }

    fn draw_inspector(&mut self, ctx: &egui::Context) {
        if !self.state.show_inspector {
            return;
        }

        egui::Window::new("Inspector").show(ctx, |ui| match self.state.selected_entity {
            Some(selected_id) => {
                if let Some(entity) = self.scene.entities().iter().find(|e| e.id == selected_id) {
                    ui.heading(&entity.name);
                    ui.label(format!("Entity ID: {}", entity.id));
                } else {
                    ui.label("Selected entity does not exist.");
                }
            }
            None => {
                ui.label("No entity selected.");
            }
        });
    }

    fn draw_console(&mut self, ctx: &egui::Context) {
        if !self.state.show_console {
            return;
        }

        egui::Window::new("Console").show(ctx, |ui| {
            for line in logger::history() {
                ui.monospace(line);
            }
        });
    }
}

impl eframe::App for DeformEditorApp {
    fn update(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame) {
        let screen_size = ctx.input(|i| i.content_rect().size());
        self.renderer
            .resize(screen_size.x.max(1.0) as u32, screen_size.y.max(1.0) as u32);
        self.renderer.begin_frame(self.state.clear_color);

        self.draw_menu_bar(ctx);
        self.draw_scene_panel(ctx);
        self.draw_hierarchy(ctx);
        self.draw_inspector(ctx);
        self.draw_console(ctx);

        self.renderer.present();
    }
}
