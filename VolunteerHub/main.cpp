#include <iostream>
#include <fstream>
#include <clocale>
#include <windows.h> 

using namespace std;

// ============================================================================
// ЭТАП 1. СТРУКТУРЫ И ГЛОБАЛЬНЫЕ МАССИВЫ
// ============================================================================

const int MAX_REQUESTS = 100;
const int MAX_VOLUNTEERS = 100;

struct Request {
    int id;
    char description[100];
    char category[30];
    int urgency;
    int requiredHours;
    bool isClosed;
};

struct Volunteer {
    int id;
    char name[50];
    char skill[30];
    int bloodType;
    int totalHours;
};

Request requests[MAX_REQUESTS];
int requestCount = 0;

Volunteer volunteers[MAX_VOLUNTEERS];
int volunteerCount = 0;

// ============================================================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ (ЗАЩИТА ВВОДA И СРАВНЕНИЕ СТРОК)
// ============================================================================

int getSafeIntInput() {
    int value;
    while (true) {
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << " [!] Ошибка! Введите корректное целое число: ";
        }
        else {
            cin.ignore(10000, '\n');
            return value;
        }
    }
}

bool areStringsEqual(const char* str1, const char* str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) return false;
        i++;
    }
    return str1[i] == str2[i];
}

// ============================================================================
// ЭТАП 2. ЗАГРУЗКА И СОХРАНЕНИЕ ФАЙЛОВ
// ============================================================================

void saveRequests() {
    ofstream file("requests.txt");
    if (file.is_open()) {
        file << requestCount << "\n";
        for (int i = 0; i < requestCount; ++i) {
            file << requests[i].id << "\n"
                << requests[i].description << "\n"
                << requests[i].category << "\n"
                << requests[i].urgency << "\n"
                << requests[i].requiredHours << "\n"
                << requests[i].isClosed << "\n";
        }
        file.close();
    }
}

void loadRequests() {
    ifstream file("requests.txt");
    if (file.is_open()) {
        file >> requestCount;
        file.ignore();
        for (int i = 0; i < requestCount; ++i) {
            file >> requests[i].id;
            file.ignore();
            file.getline(requests[i].description, 100);
            file.getline(requests[i].category, 30);
            file >> requests[i].urgency;
            file >> requests[i].requiredHours;
            file >> requests[i].isClosed;
            file.ignore();
        }
        file.close();
    }
}

void saveVolunteers() {
    ofstream file("volunteers.txt");
    if (file.is_open()) {
        file << volunteerCount << "\n";
        for (int i = 0; i < volunteerCount; ++i) {
            file << volunteers[i].id << "\n"
                << volunteers[i].name << "\n"
                << volunteers[i].skill << "\n"
                << volunteers[i].bloodType << "\n"
                << volunteers[i].totalHours << "\n";
        }
        file.close();
    }
}

void loadVolunteers() {
    ifstream file("volunteers.txt");
    if (file.is_open()) {
        file >> volunteerCount;
        file.ignore();
        for (int i = 0; i < volunteerCount; ++i) {
            file >> volunteers[i].id;
            file.ignore();
            file.getline(volunteers[i].name, 50);
            file.getline(volunteers[i].skill, 30);
            file >> volunteers[i].bloodType;
            file >> volunteers[i].totalHours;
            file.ignore();
        }
        file.close();
    }
}

// ============================================================================
// ЭТАП 4. РЕГИСТРАЦИЯ ЗАЯВОК И ВОЛОНТЕРОВ (С ФИЛЬТРАЦИЕЙ МУСОРА)
// ============================================================================

