---

marp: true
theme: default
class: invert

---

# What is docker?

![width:200](images/docker-logo.png)

A tool to **build**, **distribute**, and **run** applications by using **containers**



---

# Build

Dockerfile - a text file that contains all the commands a user could call on the command line to assemble an image


```dockerfile
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y python3
RUN echo "print('Hello World')" > hello.py
```

```bash
$ docker build -t my-python-app .
```

---
<!-- backgroundColor: #0D1117 -->
![width:200px](images/edit.gif)


---

# What is a container?

A container is a lightweight, standalone, and executable package that includes everything an application needs to run

```dockerfile
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y python3
```
