# Отчет ИДЗ4, Мельников Игорь, БПИ218, Вариант 14.

Задача о гостинице - 3 (дамы и джентльмены). В гостинице 10 номеров рассчитаны на одного человека и 15 номеров рассчитаны на двух человек. 
В гостиницу случайно приходят клиенты дамы и клиенты джентльмены,
и конечно они могут провести ночь в номере только с представителем своего
пола. Если для клиента не находится подходящего номера, он уходит искать
ночлег в другое место. Клиенты порождаются динамически и уничтожаются
при освобождении номера или уходе из гостиницы при невозможности поселиться. Создать многопоточное приложение, моделирующее работу гостиницы.

## Задания на 4 балла:

### Описание модели параллельных вычислений:

Поведение посетителя реализовано в функции hotel_visitor. Благодаря этой функции в отель заселяется человек 
случайного пола с автоинкрементирующимся id. Для каждого нового посетителя отеля заведу отдельный thread.

Опишу класс отеля, который хранит номера, а также для каждого номера хранит id и пол человека, живущего в нем. 
Отель реализует методы заселения пользователя в отель по id и его полу и  выселения пользователя по его id. 
Для моделирования работы отеля был выбран алгоритм, который гарантирует максимальное заполнение номеров отеля:
приходящего пользователя в первую очередь пытаются заселить в номер к человеку того же пола, если таких номеров 
нет, то в номер на одного, если таких нет, то в пустой номер на двоих.

Сгенерирую указанное пользователем количество посетителей отеля (то есть указанное количество раз в новом
потоке запущу функцию hotel_visitor) (генерировать буду с небольшой задержкой, чтобы имитировать реальное
поведение посетителей, которые время от времени приходят в отель). После этого дождусь окончания работы
всех потоков.

### Описание входных данных:
Программа принимает на вход одно целое число: количество посетителей отеля, которые захотят прийти.
Пользователь не влияет на плотность потока посетителей, их среднее время пребывания в отеле, время
пребывания в отеле конкретного посетителя, а также никаким образом не влияет на поведение посетителей
или меахнику заселения и выселения из отеля.

### Исходный код (source_for_4.cpp)

```c
#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <thread>

using namespace std;

class Hotel {
private:
    // .first - id, .second - sex
    pair<int, int> singleRooms[10];
    vector<pair<int, int>> pairedRooms[15];

public:
    Hotel() {
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
        for (int i = 0; i < 10; i++) {
            if (singleRooms[i].second == 0) {
                singleRooms[i].first = client_num;
                singleRooms[i].second = client_sex;
                cout << "Visitor " << client_num << " booked room " << i << endl;
                return true;
            }
        }
        for (int i = 0; i < 15; i++) {
            if (pairedRooms[i][0].second == 0 && pairedRooms[i][1].second == 0) {
                pairedRooms[i][0].second = client_sex;
                pairedRooms[i][0].first = client_num;
                cout << "Visitor " << client_num << " booked room " << 10 + i << endl;
                return true;
            }
        }
        cout << "Visitor " << client_num << " can not find room!" << endl;
        return false;
    }

    void client_leave(int client_num) {
        for (int i = 0; i < 10; i++) {
            if (singleRooms[i].first == client_num) {
                singleRooms[i].first = 0;
                singleRooms[i].second = 0;
                cout << "Visitor " << client_num << " left from " << i << endl;
                return;
            }
        }
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

Hotel h;
int counter = 0;

void* hotel_visitor(void* vptr_args) {
    int k = ++counter;
    if (!h.process_client(rand() % 2 + 1, k)) {
        return NULL;
    }
    this_thread::sleep_for(std::chrono::milliseconds(rand() % 5000));
    h.client_leave(k);
    return NULL;
}

int main() {
    srand(time(NULL));
    int n;
    cin >> n;
    if (n < 0 || n > 1000000) {
        cout << "invalid count of visitors!!!" << endl;
        return EXIT_SUCCESS;
    }
    vector<pthread_t> visitors(n);
    for (int i = 0; i < n; i++) {
        this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
        if (pthread_create(&visitors[i], NULL, hotel_visitor, NULL)) {
            return EXIT_FAILURE;
        }
    }
    for (int i = 0; i < n; i++) {
        if (pthread_join(visitors[i], NULL)) {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
```

## Задания на 5 баллов:

