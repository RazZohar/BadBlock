make odroidc_defconfig
make -j4
make -j4 modules
make uImage

sudo mkdir -p mount

#First mount the 1st partion in the img file(offset should be SIZE(512) * start) 
sudo mount -o loop,offset=1572864 BadBlock.img ./mount/
sudo cp arch/arm/boot/uImage arch/arm/boot/dts/meson8b_odroidc.dtb ./mount && sync && sudo umount ./mount

#MOunt the 2nd partion
sudo mount -o loop,offset=136314880 BadBlock.img ./mount

#creat temp dir to install the module
sudo make modules_install ARCH=arm INSTALL_MOD_PATH=~/Temp 

#remove the old modules&firmware
cd ./mount/lib/
sudo rm -r firmware/ modules/

#copy the new firamware and modules
sudo cp -r ~/Temp/lib/firmware ./firmware
sudo cp -r ~/Temp/lib/modules ./modules

cd ../
sudo mkdir -p BadBlock
#sync and unmount
cd ../../ && sync && sudo umount ./mount
