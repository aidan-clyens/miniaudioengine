@echo off
REM Pull Docker image from GHCR and create container.

set IMAGE="ghcr.io/aidan-clyens/miniaudioengine-dev:latest@sha256:c475fb9ff5d339834e61510536c0152649e37f49f25bbc3b81c942dc9140f827"
set CONTAINER_NAME="miniaudioengine-x86_64"
set SCRIPT_DIR="docker\windows\"
set REPO_ROOT= ".\"
set CONTAINER_HOME="/home/$(id -un)"

if ! command -v docker >/dev/null 2>&1; then
	echo "docker is required but was not found in PATH." >&2
	exit 1
fi

if docker container inspect "${CONTAINER_NAME}" >/dev/null 2>&1; then
	docker start "${CONTAINER_NAME}" >/dev/null
	echo "Started existing container '${CONTAINER_NAME}'."
	exit 0
fi

docker create -it --init --platform=linux/arm64 \
	--name "${CONTAINER_NAME}" \
	-v "${REPO_ROOT}:${CONTAINER_HOME}/miniaudioengine" \
	-v "${HOME}/.ssh:/root/.ssh" \
	-v "/tmp/.X11-unix:/tmp/.X11-unix" \
	-w "${CONTAINER_HOME}/miniaudioengine" \
	"${IMAGE}" \
	tail -f /dev/null

docker start "${CONTAINER_NAME}"

echo "Created container '${CONTAINER_NAME}' from ${IMAGE}."
