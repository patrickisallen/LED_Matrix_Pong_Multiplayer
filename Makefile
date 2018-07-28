all:
	make --directory=pong
	make --directory=driver

clean:
	make clean --directory=pong
	make clean --directory=driver
