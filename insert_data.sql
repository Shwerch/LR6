INSERT INTO airports (code, city, country) VALUES
('SVO', 'Москва', 'Россия'),
('LED', 'Санкт-Петербург', 'Россия'),
('DXB', 'Дубай', 'ОАЭ'),
('JFK', 'Нью-Йорк', 'США'),
('IST', 'Стамбул', 'Турция');

INSERT INTO airlines (name, base_country) VALUES
('Aeroflot', 'Россия'),
('Emirates', 'ОАЭ'),
('Turkish Airlines', 'Турция'),
('S7 Airlines', 'Россия');

INSERT INTO planes (model, capacity, airline_id) VALUES
('Airbus A320', 150, 1),
('Boeing 737', 180, 1),
('Airbus A380', 500, 2),
('Boeing 777', 300, 3),
('Embraer 170', 78, 4);

INSERT INTO flights (flight_number, plane_id, departure_airport_id, arrival_airport_id, departure_time, status, price) VALUES
('SU100', 1, 1, 2, '2024-06-01 10:00:00', 'Arrived', 5500.00),
('SU101', 2, 2, 1, '2024-06-01 18:00:00', 'Scheduled', 4500.00),
('EK132', 3, 1, 3, '2024-06-02 23:50:00', 'Scheduled', 45000.00),
('TK400', 4, 1, 5, '2024-06-03 14:30:00', 'Delayed', 25000.00),
('S7020', 5, 2, 1, '2024-06-04 08:00:00', 'Cancelled', 3000.00);

INSERT INTO tickets (flight_id, passenger_name, passport_number, seat_number) VALUES
(1, 'Иван Иванов', '1234 567890', '12A'),
(1, 'Петр Петров', '4321 098765', '12B'),
(3, 'Анна Смирнова', '5555 111222', '45K'),
(4, 'Джон Смит', 'US998877', '10C');