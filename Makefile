all:
	mkdir -p ./bin
	$(MAKE) -C server
	$(MAKE) -C client
