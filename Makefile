CXX = g++
CXXFLAGS = -std=c++17
LDFLAGS = -lpqxx -lpq
TARGET = flight_app
SOURCES = main.cpp operations.cpp
HEADERS = operations.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Для быстрой настройки БД
db-init:
	psql -U postgres -d aviation_db -f create_tables.sql
	psql -U postgres -d aviation_db -f insert_data.sql

.PHONY: all clean run db-init