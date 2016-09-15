CPP  = g++ -std=c++11
CC   = gcc

OBJ  = Application.o apriori.o main.o istree.o symtab.o tfscan.o tract.o Trie.o vecops.o Zigzag.o $(RES)
LINKOBJ  = Application.o apriori.o main.o istree.o symtab.o tfscan.o tract.o Trie.o vecops.o Zigzag.o $(RES)

BIN  = fim_maximal 
CXXFLAGS =    -fpermissive -Wformat-security -Wint-to-pointer-cast
CFLAGS =    -fpermissive -Wformat-security -Wint-to-pointer-cast

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $@

Application.o: Application.cpp
	$(CPP) -c Application.cpp -o Application.o $(CXXFLAGS)

apriori.o: apriori.cpp
	$(CC) -c apriori.cpp -o apriori.o $(CFLAGS)

main.o: main.cpp
	$(CPP) -c main.cpp -o main.o $(CXXFLAGS)

istree.o: istree.cpp
	$(CC) -c istree.cpp -o istree.o $(CFLAGS)

symtab.o: symtab.cpp
	$(CC) -c symtab.cpp -o symtab.o $(CFLAGS)

tfscan.o: tfscan.cpp
	$(CC) -c tfscan.cpp -o tfscan.o $(CFLAGS)

tract.o: tract.cpp
	$(CC) -c tract.cpp -o tract.o $(CFLAGS)

Trie.o: Trie.cpp
	$(CPP) -c Trie.cpp -o Trie.o $(CXXFLAGS)

vecops.o: vecops.cpp
	$(CC) -c vecops.cpp -o vecops.o $(CFLAGS)

Zigzag.o: Zigzag.cpp
	$(CPP) -c Zigzag.cpp -o Zigzag.o $(CXXFLAGS)
