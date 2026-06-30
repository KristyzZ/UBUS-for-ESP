.PHONY: default_target src lib

default_target: lib src clean

lib:
	$(MAKE) -C lib

src:
	$(MAKE) -C src

libserialport:
	$(MAKE) -C libserialport
 
clean:
	
