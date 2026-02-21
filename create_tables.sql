DROP TABLE IF EXISTS tickets;
DROP TABLE IF EXISTS flights;
DROP TABLE IF EXISTS planes;
DROP TABLE IF EXISTS airlines;
DROP TABLE IF EXISTS airports;

CREATE TABLE airports (
    airport_id SERIAL PRIMARY KEY,
    code VARCHAR(3) NOT NULL UNIQUE,
    city VARCHAR(100) NOT NULL,
    country VARCHAR(100) NOT NULL
);

CREATE TABLE airlines (
    airline_id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    base_country VARCHAR(100)
);

CREATE TABLE planes (
    plane_id SERIAL PRIMARY KEY,
    model VARCHAR(100) NOT NULL,
    capacity INTEGER CHECK (capacity > 0),
    airline_id INTEGER REFERENCES airlines(airline_id) ON DELETE CASCADE
);

CREATE TABLE flights (
    flight_id SERIAL PRIMARY KEY,
    flight_number VARCHAR(10) NOT NULL,
    plane_id INTEGER REFERENCES planes(plane_id) ON DELETE SET NULL,
    departure_airport_id INTEGER REFERENCES airports(airport_id) ON DELETE CASCADE,
    arrival_airport_id INTEGER REFERENCES airports(airport_id) ON DELETE CASCADE,
    departure_time TIMESTAMP NOT NULL,
    status VARCHAR(20) DEFAULT 'Scheduled',
    price DECIMAL(10, 2) NOT NULL
);

CREATE TABLE tickets (
    ticket_id SERIAL PRIMARY KEY,
    flight_id INTEGER REFERENCES flights(flight_id) ON DELETE CASCADE,
    passenger_name VARCHAR(150) NOT NULL,
    passport_number VARCHAR(50) NOT NULL,
    seat_number VARCHAR(5),
    purchase_date DATE DEFAULT CURRENT_DATE
);