
# build_mode=release
build_mode=debug

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

NAME_APP=	$(DIR_APP)/exec

#### DIRS

DIR_SRC=	src
DIR_APP_SRC=	$(DIR_SRC)/application
DIR_NETOWRK_WRAPPER_LIB=../../../network-wrapper

#### /DIRS

DIR_OBJ=	./obj/native
DIR_APP_OBJ=	$(DIR_OBJ)/application


#### SRC

APP_SRC+=	\
	$(wildcard \
		$(DIR_APP_SRC)/*.cpp \
		$(DIR_APP_SRC)/https-server/*.cpp \
		$(DIR_APP_SRC)/https-server/file-manager/*.cpp \
		$(DIR_APP_SRC)/websocket-server/*.cpp \
		$(DIR_APP_SRC)/websocket-server/session-manager/*.cpp \
		$(DIR_APP_SRC)/websocket-server/session-manager/internals/*.cpp \
	)

#

APP_OBJ=	$(patsubst %.cpp, $(DIR_APP_OBJ)/%.o, $(APP_SRC))

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
CXXFLAGS +=	-std=c++20
CXXFLAGS += -Wall -W -Wextra -Wunused -Wpedantic -Wshadow -Wconversion -Werror
CXXFLAGS +=	-I$(DIR_SRC)
CXXFLAGS += -I$(DIR_NETOWRK_WRAPPER_LIB)/src

LDFLAGS += $(BUILD_FLAG)
LDFLAGS += $(DIR_NETOWRK_WRAPPER_LIB)/lib/native/network-wrapper.a
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

application:	ensureFolders  $(APP_OBJ)
	@echo ' ---> building $(LOG_INFO) "application"'
	@mkdir -p `dirname $(NAME_APP)`
	@$(CXX) $(CXXFLAGS) $(APP_OBJ) -o $(NAME_APP) $(LDFLAGS)
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

fclean:	clean
	$(RM) $(NAME_APP)

re:	fclean all

.PHONY:	\
	all \
	ensureFolders \
	application \
	clean \
	fclean \
	re

## RULE(S)
#