void registerEntity() {
    cout << "\n=========================================\n";
    cout << "         ОКНО РЕГИСТРАЦИИ ДАННЫХ\n";
    cout << "=========================================\n";
    cout << "1. Зарегистрировать нового волонтера\n";
    cout << "2. Добавить новую заявку на помощь\n";
    cout << "Выберите действие (1-2): ";
    int choice = getSafeIntInput();

    if (choice == 1) {
        if (volunteerCount >= MAX_VOLUNTEERS) {
            cout << " [!] База волонтеров штаба переполнена!\n";
            return;
        }
        Volunteer v;
        v.id = (volunteerCount == 0) ? 101 : volunteers[volunteerCount - 1].id + 1;

        cout << "Введите ФИО волонтера (через_подчеркивание, например Ivan_Ivanov): ";
        cin >> v.name;

        cout << "Введите навык/специализацию (Медицина, Поиск, Еда, Ремонт): ";
        cin >> v.skill;

        // Фильтр группы крови (строго от 1 до 4)
        cout << "Введите группу крови (1-4): ";
        v.bloodType = getSafeIntInput();
        while (v.bloodType < 1 || v.bloodType > 4) {
            cout << " [!] Ошибка фильтрации: Группа крови должна быть от 1 до 4. Повторите ввод: ";
            v.bloodType = getSafeIntInput();
        }

        v.totalHours = 0;
        volunteers[volunteerCount] = v;
        volunteerCount++;
        cout << " -> Успешно! Волонтер зарегистрирован с ID: " << v.id << "\n";
    }
    else if (choice == 2) {
        if (requestCount >= MAX_REQUESTS) {
            cout << " [!] База заявок штаба переполнена!\n";
            return;
        }
        Request r;
        r.id = (requestCount == 0) ? 1 : requests[requestCount - 1].id + 1;

        cout << "Введите описание проблемы (через_подчеркивание): ";
        cin >> r.description;

        cout << "Введите категорию помощи (Медицина, Поиск, Еда, Ремонт): ";
        cin >> r.category;

        // Фильтр уровня критичности (строго от 1 до 3)
        cout << "Уровень критичности (1 - Низкий, 2 - Средняя, 3 - ЧС): ";
        r.urgency = getSafeIntInput();
        while (r.urgency < 1 || r.urgency > 3) {
            cout << " [!] Ошибка фильтрации: Уровень ЧС может быть только от 1 до 3. Повторите ввод: ";
            r.urgency = getSafeIntInput();
        }

        // Фильтр адекватности рабочих часов (от 1 до 24 часов за раз)
        cout << "Необходимое количество часов работы (1-24): ";
        r.requiredHours = getSafeIntInput();
        while (r.requiredHours <= 0 || r.requiredHours > 24) {
            cout << " [!] Ошибка фильтрации: Часы задачи должны быть в диапазоне от 1 до 24. Повторите ввод: ";
            r.requiredHours = getSafeIntInput();
        }

        r.isClosed = false;
        requests[requestCount] = r;
        requestCount++;
        cout << " -> Успешно! Заявка добавлена с ID: " << r.id << "\n";
    }
    saveRequests();
    saveVolunteers();
}

// ============================================================================
// ЭТАП 5. ЭКСТРЕННЫЙ МЭТЧИНГ (ПОИСК КАДРОВ ПО КАТЕГОРИИ)
// ============================================================================

void emergencyMatching() {
    cout << "\n=========================================\n";
    cout << "       АЛГОРИТМ ЭКСТРЕННОГО МЭТЧИНГА\n";
    cout << "=========================================\n";
    cout << "Введите ID заявки для подбора команды: ";
    int targetId = getSafeIntInput();

    int foundIdx = -1;
    for (int i = 0; i < requestCount; ++i) {
        if (requests[i].id == targetId) {
            foundIdx = i;
            break;
        }
    }

    if (foundIdx == -1) {
        cout << " [!] Заявка с таким ID не найдена.\n";
        return;
    }

    cout << " Анализ задачи: [" << requests[foundIdx].description << "]\n";
    cout << " Требуемый навык: " << requests[foundIdx].category << "\n";
    cout << "-----------------------------------------\n";
    cout << "| ID  | Подходящий волонтер  | Навык    |\n";
    cout << "-----------------------------------------\n";

    bool anyFound = false;
    for (int i = 0; i < volunteerCount; ++i) {
        if (areStringsEqual(volunteers[i].skill, requests[foundIdx].category)) {
            cout << "| " << volunteers[i].id << " | "
                << volunteers[i].name << " \t| "
                << volunteers[i].skill << " |\n";
            anyFound = true;
        }
    }
    if (!anyFound) {
        cout << "|      Свободные специалисты не найдены |\n";
    }
    cout << "-----------------------------------------\n";
}

// ============================================================================
// ЭТАП 6. ФИКСАЦИЯ ПОМОЩИ И НАЧИСЛЕНИЕ ЧАСОВ
// ============================================================================