### Комментирую код (source_for_4.cpp):

```c
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

int main() {
    // сбрасываю семя рандома
    srand(time(NULL));
    int n;
    cin >> n;
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
```

### Описания сценария одновременного действия сущностей:

Два и более посетителя могут одновременно прийти в отель и запросить номер. В таком случае, номер для
них будет подбираться по одному и тому же алгоритму, и им будет одновременно предложен один и тот же 
номер. В таком случае, кто-то один из них успеет записаться в этот номер, другой не успеет
(потенциально, может возникнуть ситуация, когда на одно и то же место в номере записалось 2 человека,
но при запуске программы ~50 раз такая ситуация не была ни разу обнаружена). Также, одновременно 2 и
более посетителя могут одновременно выселиться, но это точно не создаст проблем, потому что они не
конкурируют за одни и те же номера.

## Задания на 6 баллов:

### Подробное описание обобщенного алгоритма:

Поведение посетителя реализовано в функции hotel_visitor. Благодаря этой функции в отель заселяется человек 
случайного пола с автоинкрементирующимся id. Для каждого нового посетителя отеля заведу отдельный thread.

Опишу класс отеля, который хранит номера, а также для каждого номера хранит id и пол человека, живущего в нем. 
Отель реализует методы заселения пользователя в отель по id и его полу и выселения пользователя по его id. 
Для моделирования работы отеля был выбран алгоритм, который гарантирует максимальное заполнение номеров отеля:
приходящего пользователя в первую очередь пытаются заселить в номер к человеку того же пола, если таких номеров 
нет, то в номер на одного, если таких нет, то в пустой номер на двоих.

Сгенерирую указанное пользователем количество посетителей отеля (то есть указанное количество раз в новом
потоке запущу функцию hotel_visitor) (генерировать буду с небольшой задержкой, чтобы имитировать реальное
поведение посетителей, которые время от времени приходят в отель). Каждый раз при запуске функции будет
производиться попытка зписать посетителя в номер (при этом некоторым посетителям места может не хватить).
После этого дождусь окончания работы всех потоков.

### Реализую ввод данных из командной строки (task_for_6.cpp):

```c
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
```

## Задания на 7 баллов:

### Изменяю программу так, чтобы в ней был ввод/вывод из файла, передаваемого через аргументы:

```c
#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <thread>
#include <fstream>

using namespace std;

ofstream fout;

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
                fout << "Visitor " << client_num << " booked room " << 10 + i << endl;
                return true;
            }
            if (pairedRooms[i][1].second == client_sex && pairedRooms[i][0].second == 0) {
                pairedRooms[i][0].second = client_sex;
                pairedRooms[i][0].first = client_num;
                cout << "Visitor " << client_num << " booked room " << 10 + i << endl;
                fout << "Visitor " << client_num << " booked room " << 10 + i << endl;
                return true;
            }
        }
        // если не вышло, то пытаюсь заселить в пустой одиночный номер
        for (int i = 0; i < 10; i++) {
            if (singleRooms[i].second == 0) {
                singleRooms[i].first = client_num;
                singleRooms[i].second = client_sex;
                cout << "Visitor " << client_num << " booked room " << i << endl;
                fout << "Visitor " << client_num << " booked room " << i << endl;
                return true;
            }
        }
        // если не вышло, то пытаюсь заселить в пустой сдвоенный номер
        for (int i = 0; i < 15; i++) {
            if (pairedRooms[i][0].second == 0 && pairedRooms[i][1].second == 0) {
                pairedRooms[i][0].second = client_sex;
                pairedRooms[i][0].first = client_num;
                cout << "Visitor " << client_num << " booked room " << 10 + i << endl;
                fout << "Visitor " << client_num << " booked room " << 10 + i << endl;
                return true;
            }
        }
        // если не вышло, то заселять некуда :(
        cout << "Visitor " << client_num << " can not find room!" << endl;
        fout << "Visitor " << client_num << " can not find room!" << endl;
        return false;
    }

    void client_leave(int client_num) {
        // выселяю человека, вначале ищу его в одиночных номерах и если нашел, выселяю
        for (int i = 0; i < 10; i++) {
            if (singleRooms[i].first == client_num) {
                singleRooms[i].first = 0;
                singleRooms[i].second = 0;
                cout << "Visitor " << client_num << " left from " << i << endl;
                fout << "Visitor " << client_num << " left from " << i << endl;
                return;
            }
        }
        // выселяю человека, ищу его в сдвоенных номерах и если нашел, выселяю
        for (int i = 0; i < 15; i++) {
            if (pairedRooms[i][0].first == client_num) {
                pairedRooms[i][0].first = 0;
                pairedRooms[i][0].second = 0;
                cout << "Visitor " << client_num << " left from " << i + 10 << endl;
                fout << "Visitor " << client_num << " left from " << i + 10 << endl;
                return;
            }
            if (pairedRooms[i][1].first == client_num) {
                pairedRooms[i][1].first = 0;
                pairedRooms[i][1].second = 0;
                cout << "Visitor " << client_num << " left from " << i + 10 << endl;
                fout << "Visitor " << client_num << " left from " << i + 10 << endl;
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
    ifstream fin(argv[1]);
    fout.open(argv[2]);
    int n;
    fin >> n;
    fin.close();
    if (n < 0 || n > 1000000) {
        cout << "invalid count of visitors!!!" << endl;
        fout << "invalid count of visitors!!!" << endl;
        fout.close();
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
    fout.close();
    return EXIT_SUCCESS;
}
```

