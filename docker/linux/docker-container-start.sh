#!/usr/bin/env bash

set -euo pipefail

readonly IMAGE="ghcr.io/aidan-clyens/miniaudioengine-dev:latest@sha256:6c31c9e7caca93e93f54213f3ab22474f82a0b8f42350e4f5c55b8ac0de74b4e"
readonly CONTAINER_NAME="miniaudioengine"
readonly SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
readonly REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
readonly CONTAINER_HOME="/home/$(id -un)"

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
