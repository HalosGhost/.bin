#!/usr/bin/env bash

die () { printf "failed to $1\n"; exit 1; }
msg () { printf "$1\n"; }

msg 'Unmounting old_root'
umount -R /old_root || die 'unmount old_root'

msg 'Cleaning up old_root'
rm -rf /old_root || die 'clean up old_root'

msg 'Killing LVM'
vgremove -ff centos || die 'kill LVM'

msg 'Wiping partitions'
dd bs=512 count=1 if=/dev/zero of=/dev/sda || die 'wipe /dev/sda'

def_package_list=(
   'bash' 'bzip2' 'coreutils' 'device-mapper' 'diffutils' 'e2fsprogs' 'file'
   'filesystem' 'findutils' 'gawk' 'gcc-libs' 'gettext' 'glibc' 'grep' 'gzip'
   'inetutils' 'iproute2' 'iputils' 'less' 'licenses' 'logrotate' 'man-db'
   'man-pages' 'pacman' 'pciutils' 'perl' 'procps-ng' 'psmisc' 'sed' 'shadow'
   'sysfsutils' 'systemd-sysvcompat' 'tar' 'texinfo' 'usbutils' 'util-linux'
   'which' 'sudo' 'nftables' 'vim' 'syslinux' 'linux-grsec' 'paxd' 'gradm' 'zsh'
   'zsh-syntax-highlighting' 'openssh'
)
