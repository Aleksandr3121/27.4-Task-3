#include <iostream>
#include <list>
#include <vector>

using namespace std;

enum TypeTask {
    NONE,
    A,
    B,
    C
};

class Employee {
public:
    Employee(size_t sizeNode) : node(list<Employee*>(sizeNode)) {}

    virtual int Task(int intSpecifier = 0) = 0;

protected:
    list<Employee*> node;
};

class Worker : public Employee {
public:
    Worker() : Employee(0), typeTask(TypeTask::NONE) {}

    int Task(int inSpecifier = 0) override {
        if (typeTask != TypeTask::NONE)return 0;
        typeTask = static_cast<TypeTask>(inSpecifier);
        return 1;
    }

private:
    TypeTask typeTask;
};

class MiddleManager : public Employee {
public:
    explicit MiddleManager(size_t sizeTeam, size_t index) :
            Employee(sizeTeam),
            num(index),
            quantityTasks(0),
            quantityFreeWorkers(sizeTeam) {
        auto it = node.begin();
        while (it != node.end()) {
            (*it) = new Worker();
            ++it;
        }
    }

    int Task(int inSpecifier = 0) override {

        if (quantityFreeWorkers == 0)return 0;
        if (quantityFreeWorkers == 1) quantityTasks = 1;
        else {
            srand(inSpecifier + num);
            quantityTasks = 1 + rand() % quantityFreeWorkers;
        }

        auto it = node.begin();
        while (it != node.end() && quantityTasks > 0) {
            if ((*it)->Task((1 + rand() % 3))) {
                quantityTasks--;
                quantityFreeWorkers--;
            }
            ++it;
        }
        return quantityFreeWorkers;
    }

    ~MiddleManager() {
        auto it = node.begin();
        while (it != node.end()) {
            delete (*it);
            ++it;
        }
    }

private:
    size_t num;
    int quantityTasks;
    int quantityFreeWorkers;
};

class TopManager : public Employee {
public:
    explicit TopManager(const vector<size_t>& teams) : Employee(teams.size()) {
        auto it = node.begin();
        for (int i = 0; i < teams.size(); ++i) {
            (*it) = new MiddleManager(teams[i], i);
            ++it;
        }
    }

    int Task(int inSpecifier = 0) override {
        int quantityFreeWorkers = 0;
        auto it = node.begin();
        while (it != node.end()) {
            quantityFreeWorkers += ((*it)->Task(inSpecifier));
            ++it;
        }
        return quantityFreeWorkers;
    }

    ~TopManager() {
        auto it = node.begin();
        while (it != node.end()) {
            delete (*it);
            ++it;
        }
    }
};

class Company {
public:
    void CreateCompany(const vector<size_t>& teams) {
        head = new TopManager(teams);
    }

    int Task(int inSpecifier) {
        return head->Task(inSpecifier);
    }

    ~Company() {
        delete head;
    }

private:
    TopManager* head;
};

int main() {
    Company company;
    size_t quantityTeams;
    cout << "Enter the number of commands: ";
    cin >> quantityTeams;
    vector<size_t> teams(quantityTeams);
    for (int i = 0; i < teams.size(); ++i) {
        cout << "Enter the number of employees in the team " << i + 1 << endl;
        cin >> teams[i];
    }
    company.CreateCompany(teams);
    int idTask = 0;
    int quantityFreeWorkers = 0;
    do {
        cout << "Enter task id" << endl;
        cin >> idTask;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(32767, '\n');
            cerr << "Error of input" << endl;
            continue;
        }
        quantityFreeWorkers = company.Task(idTask);
        cout << "Number of available employees: " << quantityFreeWorkers << endl;
    } while (quantityFreeWorkers > 0);
    return 0;
}
