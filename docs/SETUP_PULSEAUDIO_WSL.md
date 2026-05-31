# PulseAudio on WSL2

## Install PulseAudio on Windows

1. Download [PulseAudio](https://www.freedesktop.org/wiki/Software/PulseAudio/Ports/Windows/Support/).
2. Extract ZIP file to `C:\Program Files\pulseaudio`.
3. Configure network settings to allow PulseAudio to forward traffic to WSL2 VM.
4. Open PowerShell as Adminsitrator and navigate to `C:\Program Files\pulseaudio`.
5. Edit `.\etc\pulse\default.pa` and change the following:
  `load-module module-waveout sink_name=output source_name=input record=0`
  `load-module module-native-protocol-tcp auth-ip-acl=127.0.0.1;192.168.1.0/24 auth-anonymous=1`
  `load-module module-esound-protocol-tcp port=4714 auth-ip-acl=127.0.0.1;192.168.1.0/24 auth-anonymous=1`
7. Edit `.\etc\pulse\daemon.conf` and change the following: 
  `exit-idle-time = -1`
8. Run the PulseAudio executable:
  `.\bin\pulseaudio.exe`
9. TODO - Run PulseAudio as a Windows service automatically.

## Install PulseAudio on WSL2

1. Install PulseAudio on Linux:
  `sudo apt-get update; sudo apt-get install pulseaudio pulseaudio-utils`
2. Open PowerShell and get the host ip address using `ipconfig`.
  e.g. `192.168.56.1`
3. Edit `/etc/pulse/client.conf` and change the default server address to the Windows host:
  `default-server = tcp:192.168.56.1`
4. Test audio output:
  `paplay test.wav`
