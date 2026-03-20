FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive
ARG CUBECLT_VERSION=1.21.0
ARG CUBECLT_INSTALLER=cubeclt_${CUBECLT_VERSION}_installer.sh
ARG UV_INSTALLER_URL=https://astral.sh/uv/install.sh

RUN echo "deb http://security.ubuntu.com/ubuntu focal-security main universe" > /etc/apt/sources.list.d/ubuntu-focal-sources.list && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
        bash ca-certificates curl git git-lfs \
        python3 python3-pip python3-venv \
        cmake ninja-build \
        g++ build-essential \
        libncurses5 libusb-1.0-0 \
        gdb && \
    git lfs install --system && \
    rm -rf /var/lib/apt/lists/*

RUN curl -LsSf "${UV_INSTALLER_URL}" | sh && \
    ln -sf /root/.local/bin/uv /usr/local/bin/uv && \
    python3 -m pip install --break-system-packages \
        Jinja2==3.1.5 \
        GitPython==3.1.43

# Install cubeclt (STM32CubeCLT)
WORKDIR /tmp

RUN git clone --depth 1 https://github.com/Hyperloop-UPV/cubeclt.git && \
    cd cubeclt && \
    git lfs pull --include="${CUBECLT_INSTALLER}" && \
    chmod +x ${CUBECLT_INSTALLER} && \
    echo | LICENSE_ALREADY_ACCEPTED=1 ./${CUBECLT_INSTALLER} && \
    actual_root="$(find /opt/st -maxdepth 1 -type d -name "stm32cubeclt_${CUBECLT_VERSION}*" | head -n1)" && \
    test -n "${actual_root}" && \
    mkdir -p /opt/ST && \
    ln -s "${actual_root}" "/opt/ST/STM32CubeCLT_${CUBECLT_VERSION}" && \
    cd / && \
    rm -rf /tmp/cubeclt

ENV STM32_CLT_ROOT=/opt/ST/STM32CubeCLT_${CUBECLT_VERSION}
ENV HYPER_STM32CLT_ROOT=/opt/ST/STM32CubeCLT_${CUBECLT_VERSION}
ENV PATH="/root/.local/bin:${STM32_CLT_ROOT}/GNU-tools-for-STM32/bin:${STM32_CLT_ROOT}/STM32CubeProgrammer/bin:${STM32_CLT_ROOT}/CMake/bin:${STM32_CLT_ROOT}/Ninja/bin:${PATH}"

WORKDIR /workspace
