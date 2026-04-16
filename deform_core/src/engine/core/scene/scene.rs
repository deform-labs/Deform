use crate::engine::core::object::object::Object;

#[derive(Debug, Default)]
pub struct Scene {
    objects: Vec<Object>,
    next_id: u64,
}

impl Scene {
    pub fn create_object(&mut self, name: impl Into<String>) -> u64 {
        let id = self.next_id;
        self.next_id += 1;

        self.objects.push(Object {
            id,
            name: name.into(),
            transform: Default::default(),
        });

        id
    }

    pub fn objects(&self) -> &[Object] {
        &self.objects
    }
}
