#include <iostream>
#include <fstream>
#include <clocale>
#include <windows.h> 
#include <iomanip> 

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

bool isAdmin = false;

// ============================================================================
// МОДУЛЬ: ФУНКЦИИ ДЛЯ РАБОТЫ С ЦВЕТОМ КОНСОЛИ
// ============================================================================

void setColor(int colorCode) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, colorCode);
}

void colorReset() { setColor(7); }
void colorRed() { setColor(12); }
void colorGreen() { setColor(10); }
void colorYellow() { setColor(14); }
void colorCyan() { setColor(11); }
void colorWhite() { setColor(15); }

// ============================================================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ (ЗАЩИТА ВВОДA, СРАВНЕНИЕ СТРОК, НАДЕЖНАЯ ПАУЗА)
// ============================================================================

int getSafeIntInput() {
    int value;
    while (true) {
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            colorRed();
            cout << " [!] Ошибка! Введите корректное целое число: ";
            colorReset();
        }
        else {
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

// ИСПРАВЛЕННАЯ ФУНКЦИЯ: без ошибки C2679 / E0349
void getValidCategory(char* targetArray) {
    char buffer[30];
    while (true) {
        cin >> buffer;
        if (areStringsEqual(buffer, "Медицина") ||
            areStringsEqual(buffer, "Поиск") ||
            areStringsEqual(buffer, "Еда") ||
            areStringsEqual(buffer, "Ремонт")) {

            int i = 0;
            while (buffer[i] != '\0' && i < 29) {
                targetArray[i] = buffer[i];
                i++;
            }
            targetArray[i] = '\0';
            break;
        }
        colorRed();
        cout << " [!] Ошибка фильтрации: Неверная категория!\n";
        colorYellow();
        cout << " Разрешены СТРОГО: Медицина, Поиск, Еда, Ремонт\n";
        colorWhite();
        cout << " Повторите ввод: ";
        colorReset();
    }
}

void waitForEnter() {
    cout << "\nНажмите Enter, чтобы вернуться в меню...";
    cin.clear();
    cin.ignore(10000, '\n');
    cin.get();
}

// ============================================================================
// ОКНО ВХОДА (АВТОРИЗАЦИЯ С ЦВЕТНЫМ ИНТЕРФЕЙСОМ)
// ============================================================================

void loginSystem() {
    while (true) {
        system("cls");
        colorCyan();
        cout << "=========================================\n";
        cout << "      АВТОРИЗАЦИЯ В СИСТЕМЕ VOLUNTEER HUB\n";
        cout << "=========================================\n";
        colorReset();
        cout << "1. Войти как Координатор (Администратор)\n";
        cout << "2. Войти как Гость (Только просмотр)\n";
        colorYellow();
        cout << "0. Выйти из программы\n";
        cout << "=========================================\n";
        cout << "Выберите действие: ";
        colorReset();

        int choice = getSafeIntInput();

        if (choice == 1) {
            char username[20];
            char password[20];

            cout << "Введите логин: ";
            cin >> username;
            cout << "Введите пароль: ";
            cin >> password;

            if (areStringsEqual(username, "admin") && areStringsEqual(password, "1234")) {
                isAdmin = true;
                colorGreen();
                cout << "\n [ -> ] Доступ разрешен! Добро пожаловать, Координатор.\n";
                colorReset();
                waitForEnter();
                break; // Выходим из цикла авторизации и идем в главное меню
            }
            else {
                colorRed();
                cout << "\n [!] Ошибка: Неверный логин или пароль!\n";
                colorReset();
                waitForEnter();
                // Цикл повторится, гостя принудительно не включает
            }
        }
        else if (choice == 2) {
            isAdmin = false;
            colorYellow();
            cout << "\n [ -> ] Вход выполнен в режиме Гостя.\n";
            colorReset();
            waitForEnter();
            break; // Выходим из цикла авторизации и идем в главное меню как гость
        }
        else if (choice == 0) {
            colorYellow();
            cout << "\nВыход из программы. Всего доброго!\n";
            colorReset();
            exit(0); // Полное завершение работы приложения
        }
        else {
            colorRed();
            cout << "\n [!] Ошибка: Неверный выбор! Используйте только цифры 0, 1 или 2.\n";
            colorReset();
            waitForEnter();
        }
    }
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
// ЭТАП 4. РЕГИСТРАЦИЯ ЗАЯВОК И ВОЛОНТЕРОВ
// ============================================================================

void registerEntity() {
    system("cls");
    colorCyan();
    cout << "\n=========================================\n";
    cout << "         ОКНО РЕГИСТРАЦИИ ДАННЫХ\n";
    cout << "=========================================\n";
    colorReset();
    cout << "1. Зарегистрировать нового волонтера\n";
    cout << "2. Добавить новую заявку на помощь\n";
    colorYellow();
    cout << "Выберите действие (1-2): ";
    colorReset();
    int choice = getSafeIntInput();

    if (choice == 1) {
        if (volunteerCount >= MAX_VOLUNTEERS) {
            colorRed();
            cout << " [!] База волонтеров штаба переполнена!\n";
            colorReset();
            waitForEnter();
            return;
        }
        Volunteer v;
        v.id = (volunteerCount == 0) ? 1 : volunteers[volunteerCount - 1].id + 1;

        cout << "Введите ФИО волонтера (через подчеркивание, например Ivanov_Ivan): ";
        cin >> v.name;

        bool hasUnderscore = false;
        for (int i = 0; v.name[i] != '\0'; i++) {
            if (v.name[i] == '_') { hasUnderscore = true; break; }
        }
        while (!hasUnderscore) {
            colorRed();
            cout << " [!] Ошибка: Введите Фамилию и Имя через '_' (например, Petrov_Petr): ";
            colorReset();
            cin >> v.name;
            for (int i = 0; v.name[i] != '\0'; i++) {
                if (v.name[i] == '_') { hasUnderscore = true; break; }
            }
        }

        cout << "Введите навык (Медицина, Поиск, Еда, Ремонт): ";
        getValidCategory(v.skill);

        cout << "Введите группу крови (1-4): ";
        v.bloodType = getSafeIntInput();
        while (v.bloodType < 1 || v.bloodType > 4) {
            colorRed();
            cout << " [!] Ошибка: Группа крови должна быть от 1 до 4. Повторите ввод: ";
            colorReset();
            v.bloodType = getSafeIntInput();
        }

        v.totalHours = 0;
        volunteers[volunteerCount] = v;
        volunteerCount++;
        colorGreen();
        cout << " -> Успешно! Волонтер зарегистрирован с ID: " << v.id << "\n";
        colorReset();
    }
    else if (choice == 2) {
        if (requestCount >= MAX_REQUESTS) {
            colorRed();
            cout << " [!] База заявок штаба переполнена!\n";
            colorReset();
            waitForEnter();
            return;
        }
        Request r;
        r.id = (requestCount == 0) ? 1 : requests[requestCount - 1].id + 1;

        cout << "Введите описание проблемы (через_подчеркивание): ";
        cin >> r.description;

        cout << "Введите категорию помощи (Медицина, Поиск, Еда, Ремонт): ";
        getValidCategory(r.category);

        cout << "Уровень критичности (1 - Низкий, 2 - Средняя, 3 - ЧС): ";
        r.urgency = getSafeIntInput();
        while (r.urgency < 1 || r.urgency > 3) {
            colorRed();
            cout << " [!] Ошибка: Уровень ЧС может быть только от 1 до 3. Повторите ввод: ";
            colorReset();
            r.urgency = getSafeIntInput();
        }

        cout << "Необходимое количество часов работы (1-24): ";
        r.requiredHours = getSafeIntInput();
        while (r.requiredHours <= 0 || r.requiredHours > 24) {
            colorRed();
            cout << " [!] Ошибка: Часы должны быть от 1 до 24. Повторите ввод: ";
            colorReset();
            r.requiredHours = getSafeIntInput();
        }

        r.isClosed = false;
        requests[requestCount] = r;
        requestCount++;
        colorGreen();
        cout << " -> Успешно! Заявка добавлена с ID: " << r.id << "\n";
        colorReset();
    }
    saveRequests();
    saveVolunteers();
    waitForEnter();
}

// ============================================================================
// ЭТАП 5. ЭКСТРЕННЫЙ МЭТЧИНГ (ПОИСК КАДРОВ ПО КАТЕГОРИИ)
// ============================================================================

void emergencyMatching() {
    system("cls");
    colorCyan();
    cout << "\n=========================================\n";
    cout << "       АЛГОРИТМ ЭКСТРЕННОГО МЭТЧИНГА\n";
    cout << "=========================================\n";
    colorReset();
    cout << "Введите ID заявки для подбора команды: ";
    int targetId = getSafeIntInput();

    int foundIdx = -1;
    for (int i = 0; i < requestCount; ++i) {
        if (requests[i].id == targetId) { foundIdx = i; break; }
    }

    if (foundIdx == -1) {
        colorRed();
        cout << " [!] Заявка с таким ID не найдена.\n";
        colorReset();
        waitForEnter();
        return;
    }

    colorYellow();
    cout << " Анализ задачи: [" << requests[foundIdx].description << "]\n";
    cout << " Требуемый навык: " << requests[foundIdx].category << "\n";
    colorCyan();
    cout << "-----------------------------------------\n";
    cout << "| " << setw(4) << left << "ID"
        << "| " << setw(20) << left << "Подходящий волонтер"
        << "| " << setw(10) << left << "Навык" << "|\n";
    cout << "-----------------------------------------\n";
    colorReset();

    bool anyFound = false;
    for (int i = 0; i < volunteerCount; ++i) {
        if (areStringsEqual(volunteers[i].skill, requests[foundIdx].category)) {
            cout << "| " << setw(4) << left << volunteers[i].id
                << "| " << setw(20) << left << volunteers[i].name
                << "| " << setw(10) << left << volunteers[i].skill << "|\n";
            anyFound = true;
        }
    }
    if (!anyFound) {
        colorYellow();
        cout << "|        Свободные специалисты не найдены        |\n";
        colorReset();
    }
    colorCyan();
    cout << "-----------------------------------------\n";
    colorReset();
    waitForEnter();
}

// ============================================================================
// ЭТАП 6. ФИКСАЦИЯ ПОМОЩИ И НАЧИСЛЕНИЕ ЧАСОВ
// ============================================================================

void closeRequestAndReward() {
    system("cls");
    colorCyan();
    cout << "\n=========================================\n";
    cout << "          ФИКСАЦИЯ ВЫПОЛНЕНИЯ ПОМОЩИ\n";
    cout << "=========================================\n";
    colorReset();
    cout << "Введите ID закрываемой заявки: ";
    int reqId = getSafeIntInput();

    int reqIdx = -1;
    for (int i = 0; i < requestCount; ++i) {
        if (requests[i].id == reqId) { reqIdx = i; break; }
    }

    if (reqIdx == -1 || requests[reqIdx].isClosed) {
        colorRed();
        cout << " [!] Заявка не найдена или уже была закрыта.\n";
        colorReset();
        waitForEnter();
        return;
    }

    cout << "Введите ID исполнителя (волонтера): ";
    int volId = getSafeIntInput();

    int volIdx = -1;
    for (int i = 0; i < volunteerCount; ++i) {
        if (volunteers[i].id == volId) { volIdx = i; break; }
    }

    if (volIdx == -1) {
        colorRed();
        cout << " [!] Волонтер с таким ID не найден.\n";
        colorReset();
        waitForEnter();
        return;
    }

    requests[reqIdx].isClosed = true;
    volunteers[volIdx].totalHours += requests[reqIdx].requiredHours;

    colorGreen();
    cout << " -> Статус изменен! Волонтеру " << volunteers[volIdx].name
        << " успешно начислено " << requests[reqIdx].requiredHours << " ч.\n";
    colorReset();
    saveRequests();
    saveVolunteers();
    waitForEnter();
}

// ============================================================================
// ЭТАП 7. ПУЗЫРЬКОВАЯ СОРТИРОВКА ПО КРИТИЧНОСТИ С ЦВЕТНЫМ ВЫДЕЛЕНИЕМ ЧС
// ============================================================================

void printSortedRequests() {
    system("cls");
    colorCyan();
    cout << "\n=========================================================================\n";
    cout << "            АКТУАЛЬНЫЕ ЗАЯВКИ (СОРТИРОВКА ПО УРОВНЮ ЧС)\n";
    cout << "=========================================================================\n";
    colorReset();

    if (requestCount == 0) {
        colorYellow();
        cout << " База данных заявок пуста.\n";
        colorReset();
        waitForEnter();
        return;
    }

    Request temp[MAX_REQUESTS];
    for (int i = 0; i < requestCount; ++i) { temp[i] = requests[i]; }

    for (int i = 0; i < requestCount - 1; ++i) {
        for (int j = 0; j < requestCount - i - 1; ++j) {
            if (temp[j].urgency < temp[j + 1].urgency) {
                Request swapHolder = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = swapHolder;
            }
        }
    }

    colorCyan();
    cout << "-------------------------------------------------------------------------\n";
    cout << "| " << setw(4) << left << "ID"
        << "| " << setw(6) << left << "КРИТ."
        << "| " << setw(15) << left << "КАТЕГОРИЯ"
        << "| " << setw(6) << left << "ЧАСЫ"
        << "| " << setw(10) << left << "СТАТУС"
        << "| " << setw(20) << left << "ОПИСАНИЕ" << "|\n";
    cout << "-------------------------------------------------------------------------\n";
    colorReset();

    for (int i = 0; i < requestCount; ++i) {
        if (temp[i].urgency == 3 && !temp[i].isClosed) {
            colorRed();
        }
        else if (temp[i].isClosed) {
            colorGreen();
        }
        else {
            colorWhite();
        }

        cout << "| " << setw(4) << left << temp[i].id
            << "| " << setw(6) << left << temp[i].urgency
            << "| " << setw(15) << left << temp[i].category
            << "| " << setw(6) << left << temp[i].requiredHours
            << "| " << setw(10) << left << (temp[i].isClosed ? "ЗАКРЫТА" : "ОТКРЫТА")
            << "| " << setw(20) << left << temp[i].description << "|\n";

        colorReset();
    }
    colorCyan();
    cout << "-------------------------------------------------------------------------\n";
    colorReset();
    waitForEnter();
}

// ============================================================================
// ЭТАП 8. АНАЛИТИКА ЭФФЕКТИВНОСТИ ШТАБА (ЦВЕТНЫЕ ПОКАЗАТЕЛИ)
// ============================================================================

void showAnalytics() {
    system("cls");
    colorCyan();
    cout << "\n=========================================\n";
    cout << "      АНАЛИТИКА ЭФФЕКТИВНОСТИ ШТАБА\n";
    cout << "=========================================\n";
    colorReset();

    if (requestCount == 0) {
        colorYellow();
        cout << " Нет данных для анализа.\n";
        colorReset();
        waitForEnter();
        return;
    }

    int closedCount = 0;
    int activeCount = 0;
    int totalVolHours = 0;

    for (int i = 0; i < requestCount; ++i) {
        if (requests[i].isClosed) closedCount++;
        else activeCount++;
    }

    for (int i = 0; i < volunteerCount; ++i) { totalVolHours += volunteers[i].totalHours; }

    double closedPercent = ((double)closedCount / requestCount) * 100.0;

    cout << " Активных (открытых) заявок в ЧС: "; colorRed(); cout << activeCount << "\n"; colorReset();
    cout << " Успешно закрытых задач населения: "; colorGreen(); cout << closedCount << "\n"; colorReset();
    cout << " Процент выполнения задач штаба:   "; colorYellow(); cout << closedPercent << " %\n"; colorReset();
    cout << " Суммарный вклад работы волонтеров: "; colorCyan(); cout << totalVolHours << " ч.\n"; colorReset();
    colorCyan();
    cout << "=========================================\n";
    colorReset();
    waitForEnter();
}

// ============================================================================
// ЭТАП 9. ЭКСПОРТ КРАСИВОГО REPORT.TXT
// ============================================================================

void exportReport() {
    system("cls");
    ofstream report("report.txt");
    if (!report.is_open()) {
        colorRed();
        cout << " [!] Не удалось создать файл отчета.\n";
        colorReset();
        waitForEnter();
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
    colorGreen();
    cout << " -> Успешно! Сводная таблица сохранена в файл 'report.txt'.\n";
    colorReset();
    waitForEnter();
}

void showInstructions() {
    system("cls");
    colorCyan();
    cout << "\n=========================================================\n";
    cout << "          ИНСТРУКЦИЯ ПОЛЬЗОВАТЕЛЯ VOLUNTEER HUB          \n";
    cout << "=========================================================\n";
    colorReset();
    cout << " Добро пожаловать в систему координации штаба ЧС!\n\n";
    cout << " 1. Правила ввода текстовых данных:\n";
    cout << "    ВАЖНО: Вводите ФИО и описания строго через подчеркивание\n";
    cout << "    (например: Ivanov_Ivan).\n\n";
    cout << " 2. Система умной фильтрации мусора:\n";
    cout << "    Группа крови должна быть от 1 до 4, уровень ЧС от 1 до 3.\n\n";
    cout << " 3. Сохранение данных:\n";
    cout << "    База данных сохраняется автоматически при нажатии '0'.\n";
    colorCyan();
    cout << "=========================================================\n";
    colorReset();
    waitForEnter();
}

// ============================================================================
// ГЛАВНЫЙ ЦИКЛ ПРОГРАММЫ И МЕНЮ SWITCH С ЦВЕТНЫМИ ПОДСКАЗКАМИ
// ============================================================================

int main() {
        // Настройки кодировки удалены для кроссплатформенности. 
        // Настройте кодировку вашего терминала (UTF-8 / CP1251) в параметрах компилятора.

        loadRequests();
        loadVolunteers();

        loginSystem();

        int choice = -1;
        while (choice != 0) {
            system("cls");
            colorCyan();
            cout << "\n=========================================\n";
            cout << "          VOLUNTEER HUB SYSTEM           \n";
            cout << "   Текущий режим: ";
            if (isAdmin) { colorGreen(); cout << "[КООРДИНАТОР]\n"; }
            else { colorYellow(); cout << "[ГОСТЬ]\n"; }
            colorCyan();
            cout << "=========================================\n";
            colorReset();

            cout << "1. Регистрация волонтеров и заявок ЧС "; if (!isAdmin) { colorRed(); cout << "(ЗАБЛОКИРОВАНО)"; colorReset(); } cout << "\n";
            cout << "2. Экстренный мэтчинг (Поиск кадров) "; if (!isAdmin) { colorRed(); cout << "(ЗАБЛОКИРОВАНО)"; colorReset(); } cout << "\n";
            cout << "3. Фиксация помощи и начисление часов "; if (!isAdmin) { colorRed(); cout << "(ЗАБЛОКИРОВАНО)"; colorReset(); } cout << "\n";
            cout << "4. Показать заявки (Сортировка по ЧС)\n";
            cout << "5. Вывести аналитику эффективности штаба\n";
            cout << "6. Экспорт официального отчета в файл "; if (!isAdmin) { colorRed(); cout << "(ЗАБЛОКИРОВАНО)"; colorReset(); } cout << "\n";
            cout << "7. Инструкция по работе со штабом\n";
            colorYellow();
            cout << "0. Сохранить изменения и выйти\n";
            colorCyan();
            cout << "=========================================\n";
            colorWhite();
            cout << "Введите ваш выбор: ";
            colorReset();

            choice = getSafeIntInput();

            switch (choice) {
            case 1:
                if (isAdmin) registerEntity();
                else { colorRed(); cout << " [!] Доступ закрыт! Гости не могут менять базу данных.\n"; colorReset(); waitForEnter(); }
                break;
            case 2:
                if (isAdmin) emergencyMatching();
                else { colorRed(); cout << " [!] Доступ закрыт! Поиск доступен только координаторам.\n"; colorReset(); waitForEnter(); }
                break;
            case 3:
                if (isAdmin) closeRequestAndReward();
                else { colorRed(); cout << " [!] Доступ закрыт! Начислять опыт может только админ.\n"; colorReset(); waitForEnter(); }
                break;
            case 4: printSortedRequests(); break;
            case 5: showAnalytics(); break;
            case 6:
                if (isAdmin) exportReport();
                else { colorRed(); cout << " [!] Доступ закрыт! Экспорт отчетов доступен только админу.\n"; colorReset(); waitForEnter(); }
                break;
            case 7: showInstructions(); break;
            case 0:
                if (isAdmin) {
                    saveRequests();
                    saveVolunteers();
                    colorGreen();
                    cout << "\n[!] Все базы сохранены. Завершение работы штаба.\n";
                    colorReset();
                }
                else {
                    colorYellow();
                    cout << "\n[!] Завершение работы в режиме гостя (изменения не перезаписаны).\n";
                    colorReset();
                }
                break;
            default:
                colorRed();
                cout << " [!] Неверный пункт меню. Повторите попытку.\n";
                colorReset();
                waitForEnter();
            }
        }
        return 0;
    }
