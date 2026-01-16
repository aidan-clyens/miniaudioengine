@echo off
REM Run the Docker container in detached mode
REM Mounts the current workspace and SSH keys

docker run -dit --init --platform=linux/amd64 ^
    --name miniaudioengine ^
    -v "%CD%:/workspace/miniaudioengine" ^
    -w "/workspace/miniaudioengine" ^
    miniaudioengine:latest ^
    tail -f /dev/null