### Привожу пример входных и выходных данных:

Файл с входными данными содержит только одно число - 200.

Выходные данные:

```
Visitor 1 booked room 0
Visitor 2 booked room 1
Visitor 3 booked room 2
Visitor 4 booked room 3
Visitor 5 booked room 4
Visitor 6 booked room 5
Visitor 7 booked room 6
Visitor 8 booked room 7
Visitor 9 booked room 8
Visitor 10 booked room 9
Visitor 11 booked room 10
Visitor 12 booked room 10
Visitor 13 booked room 11
Visitor 14 booked room 11
Visitor 15 booked room 12
Visitor 16 booked room 12
Visitor 17 booked room 13
Visitor 18 booked room 13
Visitor 19 booked room 14
Visitor 20 booked room 14
Visitor 21 booked room 15
Visitor 22 booked room 15
Visitor 23 booked room 16
Visitor 24 booked room 16
Visitor 25 booked room 17
Visitor 26 booked room 17
Visitor 27 booked room 18
Visitor 28 booked room 18
Visitor 29 booked room 19
Visitor 30 booked room 19
Visitor 31 booked room 20
Visitor 32 booked room 20
Visitor 33 booked room 21
Visitor 34 booked room 21
Visitor 35 booked room 22
Visitor 36 booked room 22
Visitor 37 booked room 23
Visitor 38 booked room 23
Visitor 39 booked room 24
Visitor 40 booked room 24
Visitor 41 can not find room!
Visitor 42 can not find room!
Visitor 43 can not find room!
Visitor 44 can not find room!
Visitor 45 can not find room!
Visitor 46 can not find room!
Visitor 47 can not find room!
Visitor 48 can not find room!
Visitor 49 can not find room!
Visitor 50 can not find room!
Visitor 51 can not find room!
Visitor 52 can not find room!
Visitor 53 can not find room!
Visitor 54 can not find room!
Visitor 55 can not find room!
Visitor 56 can not find room!
Visitor 57 can not find room!
Visitor 58 can not find room!
Visitor 59 can not find room!
Visitor 60 can not find room!
Visitor 61 can not find room!
Visitor 1 left from 0
Visitor 2 left from 1
Visitor 62 booked room 0
Visitor 3 left from 2
Visitor 4 left from 3
Visitor 5 left from 4
Visitor 63 booked room 1
Visitor 64 booked room 2
Visitor 6 left from 5
Visitor 65 booked room 3
Visitor 7 left from 6
Visitor 8 left from 7
Visitor 66 booked room 4
Visitor 9 left from 8
Visitor 10 left from 9
Visitor 11 left from 10
Visitor 12Visitor 67 booked room 5
 left from 10
Visitor 13 left from 11
Visitor 68 booked room 11
Visitor 69 booked room 6
Visitor 70 booked room 7
Visitor 14 left from 11
Visitor 15 left from 12
Visitor 71 booked room 11
Visitor 16 left from 12
Visitor 17 left from 13
Visitor 72 booked room 13
Visitor 18 left from 13
Visitor 73 booked room 13
Visitor 74 booked room 8
Visitor 19 left from 14
Visitor 20 left from 14
Visitor 75 booked room 9
Visitor 21 left from 15
Visitor 76 booked room 15
Visitor 77 booked room 10
Visitor 78 booked room 10
Visitor 22 left from 15
Visitor 79 booked room 15
Visitor 23 left from 16
Visitor 80 booked room 16
Visitor 24 left from 16Visitor 81 booked room 16

Visitor 82 booked room 12
Visitor 25 left from 17
Visitor 83 booked room 12
Visitor 84 booked room 17
Visitor 26 left from 17
Visitor 85 booked room 17
Visitor 27 left from 18
Visitor 86 booked room 18
Visitor 28 left from 18
Visitor 87 booked room 18
Visitor 29 left from 19
Visitor 30 left from 19
Visitor 88 booked room 14
Visitor 31 left from 20
Visitor 89 booked room 14
Visitor 32 left from 20
Visitor 90 booked room 19
Visitor 33 left from 21
Visitor 91 booked room 19
Visitor 34 left from 21
Visitor 35 left from 22
Visitor 92 booked room 22
Visitor 36 left from 22
Visitor 37 left from 23
Visitor 38 left from 23
Visitor 93 booked room 22
Visitor 94 booked room 20
Visitor 39 left from 24
Visitor 95 booked room 20
Visitor 40 left from 24
Visitor 96 booked room 21
Visitor 97 booked room 21
Visitor 98 booked room 23
Visitor 99 booked room 23
Visitor 100 booked room 24
Visitor 101 booked room 24
Visitor 102 can not find room!
Visitor 103 can not find room!
Visitor 104 can not find room!
Visitor 105 can not find room!
Visitor 106 can not find room!
Visitor 107 can not find room!
Visitor 108 can not find room!
Visitor 109 can not find room!
Visitor 110 can not find room!
Visitor 111 can not find room!
Visitor 112 can not find room!
Visitor 113 can not find room!
Visitor 114 can not find room!
Visitor 115 can not find room!
Visitor 116 can not find room!
Visitor 117 can not find room!
Visitor 118 can not find room!
Visitor 119 can not find room!
Visitor Visitor 120 booked room 0
62 left from 0
Visitor 63 left from 1
Visitor 121 booked room 1
Visitor 122 can not find room!
Visitor 64 left from 2
Visitor 123 booked room 2
Visitor 124 can not find room!
Visitor 65 left from 3
Visitor 125 booked room 3
Visitor 66 left from 4
Visitor 126 booked room 4
Visitor 67 left from 5
Visitor 127 booked room 5
Visitor 128 can not find room!
Visitor 68 left from 11
Visitor 69 left from 6
Visitor 129 booked room 11
Visitor 70 left from 7
Visitor 71 left from 11
Visitor 130 booked room 11
Visitor 72 left from 13
Visitor 73 left from 13
Visitor 74 left from 8
Visitor 131 booked room 6
Visitor 132 booked room 7
Visitor 75 left from 9
Visitor 133 booked room 8
Visitor 76 left from 15
Visitor 77 left from 10
Visitor 78 left from 10
Visitor 134 booked room 15
Visitor 79 left from 15
Visitor 80 left from 16
Visitor 135 booked room 15
Visitor 81 left from 16
Visitor 136 booked room 9
Visitor 82 left from 12
Visitor 137 booked room 12
Visitor 83 left from 12
Visitor 84 left from 17
Visitor 85 left from 17
Visitor 138 booked room 12
Visitor 86 left from 18
Visitor 139 booked room 18
Visitor 87 left from 18
Visitor 140 booked room 18
Visitor 141 booked room 10
Visitor 142 booked room 10
Visitor 88 left from 14
Visitor 143 booked room 14
Visitor 144 booked room 13
Visitor 89 left from 14
Visitor 90 left from 19
Visitor 145 booked room 13
Visitor 91 left from 19
Visitor 146 booked room 14
Visitor 92 left from 22
Visitor 147 booked room 22
Visitor 148 booked room 16
Visitor 149 booked room 16
Visitor 93 left from 22
Visitor 94 left from 20
Visitor 150 booked room 20
Visitor 95 left from 20Visitor 151 booked room 20

Visitor 152 booked room 22
Visitor 96 left from 21
Visitor 153 booked room 21
Visitor 97 left from 21
Visitor 98 left from 23
Visitor 154 booked room 21
Visitor 99 left from 23
Visitor 100 left from 24
Visitor 155 booked room 24
Visitor 101 left from 24
Visitor 156 booked room 24
Visitor 157 booked room 17
Visitor 158 booked room 17
Visitor 159 booked room 19
Visitor 160 booked room 19
Visitor 161 booked room 23
Visitor 162 booked room 23
Visitor 163 can not find room!
Visitor 164 can not find room!
Visitor 165 can not find room!
Visitor 166 can not find room!
Visitor 167 can not find room!
Visitor 168 can not find room!
Visitor 169 can not find room!
Visitor 170 can not find room!
Visitor 171 can not find room!
Visitor 172 can not find room!
Visitor 173 can not find room!
Visitor 120 left from 0
Visitor 174 booked room 0
Visitor 175 can not find room!
Visitor 121 left from 1
Visitor 123 left from 2
Visitor 176 booked room 1
Visitor 177 booked room 2
Visitor 125 left from 3
Visitor 178 booked room 3
Visitor 126 left from 4
Visitor 179 booked room 4
Visitor 127 left from 5
Visitor 180 booked room 5
Visitor 129 left from 11
Visitor 181 booked room 11
Visitor 182 can not find room!
Visitor 130 left from 11
Visitor 183 booked room 11
Visitor 131 left from 6
Visitor 184 booked room 6
Visitor 132 left from 7
Visitor 185 booked room 7
Visitor 186 can not find room!
Visitor 133 left from 8
Visitor 187 booked room 8
Visitor 188 can not find room!
Visitor 134 left from 15
Visitor 189 booked room 15
Visitor 190 can not find room!
Visitor 135 left from 15
Visitor 191 booked room 15
Visitor 136 left from 9
Visitor 137 left from 12
Visitor 192 booked room 12
Visitor 193 booked room 9
Visitor 194 can not find room!
Visitor 195 can not find room!
Visitor 138 left from 12
Visitor 139 left from Visitor 196 booked room 12
18
Visitor 197 booked room 18
Visitor 198 can not find room!
Visitor 140 left from 18
Visitor 199 booked room 18
Visitor 141 left from 10
Visitor 142 left from 10
Visitor 200 booked room 10
Visitor 143 left from 14
Visitor 144 left from 13
Visitor 145 left from 13
Visitor 146 left from 14
Visitor 147 left from 22
Visitor 148 left from 16
Visitor 149 left from 16
Visitor 150 left from 20
Visitor 151 left from 20
Visitor 152 left from 22
Visitor 153 left from 21
Visitor 154 left from 21
Visitor 155 left from 24
Visitor 156 left from 24
Visitor 157 left from 17
Visitor 158 left from 17
Visitor 159 left from 19
Visitor 160 left from 19
Visitor 161 left from 23
Visitor 162 left from 23
Visitor 174 left from 0
Visitor 176 left from 1
Visitor 177 left from 2
Visitor 178 left from 3
Visitor 179 left from 4
Visitor 180 left from 5
Visitor 181 left from 11
Visitor 183 left from 11
Visitor 184 left from 6
Visitor 185 left from 7
Visitor 187 left from 8
Visitor 189 left from 15
Visitor 191 left from 15
Visitor 192 left from 12
Visitor 193 left from 9
Visitor 196 left from 12
Visitor 197 left from 18
Visitor 199 left from 18
Visitor 200 left from 10
```

