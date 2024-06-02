#[no_mangle]
pub extern "C" fn printtest(fn_in: u8) {
    println!("Hello from Rust: {}", fn_in);
}