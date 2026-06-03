# Install prerequisites and Kitware keyring
set -eux;
apt-get update;
apt-get install -y --no-install-recommends ca-certificates gpg wget;
# Fetch and install Kitware keyring
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc \
  | gpg --dearmor - > /usr/share/keyrings/kitware-archive-keyring.gpg;
# Add Kitware repository
echo "deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ noble main" \
  > /etc/apt/sources.list.d/kitware.list;
echo "deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ noble-rc main" \
  >> /etc/apt/sources.list.d/kitware.list;
apt-get update;
rm /usr/share/keyrings/kitware-archive-keyring.gpg;
apt-get install -y --no-install-recommends kitware-archive-keyring;

apt-get install -y --no-install-recommends \
  build-essential \
  net-tools \
  cmake \
  gdb \
  git \
  vim \
  wget \
  curl \
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
  rm -rf /var/lib/apt/lists/*;
