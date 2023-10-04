
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

DIR_TARGET=		./bin

NAME_APPLICATION=	$(DIR_TARGET)/exec

#### DIRS

DIR_SRC=	src

#### /DIRS

DIR_OBJ=	./obj/native

#### SRC

SRC+=	\
	$(wildcard \
		$(DIR_SRC)/*.cpp \
		$(DIR_SRC)/network-wrappers/http-server/*.cpp \
		$(DIR_SRC)/network-wrappers/http-server/internals/*.cpp \
		$(DIR_SRC)/network-wrappers/websocket-server/*.cpp \
		$(DIR_SRC)/network-wrappers/websocket-server/internals/*.cpp \
		$(DIR_SRC)/https-server/*.cpp \
		$(DIR_SRC)/https-server/file-manager/*.cpp \
		$(DIR_SRC)/websocket-server/*.cpp \
		$(DIR_SRC)/websocket-server/session-manager/*.cpp \
		)

#

OBJ=	$(patsubst %.cpp, $(DIR_OBJ)/%.o, $(SRC))

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
	@mkdir -p $(DIR_TARGET)

application:	ensureFolders $(OBJ)
	@echo ' ---> building $(LOG_INFO) "application"'
	@mkdir -p `dirname $(NAME_APPLICATION)`
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME_APPLICATION) $(LDFLAGS)
	@echo '   --> built $(LOG_INFO) "application"'

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
	$(RM) $(NAME_APPLICATION)

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
