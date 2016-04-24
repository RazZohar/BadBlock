sudo apt-get update -y
sudo apt-get install git -y
sudo apt-get install lib32stdc++6  lib32z1 lzop u-boot-tools -y
sudo apt-get install build-essential gcc -y
sudo apt-get install libncurses5-dev -y

#After you download the tool chain
cd ~/Downloads
sudo mkdir -p /opt/toolchains
sudo tar xvf gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux.tar.xz -C /opt/toolchains/


#after config the .bashrc to the ARM gcc
cd ~
git clone --depth 1 https://github.com/hardkernel/linux.git -b odroidc-3.10.y
cd linux

make odroidc_defconfig
make -j4
make -j4 modules
make uImage

sudo mkdir -p mount

#First mount the 1st partion in the img file(offset should be SIZE(512) * start) 
sudo mount -o loop,offset=1572864 ubuntu-14.04.3lts-lubuntu-odroid-c1-20150811.img ./mount/
sudo cp arch/arm/boot/uImage arch/arm/boot/dts/meson8b_odroidc.dtb ./mount && sync && sudo umount ./mount

#MOunt the 2nd partion
sudo mount -o loop,offset=136314880 ubuntu-14.04.3lts-lubuntu-odroid-c1-20150811.img ./mount
sudo make modules_install ARCH=arm INSTALL_MOD_PATH=./mount && sync && sudo umount ./mount
