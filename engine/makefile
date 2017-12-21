

MODE?=64
PLAT=mingw

CC= gcc -m$(MODE) -std=c11 -g  -O0 -Wall  -fno-omit-frame-pointer  $(DFLAG)
CPP= g++ -m$(MODE) -std=c++11 -g -O0 -Wall-fno-omit-frame-pointer  $(DFLAG)
# CC := $(CPP)
DEFINES = 

LUA_PATH = "../thirdlib/lua5.3"
MYSQL_PATH = "../thirdlib/mysql6.0"


INCLUDE = -I"$(LUA_PATH)/include" -I"$(MYSQL_PATH)/include"
LIBS = -L"$(LUA_PATH)/lib/" -llua_$(PLAT)$(MODE) -L"$(MYSQL_PATH)/lib" -lmysql_$(PLAT)$(MODE)
SYS_LIBS = 

SRC_PATH?=lualib:tool:net
C_SRC_FILES = $(foreach dd,$(subst :, ,$(SRC_PATH)),$(wildcard $(dd)/*.c))
O_FILES = $(foreach dd, $(C_SRC_FILES), $(subst .c,.o,$(dd)))

%.o:%.c
	$(CC) -c $^ -o $@ $(INCLUDE) $(DEFINES)


randserver.exe: randserver.o $(O_FILES)
	$(CC) $^  -o $@  $(LIBS) $(SYS_LIBS)


test/test_serialize.exe:  test/test_serialize.o  
	$(CC) $^  -o $@  -L"$(LUA_PATH)/lib/" -llua_$(PLAT)$(MODE)


tool/%.o:tool/%.c
	$(CC) -c $^ -o $@
test/%.o:test/%.c
	$(CC) -c $^ -o $@

test_net_mingw:
	make test/libnet_service.a SRC_PATH=tool:net DFLAG="-D_WIN32_WINNT=_WIN32_WINNT_WINXP"
	make test/clients.exe  SRC_PATH=tool:net DFLAG="-D_WIN32_WINNT=_WIN32_WINNT_WINXP"
	make test/server.exe   SRC_PATH=tool:net DFLAG="-D_WIN32_WINNT=_WIN32_WINNT_WINXP"

test/libnet_service.a: $(O_FILES)
	ar rcs $@ $^

test/%.exe:  $(O_FILES)  test/%.o
	$(CC) $^ -o $@   -L"test"  -lnet_service -lws2_32 -lmswsock -lpthread

test/test_cpack.exe:test/test_cpack.o  tool/cpack.o
	$(CC) $^ -o $@

.PHONY : test
test:
	@echo $(MODE) $(PLAT)
	@echo $(LIBS)
	@echo $(O_FILES)

.PHONY : mingw
mingw:
	@echo $(MODE) $(PLAT)
	@echo $(LIBS)
	@echo $(O_FILES)
	cp $(MYSQL_PATH)/lib/libmysql_$(PLAT)$(MODE).dll ./libmysql.dll
	make DEFINES="-DWIN32 " SYS_LIBS="-lws2_32 -lmswsock -lpthread" randserver.exe


test/test_filesystem.exe: test/test_filesystem.c
	$(CC) $^  -o $@ 

.PHONY : clean
clean:
	rm -f $(O_FILES) test/*.exe test/test_serialize ./libmysql.dll ./*.exe test/*.o test/*.exe tool/*.o
