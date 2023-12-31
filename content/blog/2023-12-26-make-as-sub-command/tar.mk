.PHONY: tar

SERVER_NAME ?= my-program
TAR_DIR=$(SERVER_NAME)-release
TAR_NAME=$(SERVER_NAME)-release.tar.gz

tar: $(TAR_NAME)
$(TAR_NAME): $(TAR_DIR)
	tar czvf $(TAR_NAME) $(TAR_DIR)

FILE_LIST = \
	bin/my-program \
	lib/libmyutil.so \
	conf/my-program.ini \
	makefile

FILE_INTAR = $(patsubst %,$(TAR_DIR)/%,$(FILE_LIST))

$(FILE_INTAR): $(TAR_DIR)/% : %
	cp $< $@

$(TAR_DIR): $(FILE_INTAR)

$(FILE_LIST): 

############################################################
## 测试代码

CUR_TIME = $(shell date +%s)

echo:
	@echo CUR_TIME = $(CUR_TIME)
	@echo TAR_DIR = $(TAR_DIR)
	@echo TAR_NAME = $(TAR_NAME)
	@echo FILE_LIST = $(FILE_LIST)
	@echo FILE_INTAR = $(FILE_INTAR)

$(warning target err should not be called)
$(info target err should not be called)
err:
	$(error this target is deprressed)
