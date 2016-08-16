## INE5420

[![Build Status](https://travis-ci.org/quenio/INE5420.svg?branch=develop)](https://travis-ci.org/quenio/INE5420)

This repository contains the exercises and projects developed during the INE5420 course towards the BCC degree at the Federal University of Santa Catarina.

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