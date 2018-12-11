CC=c++
FLAGS= -w -std=gnu++11

LDFLAGS = -L/usr/local/ssl/lib
LDLIBS = -lssl -lcrypto -lxml2 `xml2-config --cflags` `xml2-config --libs`

FILES = Makefile Readme manual.pdf feedreader.cpp xmlmod.cpp xmlmod.h sslmod.cpp sslmod.h

all: clean sslmod.o xmlmod.o feedreader.o
	$(CC) $(FLAGS) sslmod.o xmlmod.o feedreader.o -o feedreader $(LDFLAGS) $(LDLIBS)

sslmod.o: 
	$(CC) $(FLAGS) -c sslmod.cpp -o sslmod.o  -L/usr/local/ssl/lib -lssl -lcrypto -lxml2 `xml2-config --cflags` `xml2-config --libs`

xmlmod.o:
	 $(CC) $(FLAGS) -c xmlmod.cpp -o xmlmod.o  -L/usr/local/ssl/lib -lssl -lcrypto -lxml2 `xml2-config --cflags` `xml2-config --libs`

feedreader.o:
	 $(CC) $(FLAGS) -c feedreader.cpp -o feedreader.o  -L/usr/local/ssl/lib -lssl -lcrypto -lxml2 `xml2-config --cflags` `xml2-config --libs`


#%.o : %.cpp
#	$(CC) $(FLAGS) -c $< -o $@ -L/usr/local/ssl/lib -lssl -lcrypto -lxml2 `xml2-config --cflags` `xml2-config --libs`

clean:
	rm -f *.o feedreader

tar: clean
	tar -cf xkucer91.tar $(FILES)

rmtar:
	rm -f xkucer91.tar