## Задания на 8 баллов:

### Изменяю программу так, чтобы все входный данные случайно генерировались:

```c
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

int main() {
    // сбрасываю семя рандома
    srand(time(NULL));
    int n = rand() % 10000 + 1;
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
```

### Пример входных/выходных данных:

Входные данные отсутствуют, на выходе:

```
Visitor 1 booked room 0
Visitor 2 booked room 1
Visitor 3 booked room 2
Visitor 4 booked room 3
Visitor 5 booked room 4
Visitor 6 booked room 5
Visitor 7 booked room 6
Visitor 8 booked room 7
Visitor 9 booked room 8
Visitor 10 booked room 9
Visitor 11 booked room 10
Visitor 12 booked room 10
Visitor 13 booked room 11
Visitor 14 booked room 11
Visitor 15 booked room 12
Visitor 16 booked room 12
Visitor 17 booked room 13
Visitor 18 booked room 13
Visitor 19 booked room 14
Visitor 20 booked room 14
Visitor 21 booked room 15
Visitor 22 booked room 15
Visitor 23 booked room 16
Visitor 24 booked room 16
Visitor 25 booked room 17
Visitor 26 booked room 17
Visitor 27 booked room 18
Visitor 28 booked room 18
Visitor 29 booked room 19
Visitor 30 booked room 19
Visitor 31 booked room 20
Visitor 32 booked room 20
Visitor 33 booked room 21
Visitor 34 booked room 21
Visitor 35 booked room 22
Visitor 36 booked room 22
Visitor 37 booked room 23
Visitor 38 booked room 23
Visitor 39 booked room 24
Visitor 40 booked room 24
Visitor 41 can not find room!
Visitor 42 can not find room!
Visitor 43 can not find room!
Visitor 44 can not find room!
Visitor 45 can not find room!
Visitor 46 can not find room!
Visitor 47 can not find room!
Visitor 48 can not find room!
Visitor 49 can not find room!
Visitor 50 can not find room!
Visitor 51 can not find room!
Visitor 52 can not find room!
Visitor 53 can not find room!
Visitor 54 can not find room!
Visitor 55 can not find room!
Visitor 56 can not find room!
Visitor 57 can not find room!
Visitor 58 can not find room!
Visitor 1 left from 0
Visitor 2 left from 1
Visitor 59 booked room 0
Visitor 60 booked room 1
Visitor 3 left from 2
Visitor 61 booked room 2
Visitor 4 left from 3
Visitor 62 booked room 3
Visitor 5 left from 4
Visitor 6 left from 5
Visitor 63 booked room 4
Visitor 7 left from 6
Visitor 64 booked room 5
Visitor 8 left from 7
Visitor 9 left from 8
Visitor 65 booked room 6
Visitor 10 left from 9
Visitor 66 booked room 7
Visitor 11 left from 10
Visitor 67 booked room 10
Visitor 12 left from 10
Visitor 68 booked room 10
Visitor Visitor 69 booked room 11
13 left from 11
Visitor 14 left from 11
Visitor 70 booked room 11
Visitor 71 booked room 8
Visitor 72 booked room 9
Visitor 15 left from 12
Visitor 73 booked room 12
Visitor 16 left from 12
Visitor 17 left from 13
Visitor 74 booked room 12
Visitor 18 left from 13
Visitor 75 booked room 13
Visitor 76 booked room 13
Visitor 19 left from 14
Visitor 77 booked room 14
Visitor 20 left from 14
Visitor 78 booked room 14
Visitor 21 left from 15
Visitor 79 booked room 15
Visitor 22 left from 15
Visitor 23 left from 16
Visitor 24 left from 16
Visitor 80 booked room 15
Visitor 81 booked room 16
Visitor 25 left from 17
Visitor 82 booked room 16
Visitor 26 left from 17
Visitor 83 booked room 17
Visitor 84 booked room 17
Visitor 27 left from 18
Visitor 85 booked room 18
Visitor 28 left from 18
Visitor 29 left from 19
Visitor 86 booked room 18
Visitor 30 left from 19
Visitor 31 left from 20
Visitor 87 booked room 20
Visitor 88 booked room 19
Visitor 32 left from 20
Visitor 33 left from 21
Visitor 89 booked room 19
Visitor 90 booked room 20
Visitor 34 left from 21
Visitor 91 booked room 21
Visitor 35 left from 22
Visitor 92 booked room 21
Visitor 36 left from 22
Visitor 93 booked room 22
Visitor 37 left from 23
Visitor 94 booked room 22
Visitor 38 left from 23
Visitor 95 booked room 23
Visitor 39 left from 24
Visitor 40 left from 24
Visitor 96 booked room 23
Visitor 97 booked room 24
Visitor 98 booked room 24
Visitor 99 can not find room!
Visitor 100 can not find room!
Visitor 101 can not find room!
Visitor 102 can not find room!
Visitor 103 can not find room!
Visitor 104 can not find room!
Visitor 105 can not find room!
Visitor 106 can not find room!
Visitor 107 can not find room!
Visitor 108 can not find room!
Visitor 109 can not find room!
Visitor 110 can not find room!
Visitor 111 can not find room!
Visitor 112 can not find room!
Visitor 59 left from 0
Visitor 113 booked room 0
Visitor 60 left from 1
Visitor 114 booked room 1
Visitor 115 can not find room!
Visitor 61 left from 2
Visitor 116 booked room 2
Visitor 117 can not find room!
Visitor 62 left from 3
Visitor 118 booked room 3
Visitor 63 left from 4
Visitor 119 booked room 4
Visitor 120 can not find room!
Visitor 64 left from 5
Visitor 121 booked room 5
Visitor 65 left from 6
Visitor 122 booked room 6
Visitor 66 left from 7
Visitor 123 booked room 7
Visitor 67 left from 10
Visitor 124 booked room 10
Visitor 68 left from 10
Visitor 125 booked room 10
Visitor 69 left from 11
Visitor 126 booked room 11
Visitor 127 can not find room!
Visitor 70 left from 11
Visitor 71 left from 8
Visitor 128 booked room 11
Visitor 72 left from 9
Visitor 129 booked room 8
Visitor 73 left from 12
Visitor 130 booked room 12
Visitor 131 booked room 9
Visitor 74 left from 12
Visitor 75 left from 13
Visitor 76Visitor 132 booked room 12
 left from 13
Visitor 133 booked room 13
Visitor 77 left from 14
Visitor 134 booked room 13
Visitor 78 left from 14
Visitor 135 booked room 14
Visitor 79 left from 15
Visitor 136 booked room 14
Visitor 80 left from 15
Visitor Visitor 81 left from 137 booked room 16
16
Visitor 82 left from 16
Visitor 138 booked room 16
Visitor 139 booked room 15
Visitor 83 left from 17
Visitor 84 left from 17
Visitor 140 booked room 15
Visitor 141 booked room 17
Visitor 85 left from 18
Visitor 142 booked room 17
Visitor 143 booked room 18
Visitor 86 left from 18
Visitor 144 booked room 18
Visitor 87 left from 20
Visitor 88 left from 19
Visitor 145 booked room 19
Visitor 146 booked room 20
Visitor 89 left from 19
Visitor Visitor 147 booked room 19
90 left from 20
Visitor 91 left from 21
Visitor 148 booked room 20
Visitor 92 left from 21
Visitor 149 booked room 21
Visitor 93 left from 22
Visitor 150 booked room 21
Visitor 94 left from 22
Visitor 151 booked room 22
Visitor 95 left from 23
Visitor 152 booked room 22
Visitor 96 left from 23
Visitor 153 booked room 23
Visitor 154 booked room 23
Visitor 97 left from 24
Visitor 155 booked room 24
Visitor 98 left from 24
Visitor 156 booked room 24
Visitor 157 can not find room!
Visitor 158 can not find room!
Visitor 159 can not find room!
Visitor 160 can not find room!
Visitor 161 can not find room!
Visitor 162 can not find room!
Visitor 163 can not find room!
Visitor 164 can not find room!
Visitor 165 can not find room!
Visitor 166 can not find room!
Visitor 167 can not find room!
Visitor 168 can not find room!
Visitor 169 can not find room!
Visitor 170 can not find room!
Visitor 171 can not find room!
Visitor 172 can not find room!
Visitor 113 left from 0
Visitor 114 left from 1
Visitor 116 left from 2
Visitor 118 left from 3
Visitor 119 left from 4
Visitor 121 left from 5
Visitor 122 left from 6
Visitor 123 left from 7
Visitor 124 left from 10
Visitor 125 left from 10
Visitor 126 left from 11
Visitor 128 left from 11
Visitor 129 left from 8
Visitor 130 left from 12
Visitor 131 left from 9
Visitor 132 left from 12
Visitor 133 left from 13
Visitor 134 left from 13
Visitor 135 left from 14
Visitor 136 left from 14
Visitor 137 left from 16
Visitor 139 left from 15
Visitor 138 left from 16
Visitor 140 left from 15
Visitor 141 left from 17
Visitor 142 left from 17
Visitor 143 left from 18
Visitor 144 left from 18
Visitor 145 left from 19
Visitor 146 left from 20
Visitor 147 left from 19
Visitor 148 left from 20
Visitor 149 left from 21
Visitor 150 left from 21
Visitor 151 left from 22
Visitor 152 left from 22
Visitor 153 left from 23
Visitor 154 left from 23
Visitor 155 left from 24
Visitor 156 left from 24
```