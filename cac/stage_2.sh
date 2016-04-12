#!/usr/bin/env bash

msg () { printf "$1\n"; }
die () { msg "failed to $1"; exit 1; }

msg 'Unmounting old_root'
umount -R /old_root || die 'unmount old_root'

msg 'Cleaning up old_root'
rm -rf /old_root || die 'clean up old_root'

msg 'Killing LVM'
vgremove -ff centos || die 'kill LVM'

[[ "$2" != '--halosghost-unofficial-install' ]] && exit 0;

msg 'By continuing, you are not installing Arch, and your install is'
msg 'not officially supported. You have been warned. Enter YES if you'
msg 'wish to continue;'

read
[[ "$REPLY" != 'YES' ]] && exit 1

msg 'Setting DNS to use google'
echo 'nameserver 8.8.8.8' > /etc/resolv.conf || die 'set DNS'

msg 'Initializing the pacman keyring'
systemctl start haveged || msg 'start haveged'
pacman-key --init || msg 'initialize the keyring'
pacman-key --populate archlinux || msg 'populate the keyring'

read -r -d '' partition_scheme << 'EOF'
label: dos
label-id: 0x350346e6
device: /dev/sda
unit: sectors

/dev/sda1 : start=      2048, size=     20969472, type=83, bootable
EOF

msg 'Partitioning disk'
sfdisk /dev/sda <<< "$partition_scheme" || die 'partition disk'

def_package_list=(
   'bash' 'bzip2' 'coreutils' 'device-mapper' 'diffutils' 'e2fsprogs' 'file'
   'filesystem' 'findutils' 'gawk' 'gcc-libs' 'gettext' 'glibc' 'grep' 'gzip'
   'inetutils' 'iproute2' 'iputils' 'less' 'licenses' 'logrotate' 'man-db'
   'man-pages' 'pacman' 'pciutils' 'perl' 'procps-ng' 'psmisc' 'sed' 'shadow'
   'sysfsutils' 'systemd-sysvcompat' 'tar' 'texinfo' 'usbutils' 'util-linux'
   'which' 'sudo' 'nftables' 'vim' 'syslinux' 'linux-grsec' 'paxd' 'gradm' 'zsh'
   'zsh-syntax-highlighting' 'openssh'
)

msg 'Pacstrapping'
pacstrap /mnt "${def_package_list[@]}" || die 'pacstrap'
