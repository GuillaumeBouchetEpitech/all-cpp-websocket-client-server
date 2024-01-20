
build_mode=release
# build_mode=debug

#

ifeq ($(build_mode),release)
# $(info build_mode is valid, value=$(build_mode))
else ifeq ($(build_mode),debug)
# $(info build_mode is valid, value=$(build_mode))
else
$(error unsupported value for "build_mode", value=$(build_mode))
endif

LOG_INFO= '[$(build_mode)]'

#

DIR_APP=		./bin
DIR_NET_WRAPPER=		./lib

NAME_APP=	$(DIR_APP)/exec
NAME_NET_WRAPPER=	$(DIR_NET_WRAPPER)/network-wrapper.a

#### DIRS

DIR_SRC=	src
DIR_APP_SRC=	$(DIR_SRC)/application
DIR_NET_WRAPPER_SRC=	$(DIR_SRC)/network-wrapper

#### /DIRS

DIR_OBJ=	./obj/native
DIR_APP_OBJ=	$(DIR_OBJ)/application
DIR_NET_WRAPPER_OBJ=	$(DIR_OBJ)/network-wrapper


#### SRC

APP_SRC+=	\
	$(wildcard \
		$(DIR_APP_SRC)/*.cpp \
		$(DIR_APP_SRC)/https-server/*.cpp \
		$(DIR_APP_SRC)/https-server/file-manager/*.cpp \
		$(DIR_APP_SRC)/websocket-server/*.cpp \
		$(DIR_APP_SRC)/websocket-server/session-manager/*.cpp \
	)

NET_WRAPPER_SRC+=	\
	$(wildcard \
		$(DIR_NET_WRAPPER_SRC)/http-server/*.cpp \
		$(DIR_NET_WRAPPER_SRC)/http-server/internals/*.cpp \
		$(DIR_NET_WRAPPER_SRC)/websocket-server/*.cpp \
		$(DIR_NET_WRAPPER_SRC)/websocket-server/internals/*.cpp \
	)

#

APP_OBJ=	$(patsubst %.cpp, $(DIR_APP_OBJ)/%.o, $(APP_SRC))
NET_WRAPPER_OBJ=	$(patsubst %.cpp, $(DIR_NET_WRAPPER_OBJ)/%.o, $(NET_WRAPPER_SRC))

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


CXX=g++
AR=ar

CXXFLAGS +=	$(BUILD_FLAG)
CXXFLAGS +=	-std=c++17
CXXFLAGS += -Wall -W -Wextra -Wunused -Wpedantic -Wshadow -Wconversion -Werror
CXXFLAGS +=	-I$(DIR_SRC)

LDFLAGS += $(BUILD_FLAG)
LDFLAGS += $(NAME_NET_WRAPPER)
LDFLAGS += -pthread

# LDFLAGS += -lssl
# LDFLAGS += -lcrypto
LDFLAGS += -lz # libzlib
# LDFLAGS += -lboost_system # libboost-system1.67-dev


RM=			rm -rf


#######

#
## RULE(S)

all:	application

ensureFolders:
	@mkdir -p $(DIR_APP)
	@mkdir -p $(DIR_NET_WRAPPER)

application:	ensureFolders networkWrapper $(APP_OBJ)
	@echo ' ---> building $(LOG_INFO) "application"'
	@mkdir -p `dirname $(NAME_APP)`
	@$(CXX) $(CXXFLAGS) $(APP_OBJ) -o $(NAME_APP) $(LDFLAGS)
	@echo '   --> built $(LOG_INFO) "application"'

networkWrapper:	ensureFolders $(NET_WRAPPER_OBJ)
	@echo ' ---> building $(LOG_INFO): "network-wrapper library"'
	@$(AR) cr $(NAME_NET_WRAPPER) $(NET_WRAPPER_OBJ)
	@echo '   --> built $(LOG_INFO): "network-wrapper library"'

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

$(DIR_NET_WRAPPER_OBJ)/%.o: %.cpp
	@mkdir -p `dirname $@`
	@echo ' --> processing $(LOG_INFO):' $<
	@$(CXX) -c $(CXXFLAGS) -MMD -MT "$@" -MF "$@.dep" -o "$@" $<

include $(shell test -d $(DIR_NET_WRAPPER_OBJ) && find $(DIR_NET_WRAPPER_OBJ) -name "*.dep")

#

clean:
	$(RM) $(DIR_APP_OBJ)
	$(RM) $(DIR_APP_OBJ)

fclean:	clean
	$(RM) $(NAME_APP)
	$(RM) $(NAME_NET_WRAPPER)

re:	fclean all

.PHONY:	\
	all \
	ensureFolders \
	application \
	networkWrapper \
	clean \
	fclean \
	re

## RULE(S)
#
