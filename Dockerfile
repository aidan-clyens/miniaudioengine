# Use the latest stable Ubuntu image
FROM ubuntu:24.04

ARG ENABLE_RC=false

# Set non-interactive mode for apt-get
ENV DEBIAN_FRONTEND=noninteractive \
    PATH="/opt/venv/bin:$PATH"

# Install prerequisites and Kitware keyring
RUN set -eux; \
    apt-get update; \
    apt-get install -y --no-install-recommends ca-certificates gpg wget; \
    # Fetch and install Kitware keyring
    wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc \
      | gpg --dearmor - > /usr/share/keyrings/kitware-archive-keyring.gpg; \
    # Add Kitware repository
    echo "deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ noble main" \
      > /etc/apt/sources.list.d/kitware.list; \
    echo "deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ noble-rc main" \
      >> /etc/apt/sources.list.d/kitware.list; \
    apt-get update; \
    rm /usr/share/keyrings/kitware-archive-keyring.gpg; \
    apt-get install -y --no-install-recommends kitware-archive-keyring

RUN apt-get install -y --no-install-recommends \
    build-essential \
    net-tools \
    cmake \
    gdb \
    git \
    vim \
    wget \
    curl \
    rpm \
    file \
    autotools-dev \
    automake \
    libc6-dbg \
    ca-certificates \
    openssh-client \
    alsa-utils \
    pulseaudio \
    pulseaudio-utils \
    librtmidi-dev \
    librtaudio-dev \
    portaudio19-dev \
    libgtest-dev \
    libsndfile1-dev \
    python3 \
    python3-dev \
    python3-pip \
    python3-venv && \
    rm -rf /var/lib/apt/lists/*

# Configure ALSA to use the null device for audio simulation
RUN printf 'pcm.!default {\n    type null\n}\n' > /root/.asoundrc

# Default command
COPY docker/entrypoint.sh /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]
