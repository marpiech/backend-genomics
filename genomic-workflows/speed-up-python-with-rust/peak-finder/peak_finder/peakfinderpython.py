OMP_NUM_THREADS = 1

import numpy as np
from scipy.signal import find_peaks

def read_file(path: str) -> list:
  with open(path) as data_file:
    data = np.array([float(line) for line in data_file])
    return data

def main():
    data = read_file("data/spectrum.dat")
    peaks, _ = find_peaks(data)   
    for peak in peaks:
        print(data[peak])

if __name__ == "__main__":
    main()
