import numpy as np
from scipy.signal import find_peaks

def read_file(path: str) -> list()
  for line in open(path):
    data = np.array((array.float(i) for i in line.split(' ')))
    arrays.append(data)


fn main() -> () {
    let data: Vec<f64> = read_file("../../data/spectrum.dat").expect("File not read!").as_str()
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
}

def main():
fn main() -> () {
    let data: Vec<f64> = read_file("../../data/spectrum.dat").expect("File not read!").as_str()
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
}


if __name__ == "__main__":
    main()
