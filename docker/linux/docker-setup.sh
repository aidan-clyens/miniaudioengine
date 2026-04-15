# Install QEMU for cross-architecture builds
apt-get update
apt-get install qemu-user-static

# Register QEMU with Docker
docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
