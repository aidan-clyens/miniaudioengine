# PulseAudio on WSL2

## Install PulseAudio on Windows

1. Download [PulseAudio](https://www.freedesktop.org/wiki/Software/PulseAudio/Ports/Windows/Support/).
2. Extract ZIP file to `C:\Program Files\pulseaudio`.
3. Configure network settings to allow PulseAudio to forward traffic to WSL2 VM.
4. Open PowerShell as Adminsitrator and navigate to `C:\Program Files\pulseaudio`.
5. Edit `.\etc\pulse\default.pa` and change the following:
    ```
    load-module module-waveout sink_name=output source_name=input record=0
    load-module module-native-protocol-tcp auth-ip-acl=127.0.0.1;192.168.1.0/24 auth-anonymous=1
    load-module module-esound-protocol-tcp port=4714 auth-ip-acl=127.0.0.1;192.168.1.0/24 auth-anonymous=1
    ```

7. Edit `.\etc\pulse\daemon.conf` and change the following:
    ```
    exit-idle-time = -1
    ```
8. Run the PulseAudio executable:
    ```
    .\bin\pulseaudio.exe
    ```

### Run as Windows Service

1. Install [NSSM](https://woshub.com/create-service-windows/) (using `winget`):
    ```
    winget install --id NSSM.NSSM -e
    ```

2. Create a service from any executable:
    ```
    nssm install pulseaudio "C:\Program Files\pulseaudio\bin\PulseAudio.exe"
    ```

3. To edit the service:
    ```
    nssm edit pulseaudio
    ```

4. Open PowerShell as an Administrator, and start the service:
    ```
    nssm start pulseaudio
    ```

## Install PulseAudio on WSL2

1. Install PulseAudio on Linux:
    ```
    sudo apt-get update; sudo apt-get install pulseaudio pulseaudio-utils
    ```

2. Open PowerShell and get the host ip address using `ipconfig`.

      **e.g.** `192.168.56.1`

3. Edit `/etc/pulse/client.conf` and change the default server address to the Windows host:
    ```
    default-server = tcp:192.168.56.1
    ```

4. Test audio output:
    ```
    paplay test.wav
    ```

### PulseAudio with Docker

1. PulseAudio socket is found on WSL in `/mnt/wslg/`.

2. Run Docker container with the PulseAudio mount attached:
    ```
    docker create -it --init --platform="${PLATFORM}" \
        --name "${CONTAINER_NAME}" \
        -e "PULSE_SERVER=/mnt/wslg/PulseServer" \
        -v /mnt/wslg/:/mnt/wslg/ \
        -w "${CONTAINER_HOME}/miniaudioengine" \
        "${IMAGE}" \
        tail -f /dev/null
    ```
3. Or, run the `docker/linux/docker-container-start.sh` script.

4. In the Docker container, test audio output:
    ```
    paplay test.wav
    ```
