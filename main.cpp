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

class Manager {
public:
    Manager(size_t sizeNode) : node(list<Manager*>(sizeNode)) {}

    virtual int Task(int intSpecifier = 0) = 0;

    virtual TypeTask GetTypeTask() const = 0;

protected:
    list<Manager*> node;
};

class Worker : public Manager {
public:
    Worker() : Manager(0), typeTask(TypeTask::NONE) {}

    int Task(int inSpecifier = 0) override {
        typeTask = static_cast<TypeTask>(inSpecifier);
        return 0;
    }

    TypeTask GetTypeTask() const {
        return typeTask;
    }

private:
    TypeTask typeTask;
};

class MiddleManager : public Manager {
public:
    explicit MiddleManager(size_t sizeTeam, size_t index) :
            Manager(sizeTeam),
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
            if ((*it)->GetTypeTask() == TypeTask::NONE) {
                (*it)->Task((1 + rand() % 3));
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
    //переопределил виртуальный метод и записал в private,
    //чтобы к нему нельзя было обратиться - он не нужен в этом наследнике.
    //Так вообще делают в рабочих проектах?
    TypeTask GetTypeTask() const override {
        return TypeTask::NONE;
    }

    size_t num;
    int quantityTasks;
    int quantityFreeWorkers;
};

class TopManager : public Manager {
public:
    explicit TopManager(const vector<size_t>& teams) : Manager(teams.size()) {
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

private:
    TypeTask GetTypeTask() const override {
        return TypeTask::NONE;
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
    TopManager*head;
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
