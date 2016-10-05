## INE5420

This repository contains the exercises and projects developed during the INE5420 course towards the BCC degree at the Federal University of Santa Catarina.

### Branches

Master

[![Build Status](https://travis-ci.org/quenio/INE5420.svg?branch=master)](https://travis-ci.org/quenio/INE5420)

Develop

[![Build Status](https://travis-ci.org/quenio/INE5420.svg?branch=develop)](https://travis-ci.org/quenio/INE5420)

### Team

- Daniel Felipe Schröder (@mrdanielfsch)
- Quenio Cesar Machado dos Santos (@quenio)

### Usage

On Ubuntu 14.04 you'll need to install the following packages:
- pkg-config
- libgtk-3-dev
- libgtkmm-3.0-dev

```bash
sudo apt-get install -y pkg-config libgtk-3-dev libgtkmm-3.0-dev
```

After that, just run make from inside the ```graphics```folder:

```bash
cd graphics
make
```

### History

- 1.0: Initial setup with display file rendered on viewport.
- 1.1: Basic system with window navigation and zoom.
- 1.2: 2D transformations and homogeneous coordinates.
- 1.3: Window normalized coordinates system.
- 1.4: Point, Line and Polygon Clipping.
- 1.5: Bézier parametric curves.
