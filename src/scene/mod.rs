// Scene module stub
use std::collections::HashMap;

#[derive(Debug, Clone)]
pub struct Entity {
    pub id: u64,
    pub name: String,
}

pub struct Scene {
    entities: HashMap<u64, Entity>,
    next_id: u64,
}

impl Scene {
    pub fn new() -> Self {
        Self {
            entities: HashMap::new(),
            next_id: 0,
        }
    }

    pub fn create_entity(&mut self, name: &str) -> u64 {
        let id = self.next_id;
        self.next_id += 1;
        self.entities.insert(id, Entity {
            id,
            name: name.to_string(),
        });
        id
    }

    pub fn get_entity(&self, id: u64) -> Option<&Entity> {
        self.entities.get(&id)
    }

    pub fn remove_entity(&mut self, id: u64) -> Option<Entity> {
        self.entities.remove(&id)
    }

    pub fn entity_count(&self) -> usize {
        self.entities.len()
    }
}

impl Default for Scene {
    fn default() -> Self {
        Self::new()
    }
}
