# rpi4-64-yocto image

Based on: Kirkstone 
Login with: root

Added Features:

- systemd
- serial port with 115200 Bd 8/1/1 supported (see raspi pinout and use a serial usb converter)
- ssh access
- opencv4.5
- tensorflow light 2.8
- clang
- cmake
- bash
- htop
- mc (midnight commander)
- nano
- qtbase
- gstreamer

Static IP in meta-cfgip
- make changes in meta-cfgip/recipes-core/systemd-conf/files/
here: 
[Match]
Name=eth0
KernelCommandLine=!nfsroot

[Network]
Address=193.96.192.45/27
DNS=208.67.222.222 77.88.8.88
(to be changed)


