# Ophanim down

Download the files which an ophanim database references. Written *blazingly fast* with C++ 20.

## Install compiler & dependencies

### Debian (13+)

```bash
apt install \
    build-essential \
    cmake \
    libcurl4-openssl-dev \
    libspdlog-dev \
    nlohmann-json3-dev \
    libsqlite3-dev \
    libzip-dev zipmerge zipcmp ziptool
```

### Debian (12)

```bash
apt install \
    build-essential \
    cmake \
    libcurl4-openssl-dev \
    libspdlog-dev \
    nlohmann-json3-dev \
    libsqlite3-dev \
    libfmt-dev \
    libzip-dev zipmerge zipcmp ziptool
```

### Fedora

```bash
dnf install -y \
    @development-tools \
    cmake \
    libcurl-devel \
    spdlog-devel \
    nlohmann-json-devel \
    sqlite-devel \
    libzip-devel \
    libzip-utils
```

### Arch Linux

```bash

pacman -S --needed \
    base-devel \
    cmake \
    curl \
    spdlog \
    nlohmann-json \
    sqlite \
    libzip 
```

## Build

```bash
cmake -S . -B build && cmake --build build -j$(nproc)
```

The executable `ophanim-down` will now be under the *build* directory.
