#include "operations.h"

void showMenu() {
    cout << endl << string(40, '=') << endl;
    cout << "=== СИСТЕМА УПРАВЛЕНИЯ АВИАРЕЙСАМИ ===" << endl;
    cout << "1.  Все рейсы (Табло)" << endl;
    cout << "2.  Все аэропорты" << endl;
    cout << "3.  Все самолеты" << endl;
    
    cout << "\n=== ПОИСК ===" << endl;
    cout << "4.  Поиск рейсов по городу" << endl;
    cout << "5.  Самолеты конкретной авиакомпании" << endl;

    cout << "\n=== ОПЕРАЦИИ ===" << endl;
    cout << "6.  Купить билет (Регистрация пассажира)" << endl;
    cout << "7.  Обновить статус рейса" << endl;
    cout << "8.  Отменить рейс (Удаление)" << endl;

    cout << "\n=== АНАЛИТИКА ===" << endl;
    cout << "9.  Загруженные аэропорты (>1 вылета)" << endl;
    cout << "10. Самый дорогой рейс" << endl;
    cout << "11. Флот авиакомпаний" << endl;
    cout << "12. Общая статистика" << endl;

    cout << "\n=== БЕЗОПАСНОСТЬ (SQL Injection) ===" << endl;
    cout << "13. Уязвимый поиск пассажира" << endl;
    cout << "14. UNION инъекция (билеты)" << endl;
    cout << "15. Уязвимое удаление билета" << endl;

    cout << "\n0.  Выход" << endl;
}

// Показывает список всех рейсов с деталями через JOIN
void viewAllFlights(pqxx::work& txn) {
    cout << "\n--- Табло рейсов ---" << endl;
    pqxx::result res = txn.exec(
        "SELECT f.flight_number, f.departure_time, f.status, f.price, "
        "dep.city as from_city, arr.city as to_city, a.name as airline "
        "FROM flights f "
        "JOIN airports dep ON f.departure_airport_id = dep.airport_id "
        "JOIN airports arr ON f.arrival_airport_id = arr.airport_id "
        "JOIN planes p ON f.plane_id = p.plane_id "
        "JOIN airlines a ON p.airline_id = a.airline_id "
        "ORDER BY f.departure_time"
    );
    //перебор полученных строк
    for (const auto& row : res) {
        cout << "[" << row["flight_number"].c_str() << "] "
        //вывод данных
             << row["from_city"].c_str() << " -> " << row["to_city"].c_str()
             << " | " << row["status"].c_str()
             << " | " << row["airline"].c_str()
             << " | " << row["price"].as<double>() << " руб." << endl;
    }
}

// Показывает список аэропортов
void viewAllAirports(pqxx::work& txn) {
    pqxx::result res = txn.exec("SELECT code, city, country FROM airports ORDER BY city");
    for (const auto& row : res) {
        cout << row["code"].c_str() << " - " 
             << row["city"].c_str() << " (" << row["country"].c_str() << ")" << endl;
    }
}

// Показывает список самолетов
void viewAllPlanes(pqxx::work& txn) {
    pqxx::result res = txn.exec(
        "SELECT p.model, p.capacity, a.name "
        "FROM planes p JOIN airlines a ON p.airline_id = a.airline_id"
    );
    for (const auto& row : res) {
        cout << row["model"].c_str() << " (" << row["capacity"].as<int>() << " мест) - "
             << row["name"].c_str() << endl;
    }
}

// Поиск рейсов по названию города
void searchFlightsByCity(pqxx::work& txn) {
    string cityName;
    cout << "Введите город (или часть названия): ";
    getline(cin, cityName);

    // Параметризованный запрос для безопасности
    pqxx::result res = txn.exec_params(
        "SELECT f.flight_number, dep.city as from_c, arr.city as to_c "
        "FROM flights f "
        "JOIN airports dep ON f.departure_airport_id = dep.airport_id "
        "JOIN airports arr ON f.arrival_airport_id = arr.airport_id "
        "WHERE dep.city ILIKE '%' || $1 || '%' OR arr.city ILIKE '%' || $1 || '%'",
        cityName //позволяет найти безопасно город
    );

    if (res.empty()) 
        cout << "Рейсов не найдено." << endl;
    
    for (const auto& row : res) {
        cout << "Рейс " << row["flight_number"].c_str() << ": "
             << row["from_c"].c_str() << " -> " << row["to_c"].c_str() << endl;
    }
}

