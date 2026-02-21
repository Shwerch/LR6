CXX = g++
CXXFLAGS = -std=c++17 -O2
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

fix-bd:
	sudo -u postgres psql -c "ALTER USER postgres PASSWORD '12345';"
	sudo -u postgres psql -c "CREATE DATABASE aviation_db;"
	PGPASSWORD=12345 psql -h localhost -U postgres -d aviation_db -f create_tables.sql
	PGPASSWORD=12345 psql -h localhost -U postgres -d aviation_db -f insert_data.sql

.PHONY: all clean run db-init