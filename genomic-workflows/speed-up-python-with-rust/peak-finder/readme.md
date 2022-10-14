### Rust
```
cargo build
time cargo run > /dev/null
```
### Python
```
time python3 peak_finder/peakfinderpython.py  > /dev/null
```
### Rust & Python
```
maturin build
pip install --force-reinstall target/wheels/find_peaks-0.1.0-cp39-cp39-linux_x86_64.whl
time python3 peak_finder/peakfinderrust.py > /dev/null
```
### Just test
```
time cargo build
time cargo run > /dev/null
time python3 peak_finder/peakfinderpython.py  > /dev/null
time python3 peak_finder/peakfinderrust.py > /dev/null
```