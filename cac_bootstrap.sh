#!/usr/bin/env bash

mirror=http://archlinux.surlyjake.com
date=2016.02.01
iso=archlinux-"$date"-dual.iso
arch=x86_64

die () { printf "failed to $1\n"; exit 1; }
msg () { printf "$1\n"; }

msg "Downloading ISO from $mirror"
curl -O "$mirror/archlinux/iso/$date/$iso" || die "download iso"

msg "Mounting ISO to /mnt"
mount -o loop "$iso" /mnt || die "mount iso"

msg "Copying squashfs"
cp -v /mnt/arch/"$arch"/airootfs.* . || die "copy squashfs"

msg "Verifying squashfs"
md5sum -c airootfs.md5 || die "verify squashfs"

msg "Unmounting ISO"
umount /mnt || die "unmount iso"

msg "Delete ISO"
rm -- "$iso" || die "delete ISO"

msg "Extracting Squashfs"
unsquashfs airootfs.sfs || die "extract squashfs"

msg "Clean old airootfs files"
rm -- airootfs.* || die "clean up old airootfs files"

msg "Making new_root target"
mkdir -p new_root || die "create new_root target"

nbytes="$(($(du -bs squashfs-root|cut -f1)+500))"
msg "Creating tmpfs of $nbytes bytes"
mount -o size="$nbytes" -t tmpfs none ./new_root || die "create tmpfs"

msg "Moving live files to new_root"
mv squashfs-root/* ./new_root/ || die "move live files to new_root"
rm -r -- squashfs-root || die "clean up squashfs-root dir"

msg "Create location for old_root"
mkdir -p new_root/old_root || die "create old_root"

msg "Make old_root rprivate"
mount --make-rprivate / || die "make old_root rprivate"

msg "Preemptively modprobe ext4"
modprobe ext4 || die "modprobe ext4"

msg "Ready for pivot_root"
