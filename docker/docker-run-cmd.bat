@echo off
REM Run a one-off command in a new container instance
REM Container is automatically removed after execution

docker run -it --rm --init --platform=linux/amd64 ^
    --name miniaudioengine ^
    -v "%CD%:/workspace/miniaudioengine" ^
    -w "/workspace/miniaudioengine" ^
    miniaudioengine:latest %*
