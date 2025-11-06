PARSER=parser
DUG=debug
HTTP_SERVER=http_server
cc=g++

.PHONY:all
all:$(PARSER) $(DUG) $(HTTP_SERVER)

$(PARSER):parser.cc
	$(cc) -o $@ $^ -lboost_system -lboost_filesystem -std=c++11 

$(DUG):debug.cc  # debug用来进行命令行调试
	$(cc) -o $@ $^ -ljsoncpp -std=c++11

$(HTTP_SERVER):http_server.cc # http_server用来进行命令行请求
	$(cc) -o $@ $^ -ljsoncpp -lpthread -std=c++11

.PHONY:clean
clean:
	rm -f $(PARSER) $(DUG) $(HTTP_SERVER)