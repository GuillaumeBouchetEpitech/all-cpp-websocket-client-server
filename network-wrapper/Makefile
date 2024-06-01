
build_platform=native
# build_platform=web-wasm

build_mode=release
# build_mode=debug

#

ifeq ($(build_platform),native)
# $(info build_platform is valid, value=$(build_platform))
else ifeq ($(build_platform),web-wasm)
# $(info build_platform is valid, value=$(build_platform))
else
$(error unsupported value for "build_platform", value=$(build_platform))
endif

ifeq ($(build_mode),release)
# $(info build_mode is valid, value=$(build_mode))
else ifeq ($(build_mode),debug)
# $(info build_mode is valid, value=$(build_mode))
else
$(error unsupported value for "build_mode", value=$(build_mode))
endif

LOG_INFO= '[${build_mode}] [${build_platform}]'

#

ifeq ($(build_platform),native)

DIR_LIB=		./lib/native
NAME_LIB=		$(DIR_LIB)/network-wrapper.a

else ifeq ($(build_platform),web-wasm)

DIR_LIB=		./lib/wasm
NAME_LIB=		$(DIR_LIB)/network-wrapper.bc

endif

# DIR_LIB=		./lib

# NAME_LIB=	$(DIR_LIB)/network-wrapper.a

#### DIRS

DIR_SRC=	src/network-wrapper

ifeq ($(build_platform),native)

DIR_OBJ=	./obj/native

else ifeq ($(build_platform),web-wasm)

DIR_OBJ=	./obj/web-wasm

endif

#### /DIRS

#### SRC

ifeq ($(build_platform),native)

ALL_SRC+=	\
	$(wildcard \
		$(DIR_SRC)/common/*.cpp \
		$(DIR_SRC)/common/internals/*.cpp \
		$(DIR_SRC)/common/internals/*.cpp \
		$(DIR_SRC)/utilities/*.cpp \
		$(DIR_SRC)/http-server/*.cpp \
		$(DIR_SRC)/http-server/internals/*.cpp \
		$(DIR_SRC)/websocket-client/*.cpp \
		$(DIR_SRC)/websocket-client/native/*.cpp \
		$(DIR_SRC)/websocket-client/native/internals/*.cpp \
		$(DIR_SRC)/websocket-client/native/internals/worker-thread/*.cpp \
		$(DIR_SRC)/websocket-client/native/internals/worker-thread/internals/*.cpp \
		$(DIR_SRC)/websocket-server/*.cpp \
		$(DIR_SRC)/websocket-server/internals/*.cpp \
	)

else ifeq ($(build_platform),web-wasm)

ALL_SRC+=	\
	$(wildcard \
		$(DIR_SRC)/websocket-client/*.cpp \
		$(DIR_SRC)/websocket-client/wasm/*.cpp \
		$(DIR_SRC)/websocket-client/wasm/internals/*.cpp \
	)

endif

#

ALL_OBJ=	$(patsubst %.cpp, $(DIR_OBJ)/%.o, $(ALL_SRC))

#

#

#### /BULLET_SRC



#######


ifeq ($(build_mode),release)

# optimisation flag
BUILD_FLAG=		-O3

else

# debug flag
BUILD_FLAG=		-g3

endif


ifeq ($(build_platform),native)

CXX=g++
AR=ar

CXXFLAGS +=	$(BUILD_FLAG)
CXXFLAGS +=	-std=c++20
CXXFLAGS += -Wall -W -Wextra -Wunused -Wpedantic -Wshadow -Wconversion -Werror
CXXFLAGS +=	-I$(DIR_SRC)

LDFLAGS += $(BUILD_FLAG)
LDFLAGS += $(NAME_LIB)
LDFLAGS += -pthread

# LDFLAGS += -lssl
# LDFLAGS += -lcrypto
LDFLAGS += -lz # libzlib
# LDFLAGS += -lboost_system # libboost-system1.67-dev

else ifeq ($(build_platform),web-wasm)

CXX=em++
AR=emar

LDFLAGS+=	$(NAME_NET_WRAPPER)
# LDFLAGS+=	-lwebsocket.js
# LDFLAGS+=	-s TOTAL_MEMORY=128Mb
LDFLAGS+=	-s WASM=1
LDFLAGS+= -s BINARYEN_IGNORE_IMPLICIT_TRAPS=1
# LDFLAGS+= -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0
# LDFLAGS+= -s EXPORTED_FUNCTIONS="['_free']"
# LDFLAGS+= -s EXPORTED_RUNTIME_METHODS="['cwrap','stringToUTF8','lengthBytesUTF8','UTF8ToString','allocateUTF8']"

# LDFLAGS+= -s EXPORT_NAME="clientWeb"
# LDFLAGS+= -s MODULARIZE=1

ifeq ($(build_mode),debug)

LDFLAGS+=	-s DEMANGLE_SUPPORT=1
LDFLAGS+=	-s GL_ASSERTIONS=1
LDFLAGS+=	-s GL_DEBUG=1
LDFLAGS+=	-s DISABLE_EXCEPTION_CATCHING=2

endif

endif


RM=			rm -rf


#######

#
## RULE(S)

all:	networkWrapper

ensureFolders:
	@mkdir -p $(DIR_LIB)

networkWrapper:	ensureFolders $(ALL_OBJ)
	@echo ' ---> building $(LOG_INFO): "network-wrapper library"'
	@$(AR) cr $(NAME_LIB) $(ALL_OBJ)
	@echo '   --> built $(LOG_INFO): "network-wrapper library"'

#

# for every ".cpp" file
# => ensure the "obj" folder(s)
# => compile in a ".o" file

$(DIR_OBJ)/%.o: %.cpp
	@mkdir -p `dirname $@`
	@echo ' --> processing $(LOG_INFO):' $<
	@$(CXX) -c $(CXXFLAGS) -MMD -MT "$@" -MF "$@.dep" -o "$@" $<

include $(shell test -d $(DIR_OBJ) && find $(DIR_OBJ) -name "*.dep")

#

clean:
	$(RM) $(DIR_OBJ)

fclean:	clean
	$(RM) $(NAME_LIB)

re:	fclean all

.PHONY:	\
	all \
	ensureFolders \
	networkWrapper \
	clean \
	fclean \
	re

## RULE(S)
#