void closeRequestAndReward() {
    cout << "\n=========================================\n";
    cout << "          ФИКСАЦИЯ ВЫПОЛНЕНИЯ ПОМОЩИ\n";
    cout << "=========================================\n";
    cout << "Введите ID закрываемой заявки: ";
    int reqId = getSafeIntInput();

    int reqIdx = -1;
    for (int i = 0; i < requestCount; ++i) {
        if (requests[i].id == reqId) {
            reqIdx = i;
            break;
        }
    }

    if (reqIdx == -1 || requests[reqIdx].isClosed) {
        cout << " [!] Заявка не найдена или уже была закрыта.\n";
        return;
    }

    cout << "Введите ID исполнителя (волонтера): ";
    int volId = getSafeIntInput();

    int volIdx = -1;
    for (int i = 0; i < volunteerCount; ++i) {
        if (volunteers[i].id == volId) {
            volIdx = i;
            break;
        }
    }

    if (volIdx == -1) {
        cout << " [!] Волонтер с таким ID не найден.\n";
        return;
    }

    requests[reqIdx].isClosed = true;
    volunteers[volIdx].totalHours += requests[reqIdx].requiredHours;

    cout << " -> Статус изменен! Волонтеру " << volunteers[volIdx].name
        << " успешно начислено " << requests[reqIdx].requiredHours << " ч.\n";
    saveRequests();
    saveVolunteers();
}

// ============================================================================
// ЭТАП 7. ПУЗЫРЬКОВАЯ СОРТИРОВКА ПО КРИТИЧНОСТИ
// ============================================================================

void printSortedRequests() {
    cout << "\n=========================================================================\n";
    cout << "            АКТУАЛЬНЫЕ ЗАЯВКИ (СОРТИРОВКА ПО УРОВНЮ ЧС)\n";
    cout << "=========================================================================\n";

    if (requestCount == 0) {
        cout << " База данных заявок пуста.\n";
        return;
    }

    Request temp[MAX_REQUESTS];
    for (int i = 0; i < requestCount; ++i) {
        temp[i] = requests[i];
    }

    for (int i = 0; i < requestCount - 1; ++i) {
        for (int j = 0; j < requestCount - i - 1; ++j) {
            if (temp[j].urgency < temp[j + 1].urgency) {
                Request swapHolder = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = swapHolder;
            }
        }
    }

    cout << "-------------------------------------------------------------------------\n";
    cout << "| ID  | КРИТ. | КАТЕГОРИЯ       | ЧАСЫ | СТАТУС     | ОПИСАНИЕ          |\n";
    cout << "-------------------------------------------------------------------------\n";
    for (int i = 0; i < requestCount; ++i) {
        cout << "| " << temp[i].id << "   |   "
            << temp[i].urgency << "   | "
            << temp[i].category << " \t| "
            << temp[i].requiredHours << "    | "
            << (temp[i].isClosed ? "ЗАКРЫТА " : "ОТКРЫТА ") << " | "
            << temp[i].description << "\n";
    }
    cout << "-------------------------------------------------------------------------\n";
}

// ============================================================================
// ЭТАП 8. АНАЛИТИКА ЭФФЕКТИВНОСТИ ШТАБА
// ============================================================================

void showAnalytics() {
    cout << "\n=========================================\n";
    cout << "      АНАЛИТИКА ЭФФЕКТИВНОСТИ ШТАБА\n";
    cout << "=========================================\n";

    if (requestCount == 0) {
        cout << " Нет данных для анализа.\n";
        return;
    }

    int closedCount = 0;
    int activeCount = 0;
    int totalVolHours = 0;

    for (int i = 0; i < requestCount; ++i) {
        if (requests[i].isClosed) closedCount++;
        else activeCount++;
    }

    for (int i = 0; i < volunteerCount; ++i) {
        totalVolHours += volunteers[i].totalHours;
    }

    double closedPercent = ((double)closedCount / requestCount) * 100.0;

    cout << " Активных (открытых) заявок в ЧС: " << activeCount << "\n";
    cout << " Успешно закрытых задач населения: " << closedCount << "\n";
    cout << " Процент выполнения задач штаба:   " << closedPercent << " %\n";
    cout << " Суммарный вклад работы волонтеров: " << totalVolHours << " человеко-часов\n";
    cout << "=========================================\n";
}

// ============================================================================
// ЭТАП 9. ЭКСПОРТ КРАСИВОГО REPORT.TXT
// ============================================================================

