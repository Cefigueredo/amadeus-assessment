#include "tinyxml2.h"
#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include "employee.cpp"
#include "utils/main.cpp"

using namespace std;
using json = nlohmann::json;

vector<Employee> employees {};
set<int> employeeIDs {};


void averageSalary() {
    int total = 0;
    for (const auto& employee : employees) {
        total += employee.getSalary();
    }
    double average = static_cast<double> (total) / employees.size();
    cout << "----- Average salary of the employees: ----\n"  << endl;
    cout << "Number of employees: " << employees.size() << endl;
    cout << "Average of salaries: " << average << endl;
    cout << endl;
}

void getHighestPaidEmployee() {
    int maxSalary = 0;
    Employee highestPaidEmployee("Dummy", 1, "Dummy", 1);
    for (const auto &employee : employees) {
        if (employee.getSalary() > maxSalary) {
            maxSalary = employee.getSalary();
            highestPaidEmployee = employee;
        }
    }
    cout << "----- Highest paid employee: -----\n" << endl;
    highestPaidEmployee.giveData();
    cout << endl;
}


void printOrderedEmployees() {
    cout << "----- Employees ordered by ID: -----\n" << endl;
    sort(employees.begin(), employees.end(), [](const Employee &a, const Employee &b) {
        return a.getId() < b.getId();
    });

    for (const auto &employee : employees) {
        employee.giveData();
        cout << endl;
    }
}

void handleOutput() {
    if (employees.empty()) {
        throw invalid_argument("No employees were loaded.");
    }

    printSection("Executing output handling...");
    averageSalary();
    getHighestPaidEmployee();
    printOrderedEmployees();
}

void addEmployee(const string &name, int id, const string &department, int salary) {
    try {
        cout << "* Adding employee: " << name << endl;
        if (employeeIDs.find(id) != employeeIDs.end()) {
            throw invalid_argument("Duplicate ID found: " + to_string(id));
        }
        employees.push_back(Employee(name, id, department, salary));
        employeeIDs.insert(id);
    } catch (const invalid_argument &e) {
        cout << "Exception adding employee: " << e.what() << endl;
        return;
    }
}

void parseXML(const string &filePath) {
    tinyxml2::XMLDocument doc;
    try {
        if (doc.LoadFile(filePath.c_str()) == tinyxml2::XML_SUCCESS) {
            tinyxml2::XMLElement* root = doc.FirstChildElement("employees");

            if (root) {
                for (tinyxml2::XMLElement* employee = root->FirstChildElement("employee"); employee; employee = employee->NextSiblingElement("employee")) {
                    const char *name = employee->FirstChildElement("name")->GetText();
                    const char *id = employee->FirstChildElement("id")->GetText();
                    const char *department = employee->FirstChildElement("department")->GetText();
                    const char *salary = employee->FirstChildElement("salary")->GetText();

                    int idInt = stoi(id);
                    int salaryInt = stoi(salary);

                    addEmployee(name, idInt, department, salaryInt);
                }
                cout << "XML file loaded successfully.\n" << endl;
            } else {
                cout << "No root element found.\n";
            }
        } else {
            throw runtime_error("Failed loading XML file.");
        }
    } catch (const runtime_error &e) {
        cout << "Failed loading an object in XML file: " << e.what() << endl;
        return;
    } catch (const invalid_argument &e) {
        cout << "Failed loading an object in XML file: " << e.what() << endl;
        return;
    }
}

void parseJSON(const string &filePath) {
    ifstream file(filePath);
    json jsonData;

    try {
        if (file >> jsonData) {
            for (const auto& employee : jsonData["employees"]) {
                string name = employee["name"];
                int id = employee["id"];
                string department = employee["department"];
                int salary = employee["salary"];

                addEmployee(name, id, department, salary);
            }
            cout << "JSON file loaded successfully.\n";
            cout << endl;
        }
    } catch (const json::exception &e) {
        cout << "Failed loading an object in JSON file: " << e.what() << endl;
        return;
    }
}

void executeFileParsing(const string &filePath) {
    printSection("Executing file parsing...");

    if (isXMLFile(filePath)) {
        parseXML(filePath);
    } else if (isJSONFile(filePath)){
        parseJSON(filePath);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
    
    try {
        executeFileParsing(filePath);
        handleOutput();
    } catch (const runtime_error &e) {
        cout << e.what() << endl;
        return 1;
    } catch (const invalid_argument &e) {
        cout << e.what() << endl;
        return 1;
    }
    return 0;
}
