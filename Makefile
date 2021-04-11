G_BIN_DIR := $(shell realpath ./bin)
G_OBJ_DIR := $(shell realpath ./obj)

export G_BIN_DIR
export G_OBJ_DIR

all: build_protobuf build_server build_client
	

build_protobuf:
	$(MAKE) -C protobuf

build_server:
	$(MAKE) -C server

build_client:
	$(MAKE) -C client


.PHONY: clean

clean:
	$(MAKE) -C protobuf clean
	$(MAKE) -C server clean
	$(MAKE) -C client clean

	rm -rf $(G_OBJ_DIR)