// Поиск самолетов по названию авиакомпании
void searchPlanesByAirline(pqxx::work& txn) {
    string airlineName;
    cout << "Введите название авиакомпании: ";
    getline(cin, airlineName);

    pqxx::result res = txn.exec_params(
        "SELECT p.model, p.capacity FROM planes p "
        "JOIN airlines a ON p.airline_id = a.airline_id "
        "WHERE a.name ILIKE $1",
        airlineName
    );

    if (res.empty()) cout << "Флот не найден." << endl;

    for (const auto& row : res) {
        cout << "Самолет: " << row["model"].c_str() << ", Мест: " << row["capacity"].as<int>() << endl;
    }
}

// Добавление нового пассажира
void registerNewPassenger(pqxx::work& txn) {
    string flightNum, name, passport, seat;
    
    cout << "Номер рейса (например, SU100): ";
    cin >> flightNum;
    cin.ignore(); 

    // Проверяем существование рейса и получаем его ID
    pqxx::result flightRes = txn.exec_params("SELECT flight_id FROM flights WHERE flight_number = $1", flightNum);
    
    if (flightRes.empty()) {
        cout << "Ошибка: Рейс не найден!" << endl;
        return;
    }
    int flightId = flightRes[0][0].as<int>();

    cout << "ФИО Пассажира: ";
    getline(cin, name);
    cout << "Номер паспорта: ";
    getline(cin, passport);
    cout << "Место (например, 15F): ";
    cin >> seat;
    cin.ignore();

    txn.exec_params(
        "INSERT INTO tickets (flight_id, passenger_name, passport_number, seat_number) "
        "VALUES ($1, $2, $3, $4)",
        flightId, name, passport, seat
    );
    cout << "Билет успешно оформлен!" << endl;
}

// Обновление статуса рейса
void updateFlightStatus(pqxx::work& txn) {
    string flightNum, newStatus;
    cout << "Номер рейса: ";
    cin >> flightNum;
    cout << "Новый статус (Scheduled/Delayed/Departed/Arrived/Cancelled): ";
    cin >> newStatus;
    cin.ignore();

    pqxx::result res = txn.exec_params(
        "UPDATE flights SET status = $1 WHERE flight_number = $2",
        newStatus, flightNum
    );

    if (res.affected_rows() > 0) cout << "Статус обновлен." << endl;
    else cout << "Рейс не найден." << endl;
}

void cancelFlight(pqxx::work& txn) {
    string flightNum;
    cout << "Введите номер рейса для отмены (удаления): ";
    cin >> flightNum;
    cin.ignore();

    // Каскадное удаление удалит и билеты
    pqxx::result res = txn.exec_params(
        "DELETE FROM flights WHERE flight_number = $1", flightNum
    );

    cout << "Удалено записей: " << res.affected_rows() << endl;
}

// Аналитика: Аэропорты с количеством вылетов > 1
void showAirportsStats(pqxx::work& txn) {
    cout << "\n--- Загруженные аэропорты ---" << endl;
    pqxx::result res = txn.exec(
        "SELECT a.city, COUNT(f.flight_id) as flights_count "
        "FROM airports a "
        "JOIN flights f ON a.airport_id = f.departure_airport_id "
        "GROUP BY a.city "
        "HAVING COUNT(f.flight_id) > 1 "
        "ORDER BY flights_count DESC"
    );

    for (const auto& row : res) {
        cout << row["city"].c_str() << ": " << row["flights_count"].as<int>() << " вылетов" << endl;
    }
}

