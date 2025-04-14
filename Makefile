# om det windows välje .win
ifeq ($(OS),Windows_NT)
    include Makefile.win
# annars välj .mac
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        include Makefile.mac
# ej Linux eller andra OS
    else
        $(error Unsupported OS)
    endif
endif
