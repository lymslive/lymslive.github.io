.PHONY: help all build start stop restart log

help:
all: build restart log

SERVER_NAME=my-program

############################################################
## 基本管理目标

build:
	@mkdir -p build
	cd build
	cmake ..
	make
	make install

start.bash:
	script/start.sh

stop.bash:
	- script/stop.sh

restart: stop start

############################################################
## 使用 systemd 管理服务

SYSTEMD_UNIT=/etc/systemd/system/$(SERVER_NAME).service
$(SYSTEMD_UNIT): script/systemd-service-generator.sh script/systemd-template.service
	script/systemd-service-generator.sh script/systemd-template.service
	sudo mv $(SERVER_NAME).service $(SYSTEMD_UNIT)
	sudo systemctl daemon-reload

start.sd: $(SYSTEMD_UNIT) 
	sudo systemctl start $(SERVER_NAME)

stop.sd: $(SYSTEMD_UNIT) 
	sudo systemctl stop $(SERVER_NAME)

status.sd: $(SYSTEMD_UNIT) 
	systemctl status $(SERVER_NAME)

restart.sd: $(SYSTEMD_UNIT) 
	sudo systemctl restart $(SERVER_NAME)

############################################################
## release 打包

include tar.mk
# tar:
#	make -f tar.mk

############################################################
## 容器化目标管理

TOUCH_DIR = .touch
$(TOUCH_DIR):
	mkdir -p $@

# 制作镜像
IMAGE_TAG = $(SERVER_NAME):latest
image_touch = $(TOUCH_DIR)/image.touch
image: $(image_touch)
$(image_touch): $(TAR_DIR)
	docker build $(TAR_DIR) -t $(IMAGE_TAG) -f ./Dockerfile
	@mkdir -p $(TOUCH_DIR)
	touch $@

# 运行容器
CONTAINER_NAME = --name $(SERVER_NAME)
CONTAINER_VOL = -v log:/workspace/log
CONTAINER_CMD = -d bin/$(SERVER_NAME)
start.docker:
	docker run $(CONTAINER_VOL) $(CONTAINER_NAME) $(IMAGE_TAG) $(CONTAINER_CMD)

# 停止容器
stop.docker:
	docker stop $(CONTAINER_NAME)

# 停止容器
DOCKER_REGISTRY = my.company.com/my-project
CUR_TIME = $(shell date +%s)
IMAGE_REMOTE = $(DOCKER_REGISTRY)/$(IMAGE_TAG)
IMAGE_VERSION = $(DOCKER_REGISTRY)/$(SERVER_NAME):$(CUR_TIME)
push_touch = $(TOUCH_DIR)/push.touch
push.docker: $(push_touch)
$(push_touch): $(image_touch)
	docker tag $(IMAGE_TAG) $(IMAGE_REMOTE)
	docker push $(IMAGE_REMOTE)
	docker tag $(IMAGE_TAG) $(IMAGE_VERSION)
	docker push $(IMAGE_VERSION)
	touch $@

# 默认的 start stop 方式
start: start.docker
stop: stop.docker

############################################################
## 日志目标管理

log:
	script/lastlog.sh

log.rm1:
	script/rmlog.sh 1
log.rm7:
	script/rmlog.sh 7
log.rm30:
	script/rmlog.sh 30

############################################################
## 帮助信息

help:
	@echo make build: 编译项目
	@echo make start: 启动服务
	@echo make stop:  停止服务
	@echo make restart: 重启服务
	@echo make log: 查看最近的日志
	@echo make all: 一键编译、重启、查看日志
	@echo make help: 打印这些帮助信息