void exportReport() {
    ofstream report("report.txt");
    if (!report.is_open()) {
        cout << " [!] Не удалось создать файл отчета.\n";
        return;
    }

    report << "=========================================================\n";
    report << "                     VOLUNTEER HUB                       \n";
    report << "               ОФИЦИАЛЬНЫЙ ОТЧЕТ КООРДИНАТОРА             \n";
    report << "=========================================================\n\n";

    report << "ЖУРНАЛ СТАТУСА ВСЕХ ЗАЯВОК НА ПОМОЩЬ:\n";
    report << "---------------------------------------------------------\n";
    report << "| ID  | CATEGORY        | URGENCY | STATUS    | HOURS   |\n";
    report << "---------------------------------------------------------\n";
    for (int i = 0; i < requestCount; ++i) {
        report << "| " << requests[i].id << "   | "
            << requests[i].category << " \t|   "
            << requests[i].urgency << "     | "
            << (requests[i].isClosed ? "CLOSED " : "OPEN   ") << " | "
            << requests[i].requiredHours << " \t|\n";
    }
    report << "---------------------------------------------------------\n";

    report.close();
    cout << " -> Успешно! Сводная таблица сохранена в файл 'report.txt'.\n";
}

// ============================================================================
// ДОПОЛНИТЕЛЬНО: ИНСТРУКЦИЯ ПОЛЬЗОВАТЕЛЯ
// ============================================================================

void showInstructions() {
    cout << "\n=========================================================\n";
    cout << "          ИНСТРУКЦИЯ ПОЛЬЗОВАТЕЛЯ VOLUNTEER HUB          \n";
    cout << "=========================================================\n";
    cout << " Добро пожаловать в систему координации штаба ЧС!\n\n";

    cout << " 1. Правила ввода текстовых данных:\n";
    cout << "    ВАЖНО: Вводите любые имена, фамилии и описания задач\n";
    cout << "    СТРОГО через нижнее подчеркивание (например: Ivan_Ivanov).\n";
    cout << "    Пробелы использовать нельзя — это ограничение потока cin.\n\n";

    cout << " 2. Система умной фильтрации мусора:\n";
    cout << "    Приложение автоматически отклоняет некорректные данные.\n";
    cout << "    Группа крови должна быть от 1 до 4, уровень ЧС от 1 до 3,\n";
    cout << "    а время одной задачи — не более 24 часов.\n\n";

    cout << " 3. Фиксация выполненной работы:\n";
    cout << "    Когда задача выполнена, выберите пункт 3. Введите ID задачи\n";
    cout << "    и ID волонтера. Задача закроется, а волонтеру автоматически\n";
    cout << "    начислятся рабочие часы.\n\n";

    cout << " 4. Сохранение данных:\n";
    cout << "    База данных сохраняется автоматически при нажатии '0'\n";
    cout << "    и корректном выходе из программы.\n";
    cout << "=========================================================\n";
    cout << "Нажмите '1' и Enter для возврата в главное меню: ";
    int back = getSafeIntInput();
}

// ============================================================================
// ГЛАВНЫЙ ЦИКЛ ПРОГРАММЫ И МЕНЮ SWITCH
// ============================================================================

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    setlocale(LC_ALL, ".UTF8");

    loadRequests();
    loadVolunteers();

    int choice = -1;
    while (choice != 0) {
        cout << "\n=========================================\n";
        cout << "          VOLUNTEER HUB SYSTEM           \n";
        cout << "=========================================\n";
        cout << "1. Регистрация волонтеров и заявок ЧС\n";
        cout << "2. Экстренный мэтчинг (Поиск кадров)\n";
        cout << "3. Фиксация помощи и начисление часов\n";
        cout << "4. Показать заявки (Сортировка по ЧС)\n";
        cout << "5. Вывести аналитику эффективности штаба\n";
        cout << "6. Экспорт официального отчета в файл\n";
        cout << "7. Инструкция по работе со штабом\n";
        cout << "0. Сохранить изменения и выйти\n";
        cout << "=========================================\n";
        cout << "Введите ваш выбор: ";

        choice = getSafeIntInput();

        switch (choice) {
        case 1: registerEntity(); break;
        case 2: emergencyMatching(); break;
        case 3: closeRequestAndReward(); break;
        case 4: printSortedRequests(); break;
        case 5: showAnalytics(); break;
        case 6: exportReport(); break;
        case 7: showInstructions(); break;
        case 0:
            saveRequests();
            saveVolunteers();
            cout << "\n[!] Все базы сохранены. Завершение работы штаба.\n";
            break;
        default:
            cout << " [!] Неверный пункт меню. Повторите попытку.\n";
        }
    }
    return 0;
}