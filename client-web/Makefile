
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

LOG_INFO= '[$(build_mode)] [$(build_mode)]'

#

DIR_TARGET=		./dist/wasm

NAME_APPLICATION=		$(DIR_TARGET)/client.js

#### DIRS

DIR_SRC=	src

#### /DIRS

DIR_OBJ=	./obj/web-wasm

#### SRC

SRC+=	\
	$(wildcard \
		$(DIR_SRC)/*.cpp \
		$(DIR_SRC)/application/*.cpp \
		$(DIR_SRC)/application/network/*.cpp \
		$(DIR_SRC)/utilities/*.cpp \
		)

#

OBJ=	$(patsubst %.cpp, $(DIR_OBJ)/%.o, $(SRC))

#

#



#######


ifeq ($(build_mode),release)

BUILD_FLAG=		-O3

else

BUILD_FLAG=		-g3

endif


CXX=em++
AR=emar

CXXFLAGS+=	$(BUILD_FLAG)
CXXFLAGS+=	-std=c++17
CXXFLAGS+=	-Wall -W -Wextra -Wunused -Wpedantic -Wshadow -Wconversion -Werror
CXXFLAGS+=	-I$(DIR_SRC)

LDFLAGS+=	$(BUILD_FLAG)
LDFLAGS+=	-lwebsocket.js
LDFLAGS+=	-s TOTAL_MEMORY=128Mb
LDFLAGS+=	-s WASM=1
LDFLAGS+=	-s EXPORTED_RUNTIME_METHODS=cwrap,allocateUTF8,_free

ifeq ($(build_mode),debug)

LDFLAGS+=	-s DEMANGLE_SUPPORT=1
LDFLAGS+=	-s GL_ASSERTIONS=1
LDFLAGS+=	-s GL_DEBUG=1
LDFLAGS+=	-s DISABLE_EXCEPTION_CATCHING=2

endif

RM=			rm -rf


#######

#
## RULE(S)

all:
	@$(MAKE) --no-print-directory application

ensureFolders:
	@mkdir -p $(DIR_TARGET)

application:
	@$(MAKE) --no-print-directory ensureFolders
	@$(MAKE) --no-print-directory $(OBJ)
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

fclean:
	@$(MAKE) --no-print-directory clean
	$(RM) $(NAME_APPLICATION)

re:
	@$(MAKE) --no-print-directory fclean
	@$(MAKE) --no-print-directory all

.PHONY:	\
	all \
	ensureFolders \
	application \
	clean \
	fclean \
	re

## RULE(S)
#
