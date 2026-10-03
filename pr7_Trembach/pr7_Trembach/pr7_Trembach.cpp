// pr7_Trembach.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#define DEBUG_MODE

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
using namespace std;

struct Car {
    string number;
    string type;
    string timeIn;
    int hours;
    double pricePerHour;

    double totalCost()
    {
        return hours * pricePerHour;
    }
};

void checkTime(const string& time)
{
    if (time.length() != 5)
        throw invalid_argument("Неверный формат времени, ожидается ЧЧ:ММ: " + time);

    if (time[2] != ':')
        throw invalid_argument("Неверный формат времени, нет ':' в позиции 3: " + time);

    for (int i = 0; i < 5; i++)
    {
        if (i == 2) continue;
        if (time[i] < '0' || time[i] > '9')
            throw invalid_argument("Время должно содержать только цифры: " + time);
    }

    int hours = stoi(time.substr(0, 2));
    int minutes = stoi(time.substr(3, 2));

    if (hours < 0 || hours > 23)
        throw out_of_range("Часы должны быть от 00 до 23: " + time);
    if (minutes < 0 || minutes > 59)
        throw out_of_range("Минуты должны быть от 00 до 59: " + time);
}

Car LineToRecord(string line)
{
    if (line.empty())
        throw runtime_error("Строка пуста");

    string data[5];
    for (int i = 0; i < 5; i++)
    {
        size_t pos = line.find(";");

        if (pos == string::npos && i == 4)
        {
            data[i] = line;
            break;
        }
        else if (pos == string::npos)
        {
            throw logic_error("Недостаточно данных в строке");
        }

        data[i] = line.substr(0, pos);

        if (data[i].empty())
            throw invalid_argument("Значение в строке пустое");

        line.erase(0, pos + 1);
    }

    Car car;

    car.number = data[0];
    if (car.number.empty())
        throw invalid_argument("Пустой госномер");

    car.type = data[1];
    if (car.type.empty())
        throw invalid_argument("Пустой тип транспорта");

    car.timeIn = data[2];
    if (car.timeIn.empty())
        throw invalid_argument("Пустое время въезда");
    checkTime(car.timeIn);

    try
    {
        car.hours = stoi(data[3]);
    }
    catch (exception& e)
    {
        throw invalid_argument("Не удалось преобразовать часы в число");
    }
    if (car.hours <= 0)
        throw out_of_range("Часов должно быть больше 0");

    try
    {
        car.pricePerHour = stod(data[4]);
    }
    catch (exception& e)
    {
        throw invalid_argument("Не удалось преобразовать стоимость в число");
    }
    if (car.pricePerHour <= 0)
        throw out_of_range("Стоимость часа должна быть больше 0");

    return car;
}

int LoadCars(const string& filename, Car cars[])
{
    ifstream file(filename);
    if (!file.is_open())
        throw runtime_error("Не удалось открыть файл: " + filename);

    int count = 0;
    string line;
    while (getline(file, line))
    {
        if (line.empty()) continue;

        Car car = LineToRecord(line);

        for (int i = 0; i < count; i++)
        {
            if (cars[i].number == car.number)
                throw invalid_argument("Дубликат номера: " + car.number);
        }

        cars[count++] = car;
    }
    file.close();

#ifdef DEBUG_MODE
    cout << "[DEBUG] Загружено записей: " << count << endl;
#endif

    return count;
}

void PrintCars(Car cars[], int count) {
    cout << "\nМашины на парковке:" << endl;
    for (int i = 0; i < count; i++)
    {
        cout << cars[i].number << " | " << cars[i].type << " | " << cars[i].timeIn << " | " << cars[i].hours << " ч" << " | " << cars[i].pricePerHour << " руб/ч" << " | Стоимость парковки: " << cars[i].totalCost() << endl;
    }
}

int FindCar(Car cars[], int count, const string& number)
{
    for (int i = 0; i < count; i++)
        if (cars[i].number == number)
            return i;
    return -1;
}

void SaveCars(const string& filename, Car cars[], int count)
{
    ofstream file2(filename);
    if (!file2.is_open())
        throw runtime_error("Не удалось открыть файл: " + filename);

    for (int i = 0; i < count; i++) {
        file2 << cars[i].number << ";" << cars[i].type << ";" << cars[i].timeIn << ";" << cars[i].hours << ";" << cars[i].pricePerHour << "\n";
    }
    file2.close();

#ifdef DEBUG_MODE
    cout << "[DEBUG] Сохранено записей: " << count << endl;
#endif
}

int main() {
    setlocale(NULL, "Russian");

    Car cars[100];
    int count = 0;

    try
    {
        count = LoadCars("parking.txt", cars);
    }
    catch (exception& e) {
        cout << "Ошибка загрузки: " << e.what() << endl;
        return 1;
    }

    PrintCars(cars, count);

    try
    {
        string number;
        cout << "\nВведите номер для поиска: ";
        cin >> number;

        int idx = FindCar(cars, count, number);
        if (idx == -1)
            throw runtime_error("Автомобиль не найден");

        cout << "Найден: " << cars[idx].number << ", стоимость: " << cars[idx].totalCost() << endl;
    }
    catch (exception& e) {
        cout << "Ошибка: " << e.what() << endl;
    }

    double totalRevenue = 0;
    try
    {
        string number;
        cout << "\nВведите номер для выезда: ";
        cin >> number;

        int idx = FindCar(cars, count, number);
        if (idx == -1)
            throw runtime_error("Автомобиль не найден");

        cout << "Выезд " << cars[idx].number << ", к оплате: " << cars[idx].totalCost() << endl;

        totalRevenue += cars[idx].totalCost();

        for (int i = idx; i < count - 1; i++)
            cars[i] = cars[i + 1];
        count--;

    }
    catch (exception& e) {
        cout << "Ошибка: " << e.what() << endl;
    }

    double currentTotal = 0;
    for (int i = 0; i < count; i++)
        currentTotal += cars[i].totalCost();

    cout << "\nВыручка с выехавших: " << totalRevenue << endl;
    cout << "Потенциальная с оставшихся: " << currentTotal << endl;
    cout << "Всего: " << totalRevenue + currentTotal << endl;

    try
    {
        SaveCars("parking_out.txt", cars, count);
        cout << "\nСохранено в parking_out.txt" << endl;
    }
    catch (exception& e) {
        cout << "Ошибка сохранения: " << e.what() << endl;
    }

    PrintCars(cars, count);

    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
