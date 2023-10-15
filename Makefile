eacy: eacy.cpp
	g++ -g eacy.cpp -o eacy

libeacyload.so: load.cpp
	g++ -g -shared -fPIC load.cpp -o libeacyload.so

all: eacy libeacyload.so

install: eacy 
	cp eacy /usr/local/bin
	mkdir -p /usr/local/include/eacy
	cp eacy.h load.h /usr/local/include/eacy
	mkdir -p /usr/local/share/eacy
	cp eacy.py /usr/local/share/eacy
	mkdir -p /usr/local/lib
	cp libeacyload.so /usr/local/lib
	
uninstall:
	touch /usr/local/bin/eacy && rm /usr/local/bin/eacy
	touch /usr/local/include/eacy && rm -rf /usr/local/include/eacy
	touch /usr/local/share/eacy && rm -rf /usr/local/share/eacy
	touch /usr/local/share/libeacyload.so && rm /usr/local/share/libeacyload.so


