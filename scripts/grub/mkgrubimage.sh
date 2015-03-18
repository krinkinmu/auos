#!/bin/bash

USER=`whoami`
IMAGE=image
KERNEL=

for arg in "$@"; do
	case $arg in
	--kernel=*)
		KERNEL="${arg#--kernel=}"
	;;
	--image=*)
		IMAGE="${arg#--image=}"
	;;
	--help)
		echo "$0 --kernel=<input> [--image=<output>]"
		echo "where:"
		echo -e "\t<input> - kernel image file name"
		echo -e "\t<output> - output image file name"
		exit 0
	;;
	*)
		echo "unknown argument $arg"
		exit 1
	;;
	esac
done

if [ ! -f "$KERNEL" ]; then
	echo "kernel image file (\'$KERNEL\') doesn't exists"
	exit 1
fi

if [ -z "$IMAGE" ]; then
	echo "output image filename must not be empty"
	exit 1
fi

dd if=/dev/zero of=$IMAGE bs=1M count=64
echo -e "n\np\n1\n\n\na\n1\nw" | fdisk $IMAGE >/dev/null 2>/dev/null

PARTITION=`sudo losetup -f`
sudo losetup $PARTITION $IMAGE

FILESYSTEM=`sudo losetup -f`
sudo losetup $FILESYSTEM $IMAGE -o 1048576
sudo mke2fs $FILESYSTEM

TMPDIR=`mktemp -d`
sudo mount -t ext2 $FILESYSTEM $TMPDIR
sudo chown $USER:$USER $TMPDIR
mkdir -p $TMPDIR/boot/grub
cp $KERNEL $TMPDIR/boot/kernel
cp grub.cfg $TMPDIR/boot/grub/grub.cfg
sudo grub-install --no-floppy --boot-directory=$TMPDIR/boot --modules="normal part_msdos ext2" $PARTITION
sync
sudo umount $TMPDIR
rm -r $TMPDIR

sudo losetup -d $FILESYSTEM
sudo losetup -d $PARTITION
