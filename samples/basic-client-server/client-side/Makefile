
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

DIR_APPLICATION=		./bin
NAME_APPLICATION=		$(DIR_APPLICATION)/exec

else ifeq ($(build_platform),web-wasm)

DIR_APPLICATION=		./dist/wasm
NAME_APPLICATION=		$(DIR_APPLICATION)/index.js

endif

#### DIRS

DIR_SRC=	src/application

DIR_NETOWRK_WRAPPER_LIB=	../../../network-wrapper

#### /DIRS

ifeq ($(build_platform),native)

DIR_OBJ=	./obj/native

else ifeq ($(build_platform),web-wasm)

DIR_OBJ=	./obj/web-wasm

endif
#### SRC

DIR_APP_OBJ=	$(DIR_OBJ)/application


ALL_SRC+=	\
	$(wildcard \
		$(DIR_SRC)/*.cpp \
		$(DIR_SRC)/utilities/*.cpp \
		)

ALL_OBJ=	$(patsubst %.cpp, $(DIR_APP_OBJ)/%.o, $(ALL_SRC))

#

#



#######


ifeq ($(build_mode),release)

BUILD_FLAG=		-O3

else

BUILD_FLAG=		-g3

endif

CXXFLAGS+=	$(BUILD_FLAG)
CXXFLAGS+=	-std=c++20
CXXFLAGS+=	-Wall -W -Wextra -Wunused -Wpedantic -Wshadow -Wconversion -Werror
CXXFLAGS+=	-I$(DIR_SRC)
CXXFLAGS+=	-I$(DIR_NETOWRK_WRAPPER_LIB)/src/

LDFLAGS+=	$(BUILD_FLAG)

ifeq ($(build_platform),native)

# CXX=clang++
CXX=g++
AR=ar

LDFLAGS+=	$(DIR_NETOWRK_WRAPPER_LIB)/lib/native/network-wrapper.a

else ifeq ($(build_platform),web-wasm)

CXX=em++
AR=emar

LDFLAGS+=	$(DIR_NETOWRK_WRAPPER_LIB)/lib/wasm/network-wrapper.a

LDFLAGS+=	-lwebsocket.js
LDFLAGS+=	-s TOTAL_MEMORY=128Mb
LDFLAGS+=	-s WASM=1
LDFLAGS+= -s BINARYEN_IGNORE_IMPLICIT_TRAPS=1
# LDFLAGS+= -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0
LDFLAGS+= -s EXPORTED_FUNCTIONS="['_free']"
# LDFLAGS+= -s EXPORTED_RUNTIME_METHODS="['cwrap','_malloc','stringToUTF8','lengthBytesUTF8','UTF8ToString','allocateUTF8']"
LDFLAGS+= -s EXPORTED_RUNTIME_METHODS="['cwrap','stringToUTF8','lengthBytesUTF8','UTF8ToString','stringToNewUTF8']"

LDFLAGS+= -s EXPORT_NAME="clientWeb"
LDFLAGS+= -s MODULARIZE=1

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

all:	application

ensureFolders:
	@mkdir -p $(DIR_APPLICATION)

application:	ensureFolders $(ALL_OBJ)
	@echo ' ---> building $(LOG_INFO) "application"'
	@mkdir -p `dirname $(NAME_APPLICATION)`
	@$(CXX) $(CXXFLAGS) $(ALL_OBJ) -o $(NAME_APPLICATION) $(LDFLAGS)
	@echo '   --> built $(LOG_INFO) "application"'

#

# for every ".cpp" file
# => ensure the "obj" folder(s)
# => compile in a ".o" file

$(DIR_APP_OBJ)/%.o: %.cpp
	@mkdir -p `dirname $@`
	@echo ' --> processing $(LOG_INFO):' $<
	@$(CXX) -c $(CXXFLAGS) -MMD -MT "$@" -MF "$@.dep" -o "$@" $<

include $(shell test -d $(DIR_APP_OBJ) && find $(DIR_APP_OBJ) -name "*.dep")

#

clean:
	$(RM) $(DIR_APP_OBJ)

fclean:  clean
	$(RM) $(NAME_APPLICATION)

re: fclean all

.PHONY:	\
	all \
	ensureFolders \
	application \
	clean \
	fclean \
	re

## RULE(S)
#
