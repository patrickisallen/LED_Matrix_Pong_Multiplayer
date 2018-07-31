# LED Matrix 

The system is an LED matrix used to display multiplayer pong across two Beaglebones  
  
## Getting Started
  
Clone the repository. 
  
If you want to build the driver:  
```make pong driver  
make pong user``` 

If you want to build the game without the driver:  
```cd pong  
make```  
  
Included is the scripts to autoload the game by using systemmd under the /scripts folder which is "start.sh" and
the other is "matrix-pong.service" which configures the system to launch "start.sh"



