FROM ubuntu:22.04

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install C compiler, build tools, and utilities
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    gdb \
    make \
    curl \
    git \
    dos2unix \
    && rm -rf /var/lib/apt/lists/*

# Create a non-root user for VS Code
RUN useradd -m vscode

# Set working directory and user
USER vscode
WORKDIR /home/vscode
