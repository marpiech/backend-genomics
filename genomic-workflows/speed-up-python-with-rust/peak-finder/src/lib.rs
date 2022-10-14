use pyo3::prelude::*;
#[allow(unused_imports)]
use std::fs::File;
use std::io::Read;
use find_peaks::PeakFinder;

/// Returns the sum of two numbers (only Rust)
fn rust_sum(a: i64, b: i64) -> i64 {
    a + b
}

fn read_file(path: &str) -> std::io::Result<String> {
    let mut file = File::open(path)?;
    let mut contents = String::new();
    file.read_to_string(&mut contents)?;
    Ok(contents)
}

#[pyfunction]
fn get_peaks() -> PyResult<String> {
    let data: Vec<f64> = read_file("data/spectrum.dat").expect("File not read!").as_str()
        .split_whitespace()
        .map(|x| x.parse::<f64>().unwrap())
        .collect();
    
    let mut fp = PeakFinder::new(&data);
    fp.with_min_prominence(200.);
    fp.with_min_height(0.);

    let peaks = fp.find_peaks();
    for p in peaks {
        println!("{} {}", p.middle_position(), p.height.unwrap());
    }
    Ok("".to_string())
}

/// Returns the sum of two numbers, wrapped by pyo3
#[pyfunction]
fn pyo3_sum(a: i64, b: i64) -> PyResult<i64> {
    Ok(rust_sum(a, b))
}

/// A Python module implemented in Rust.
#[pymodule]
#[pyo3(name = "find_peaks")]
fn _rust(_py: Python, m: &PyModule) -> PyResult<()> {
    m.add_function(wrap_pyfunction!(pyo3_sum, m)?)?;
    m.add_function(wrap_pyfunction!(get_peaks, m)?)?;
    Ok(())
}