#!/usr/bin/env bash

msg () { printf "%s\n" "$1"; }

# This should only be used for scripts with simple or no control flow
trap_err() { ret=$?; printf '%s\n' "${BASH_SOURCE[0]}:$LINENO: $BASH_COMMAND ($ret)"; exit $ret; }
trap 'trap_err' ERR

msg 'Unmounting old_root'
umount -R /old_root

msg 'Cleaning up old_root'
rm -rf /old_root

msg 'Killing LVM'
vgremove -ff centos

[[ "$1" != '--halosghost-unofficial-install' ]] && exit 0;
[[ -z "$2" ]] && exit 1;

msg 'By continuing, you are not installing Arch, and your install is'
msg 'not officially supported. You have been warned. Enter YES if you'
msg 'wish to continue;'

read -r
[[ "$REPLY" != 'YES' ]] && exit 1

msg 'Setting DNS to use openNIC'
echo 'nameserver 52.175.214.157' > /etc/resolv.conf

msg 'Initializing the pacman keyring'
systemctl start haveged
pacman-key --init
pacman-key --populate archlinux

msg 'Partitioning disk'
cfdisk /dev/sda

msg 'Formatting root'
mkfs.ext4 -F /dev/sda1

msg 'Formatting boot'
mkfs.ext4 -O^64bit -F /dev/sda2

msg 'Mounting root'
mount /dev/sda1 /mnt

msg 'Create boot mountpoint'
mkdir -p /mnt/boot

msg 'Mounting boot'
mount /dev/sda2 /mnt/boot

def_package_list=(
   'bash' 'bzip2' 'coreutils' 'device-mapper' 'diffutils' 'e2fsprogs' 'file'
   'filesystem' 'findutils' 'gawk' 'gcc-libs' 'gettext' 'glibc' 'grep' 'gzip'
   'inetutils' 'iproute2' 'iputils' 'less' 'licenses' 'logrotate' 'man-db'
   'man-pages' 'pacman' 'pciutils' 'perl' 'procps-ng' 'psmisc' 'sed' 'shadow'
   'sysfsutils' 'systemd-sysvcompat' 'tar' 'texinfo' 'usbutils' 'util-linux'
   'which' 'sudo' 'nftables' 'vim' 'syslinux' 'linux-grsec' 'paxd' 'gradm' 'zsh'
   'zsh-syntax-highlighting' 'openssh' 'archlinux-keyring'
)

msg 'Pacstrapping'
pacstrap /mnt "${def_package_list[@]}"

msg 'Generating fstab'
genfstab -U /mnt >> /mnt/etc/fstab

msg 'Generating Network Configuration'
printf '[Match]\nName=ens33\n\n[Address]\n%s\n\n[Route]\n%s\n' \
    "$(ip a show dev ens33 | awk '/inet / { print "Address=" $2 "\nBroadcast=" $4 }')" \
    "$(ip r show dev ens33 | awk 'NR == 1 { print "Gateway=" $3 }')" > /mnt/etc/systemd/network/wired.network

msg 'Grabbing third stage script'
curl 'https://raw.githubusercontent.com/HalosGhost/.bin/master/cac/stage_3.sh' \
    -fs#o /mnt/root/stage_3.sh

msg 'Changing permissions of third stage script'
chmod -c 0755 /mnt/root/stage_3.sh

msg 'Chrooting and running third stage'
arch-chroot /mnt /root/stage_3.sh "$2"
