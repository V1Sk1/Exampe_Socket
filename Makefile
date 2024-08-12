# Makefile

CXX = g++
CXXFLAGS = -std=c++11 -pthread

# Название исполняемых файлов
CLIENT_EXEC = Programm_1
SERVER_EXEC = Programm_2

# Название исходных файлов
CLIENT_SRC = Programm_1.cpp
SERVER_SRC = Programm_2.cpp

# Правило по умолчанию для сборки клиента и сервера
all: $(CLIENT_EXEC) $(SERVER_EXEC)

# Правило для сборки клиента
$(CLIENT_EXEC): $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) -o $(CLIENT_EXEC) $(CLIENT_SRC)

# Правило для сборки сервера
$(SERVER_EXEC): $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) -o $(SERVER_EXEC) $(SERVER_SRC)

# Команда очистки
clean:
	rm -f $(CLIENT_EXEC) $(SERVER_EXEC)

# Команда повторной сборки
rebuild: clean all
