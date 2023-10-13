eacy: eacy.cpp
	g++ -g eacy.cpp -o eacy

install: eacy 
	cp eacy /usr/local/bin
	mkdir -p /usr/local/include/eacy
	cp eacy.h /usr/local/include/eacy
	mkdir -p /usr/local/share/eacy
	cp eacy.py /usr/local/share/eacy
	
uninstall:
	touch /usr/local/bin/eacy && rm /usr/local/bin/eacy
	touch /usr/local/include/eacy && rm -rf /usr/local/include/eacy
	touch /usr/local/share/eacy && rm -rf /usr/local/share/eacy

