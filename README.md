# LED Matrix 

The system is an LED matrix used to display multiplayer pong across two Beaglebones  
  
## Getting Started
  
Clone the repository. 

If you want to build & run the game with the driver:  
```
$ make pong driver  
$ make pong user
``` 
Go into UBOOT and load custom kernel with script (must change IP address to your own system's):
```
setenv ipaddr 192.168.2.2;setenv serverip 192.168.2.1;setenv tftproot /home/scott/cmpt433/public/;setenv bootfile ${tftproot}4.4.95-swplumme-bone19.zImage;setenv fdtfile ${tftproot}am335x-bonegreen.dtb;tftp ${loadaddr} ${bootfile};tftp ${fdtaddr} ${fdtfile};setenv bootargs console=ttyO0,115200n8 root=/dev/mmcblk1p1 ro rootfstype=ext4 rootwait;bootz ${loadaddr} - ${fdtaddr}
```
Navigate to your /cmpt433/public/myApps/ folder
```
#cd ~/cmpt433/public/myApps
# ./pong 1 
```

If you want to build and run the game without the driver:  
```
$ cd pong  
$ make
``` 
```
# cd ~/cmpt433/public/myApps
# ./pong 1 
```
  
Included is the scripts to autoload the game by using systemmd under the /scripts folder which is "start.sh" and
the other is "matrix-pong.service" which configures the system to launch "start.sh"



