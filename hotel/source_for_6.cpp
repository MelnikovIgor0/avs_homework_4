#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <thread>

using namespace std;

class Hotel {
private:
    // массив одиночных номеров, первый элемент пары - id посетителя, второй - пол
    pair<int, int> singleRooms[10];

    // массив сдвоенных номеров (каждый номер: вектор из 2 пар), каждая пара
    // характеризует посетителя, первый элемент пары - id посетителя, второй - пол
    vector<pair<int, int>> pairedRooms[15];

public:
    Hotel() {
        // зануление всех номеров (вначале в отеле никто не живет)
        for (int i = 0; i < 10; i++) {
            singleRooms[i] = make_pair(0, 0);
        }
        for (int i = 0; i < 15; i++) {
            for (int j = 0; j < 2; j++) {
                pairedRooms[i].push_back(make_pair(0, 0));
            }
        }
    }

    bool process_client(int client_sex, int client_num) {
        // вначале пытаюсь заселить клиента в сдвоенный номер, в котором есть человек того же пола и свободное место
        for (int i = 0; i < 15; i++) {
            if (pairedRooms[i][0].second == client_sex && pairedRooms[i][1].second == 0) {
                pairedRooms[i][1].second = client_sex;
                pairedRooms[i][1].first = client_num;
                cout << "Visitor " << client_num << " booked room " << 10 + i << endl;
                return true;
            }
            if (pairedRooms[i][1].second == client_sex && pairedRooms[i][0].second == 0) {
                pairedRooms[i][0].second = client_sex;
                pairedRooms[i][0].first = client_num;
                cout << "Visitor " << client_num << " booked room " << 10 + i << endl;
                return true;
            }
        }
        // если не вышло, то пытаюсь заселить в пустой одиночный номер
        for (int i = 0; i < 10; i++) {
            if (singleRooms[i].second == 0) {
                singleRooms[i].first = client_num;
                singleRooms[i].second = client_sex;
                cout << "Visitor " << client_num << " booked room " << i << endl;
                return true;
            }
        }
        // если не вышло, то пытаюсь заселить в пустой сдвоенный номер
        for (int i = 0; i < 15; i++) {
            if (pairedRooms[i][0].second == 0 && pairedRooms[i][1].second == 0) {
                pairedRooms[i][0].second = client_sex;
                pairedRooms[i][0].first = client_num;
                cout << "Visitor " << client_num << " booked room " << 10 + i << endl;
                return true;
            }
        }
        // если не вышло, то заселять некуда :(
        cout << "Visitor " << client_num << " can not find room!" << endl;
        return false;
    }

    void client_leave(int client_num) {
        // выселяю человека, вначале ищу его в одиночных номерах и если нашел, выселяю
        for (int i = 0; i < 10; i++) {
            if (singleRooms[i].first == client_num) {
                singleRooms[i].first = 0;
                singleRooms[i].second = 0;
                cout << "Visitor " << client_num << " left from " << i << endl;
                return;
            }
        }
        // выселяю человека, ищу его в сдвоенных номерах и если нашел, выселяю
        for (int i = 0; i < 15; i++) {
            if (pairedRooms[i][0].first == client_num) {
                pairedRooms[i][0].first = 0;
                pairedRooms[i][0].second = 0;
                cout << "Visitor " << client_num << " left from " << i + 10 << endl;
                return;
            }
            if (pairedRooms[i][1].first == client_num) {
                pairedRooms[i][1].first = 0;
                pairedRooms[i][1].second = 0;
                cout << "Visitor " << client_num << " left from " << i + 10 << endl;
                return;
            }
        }
    }
};

// переменная отеля, в который заселяются посетители
Hotel h;

// переменная счетчик id посетителей
int counter = 0;

// функция, которая генерирует посетителя и пытается запихать его в отель
void* hotel_visitor(void* vptr_args) {
    int k = ++counter;
    // если не вышло запихать посетителя, то заканчиваем выполнение функции
    if (!h.process_client(rand() % 2 + 1, k)) {
        return NULL;
    }
    // посетитель пребывает в отеле, для этого делаю задержку
    this_thread::sleep_for(std::chrono::milliseconds(rand() % 5000));
    // выселяю посетителя
    h.client_leave(k);
    return NULL;
}

int main(int argc, char **argv) {
    // сбрасываю семя рандома
    srand(time(NULL));
    int n = atoi(argv[1]);
    if (n < 0 || n > 1000000) {
        cout << "invalid count of visitors!!!" << endl;
        return EXIT_SUCCESS;
    }
    // создаю вектор тредов
    vector<pthread_t> visitors(n);
    for (int i = 0; i < n; i++) {
        // делаю небольшую задержку в цикле, чтобы все посетители не пришли одновременно
        this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
        // вызываю в новом треде метод, пытающийся запихать пользователя
        if (pthread_create(&visitors[i], NULL, hotel_visitor, NULL)) {
            return EXIT_FAILURE;
        }
    }
    // жду окончания работы всех тредов, объединяю
    for (int i = 0; i < n; i++) {
        if (pthread_join(visitors[i], NULL)) {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}