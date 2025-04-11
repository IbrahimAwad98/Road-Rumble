ifeq ($(OS),Windows_NT)
    include Makefile.win
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        include Makefile.mac
    else
        $(error Unsupported OS)
    endif
endif
