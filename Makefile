# om det windows välje .win
ifeq ($(OS),Windows_NT)
    include Makefile.win
else
    UNAME_S := $(shell uname -s)
# macOS välj .mac
    ifeq ($(UNAME_S),Darwin)
        include Makefile.mac
# Linux välj .linux
    else ifeq ($(UNAME_S),Linux)
        include Makefile.linux
    else
        $(error Unsupported OS)
    endif
endif
