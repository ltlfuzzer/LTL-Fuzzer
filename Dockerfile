# Set the base image
FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

# Set the working directory
WORKDIR /LTL-Fuzzer

# Install dependencies
RUN apt-get update -y && apt-get install -y \
    build-essential \
    make \
    cmake \
    ninja-build \
    git \
    binutils-gold \
    binutils-dev \
    curl \
    wget \
    libboost-all-dev \
    libboost-dev \
    python3 \
    python3-dev \
    python3-pip

# Copy the project files into the Docker image
COPY . .
# Install LLVM 11.0.0 with Gold-plugin
# You may need to modify this part to install LLVM 11.0.0 and its Gold-plugin
RUN chmod +x $PWD/scripts/build-llvm-11.sh&& \
    $PWD/scripts/build-llvm-11.sh

RUN cp /usr/lib/llvm-11/lib/libLTO.so /usr/lib/bfd-plugins/
RUN cp /usr/lib/llvm-11/lib/LLVMgold.so /usr/lib/bfd-plugins/ 
# Install spot 2.9.7
# You may need to modify this part to install spot 2.9.7
RUN  chmod +x $PWD/scripts/install-spot.sh && \
    $PWD/scripts/install-spot.sh
# Upgrade pip and install Python modules
RUN pip3 install --upgrade pip && \
    pip3 install networkx pydot pydotplus



# Build the project
RUN chmod +x $PWD/scripts/build.sh && $PWD/scripts/build.sh
