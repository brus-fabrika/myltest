all: 
	mkdir ./bin
	$(MAKE) -C server
	$(MAKE) -C client
