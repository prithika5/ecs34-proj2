FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    make \
    cmake \
    git \
    libgtest-dev \
    libexpat1-dev \
    pkg-config \
    lcov \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
