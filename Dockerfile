# ============================================
# Stage 1: Builder
# ============================================
FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libcurl4-openssl-dev \
    libssl-dev \
    zlib1g-dev \
    libboost-system-dev \
    libboost-thread-dev \
    libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY CMakeLists.txt .
COPY include/ include/
COPY src/ src/

RUN mkdir build && cd build \
    && cmake .. -DCMAKE_BUILD_TYPE=Release \
    && make -j$(nproc)

# ============================================
# Stage 2: Runtime
# ============================================
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    libcurl4 \
    libssl3 \
    zlib1g \
    libboost-system1.83.0 \
    libboost-thread1.83.0 \
    libsqlite3-0 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /build/build/english_buddy_bot .

RUN mkdir -p /app/data

CMD ["./english_buddy_bot"]
