#include "operations.h"

// Основная функция программы
int main() {
    try {
        // Подключение к PostgreSQL. Измените параметры под вашу систему!
        string connStr = "host=localhost port=5432 dbname=aviation_db user=postgres password=postgres";
        pqxx::connection conn(connStr);

        if (!conn.is_open()) {
            cout << "Критическая ошибка: Нет подключения к БД!" << endl;
            return 1;
        }
        cout << "Подключение к базе данных успешно!" << endl;

        int choice = -1;
        while (choice != 0) {
            showMenu();
            cout << "Ваш выбор: ";
            
            // Проверка на ввод числа
            if (!(cin >> choice)) {
                cout << "Ошибка ввода! Введите число." << endl;
                cin.clear();
                cin.ignore(10000, '\n');
                continue;
            }
            cin.ignore(); // Очистка буфера после ввода числа

            // Создаем транзакцию для каждой операции
            pqxx::work txn(conn);

            try {
                switch (choice) {
                    case 1: viewAllFlights(txn); break;
                    case 2: viewAllAirports(txn); break;
                    case 3: viewAllPlanes(txn); break;
                    case 4: searchFlightsByCity(txn); break;
                    case 5: searchPlanesByAirline(txn); break;
                    case 6: registerNewPassenger(txn); break;
                    case 7: updateFlightStatus(txn); break;
                    case 8: cancelFlight(txn); break;
                    case 9: showAirportsStats(txn); break;
                    case 10: showMostExpensiveFlight(txn); break;
                    case 11: showAirlinesFleet(txn); break;
                    case 12: showGeneralStats(txn); break;
                    case 13: demoVulnerableSearch(txn); break;
                    case 14: demoUnionInjection(txn); break;
                    case 15: demoVulnerableDelete(txn); break;
                    case 0: cout << "Завершение работы..." << endl; break;
                    default: cout << "Неверный пункт меню." << endl;
                }
                
                // Фиксируем изменения, если это не выход
                if (choice != 0) {
                    txn.commit();
                }
            } catch (const exception &e) {
                cout << "Ошибка выполнения операции: " << e.what() << endl;
            }
        }

    } catch (const exception &e) {
        cerr << "Глобальная ошибка: " << e.what() << endl;
        return 1;
    }

    return 0;
}