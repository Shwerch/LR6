#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <iostream>
#include <string>
#include <pqxx/pqxx>

using namespace std;

void showMenu();

void viewAllFlights(pqxx::work& txn);
void viewAllAirports(pqxx::work& txn);
void viewAllPlanes(pqxx::work& txn);

void searchFlightsByCity(pqxx::work& txn);
void searchPlanesByAirline(pqxx::work& txn);

void registerNewPassenger(pqxx::work& txn);
void updateFlightStatus(pqxx::work& txn);
void cancelFlight(pqxx::work& txn);

void showAirportsStats(pqxx::work& txn); // GROUP BY + HAVING
void showMostExpensiveFlight(pqxx::work& txn); // Subquery
void showAirlinesFleet(pqxx::work& txn); // LEFT JOIN
void showGeneralStats(pqxx::work& txn); // Agregates

void demoVulnerableSearch(pqxx::work& txn);
void demoUnionInjection(pqxx::work& txn);
void demoVulnerableDelete(pqxx::work& txn);

#endif