CC=g++         
CCFLAGS= -std=c++11 -Wall -g -fsanitize=address -fno-omit-frame-pointer


clusterer: driver.o clusterer.o
	$(CC) $(CCFLAGS) driver.o clusterer.o -o clusterer

driver.o: driver.cpp
	$(CC) $(CCFLAGS) -c driver.cpp	

clusterer.o: clusterer.cpp clusterer.h
	$(CC) $(CCFLAGS) -c clusterer.cpp


clean:
	@rm -f *.o && rm -f clusterer
   
run:
	@./clusterer ./Gradient_Numbers_PPMS -o output.txt -k 10 -b 1

