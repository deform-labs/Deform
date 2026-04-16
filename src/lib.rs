// This will compile to deform.dll
#[no_mangle]
pub extern "C" fn engine_init() {
    println!("Engine initialized");
}
