# Multi-stage Dockerfile for C++ Crow REST API Backend
FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

# Install build tools & dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    g++ \
    git \
    libsqlite3-dev \
    curl \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Configure and compile with single job -j 1 to prevent OOM on free cloud tiers (Render 512MB RAM limit)
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
RUN cmake --build build --config Release --target ums_server -- -j 1

# Final lightweight runner image
FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    libsqlite3-0 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /app/build/bin/ums_server /app/ums_server

EXPOSE 8080

CMD ["/app/ums_server"]