// Аналитика: Самый дорогой рейс
void showMostExpensiveFlight(pqxx::work& txn) {
    cout << "\n--- Самый дорогой перелет ---" << endl;
    pqxx::result res = txn.exec(
        "SELECT flight_number, price FROM flights "
        "WHERE price = (SELECT MAX(price) FROM flights)"
    );

    for (const auto& row : res) {
        cout << "Рейс " << row["flight_number"].c_str() 
             << " стоит " << row["price"].as<double>() << " руб." << endl;
    }
}

// Аналитика: Количество самолетов у компаний (LEFT JOIN)
void showAirlinesFleet(pqxx::work& txn) {
    cout << "\n--- Статистика флота ---" << endl;
    pqxx::result res = txn.exec(
        "SELECT a.name, COUNT(p.plane_id) as plane_count "
        "FROM airlines a "
        "LEFT JOIN planes p ON a.airline_id = p.airline_id "
        "GROUP BY a.name"
    );

    for (const auto& row : res) {
        cout << row["name"].c_str() << ": " << row["plane_count"].as<int>() << " судов" << endl;
    }
}

void showGeneralStats(pqxx::work& txn) {
    cout << "\n--- Общая статистика ---" << endl;
    pqxx::result res = txn.exec(
        "SELECT "
        "(SELECT COUNT(*) FROM flights) as total_flights, "
        "(SELECT AVG(price)::numeric(10,2) FROM flights) as avg_price, "
        "(SELECT COUNT(*) FROM tickets) as total_passengers"
    );
    
    cout << "Всего рейсов: " << res[0]["total_flights"].as<int>() << endl;
    cout << "Средняя цена: " << res[0]["avg_price"].as<double>() << endl;
    cout << "Продано билетов: " << res[0]["total_passengers"].as<int>() << endl;
}

// Уязвимый поиск (SQL Injection)
void demoVulnerableSearch(pqxx::work& txn) {
    string namePart;
    cout << "Введите имя пассажира: ";
    getline(cin, namePart);

    // Прямая конкатенация строки!
    string query = "SELECT * FROM tickets WHERE passenger_name LIKE '%" + namePart + "%'";
    cout << "Выполняемый SQL: " << query << endl;

    try {
        pqxx::result res = txn.exec(query);
        for (const auto& row : res) {
            cout << "Билет ID: " << row["ticket_id"].c_str() 
                 << ", Пассажир: " << row["passenger_name"].c_str() << endl;
        }
    } catch (const exception& e) {
        cout << "Ошибка SQL: " << e.what() << endl;
    }
    cout << "Подсказка для взлома: введите ' OR '1'='1" << endl;
}

// UNION Injection
void demoUnionInjection(pqxx::work& txn) {
    string idStr;
    cout << "Введите ID билета: ";
    getline(cin, idStr);

    // Отсутствие проверки типов и конкатенация
    string query = "SELECT passenger_name, seat_number FROM tickets WHERE ticket_id = " + idStr;
    cout << "Выполняемый SQL: " << query << endl;
    
    try {
        pqxx::result res = txn.exec(query);
        for (const auto& row : res) {
            cout << "Данные: " << row[0].c_str() << " | " << row[1].c_str() << endl;
        }
    } catch (const exception& e) {
        cout << "Ошибка: " << e.what() << endl;
    }
    cout << "Подсказка: 1 UNION SELECT name, base_country FROM airlines" << endl;
}

// Уязвимое удаление
void demoVulnerableDelete(pqxx::work& txn) {
    string passNum;
    cout << "Введите номер паспорта для удаления билетов: ";
    getline(cin, passNum);

    string query = "DELETE FROM tickets WHERE passport_number = '" + passNum + "'";
    cout << "SQL: " << query << endl;
    cout << "[DEMO] Запрос сформирован, но выполнение заблокировано в целях безопасности демонстрации." << endl;
    cout << "Подсказка: ' OR '1'='1 приведет к удалению всех билетов!" << endl;
}