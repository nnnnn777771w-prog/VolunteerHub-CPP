#include <iostream>
#include <fstream>
#include <clocale>
#include <windows.h> 
#include <iomanip> // Библиотека для идеального выравнивания таблиц

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
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ (ЗАЩИТА ВВОДA, СРАВНЕНИЕ СТРОК, НАДЕЖНАЯ ПАУЗА)
// ============================================================================

int getSafeIntInput() {
    int value;
    while (true) {
        cin >> value;
        if (cin.fail()) { 
            cin.clear();            
            cin.ignore(10000, '\n'); 
            cout << " [!] Ошибка! Введите корректное целое число: ";
        } else {
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

// Надежная функция паузы — очищает буфер, чтобы экран не пролетал мимо
void waitForEnter() {
    cout << "\nНажмите Enter, чтобы вернуться в меню...";
    cin.clear();            // Сбрасываем ошибки потока
    cin.ignore(10000, '\n'); // Очищаем весь накопленный мусор и старые нажатия Enter
    cin.get();              // Ждем честного нажатия кнопки от пользователя
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
    system("cls"); // Очищаем экран при входе в регистрацию
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
            waitForEnter();
            return;
        }
        Volunteer v;
        v.id = (volunteerCount == 0) ? 1 : volunteers[volunteerCount - 1].id + 1;
        
        cout << "Введите ФИО волонтера (СТРОГО через подчеркивание, например Ivanov_Ivan): ";
        cin >> v.name;
        
        // Проверка на обязательное наличие фамилии и имени (символ '_')
        bool hasUnderscore = false;
        for (int i = 0; v.name[i] != '\0'; i++) {
            if (v.name[i] == '_') { hasUnderscore = true; break; }
        }
        while (!hasUnderscore) {
            cout << " [!] Ошибка: Введите Фамилию и Имя через '_' (например, Petrov_Petr): ";
            cin >> v.name;
            for (int i = 0; v.name[i] != '\0'; i++) {
                if (v.name[i] == '_') { hasUnderscore = true; break; }
            }
        }
        
        cout << "Введите навык/специализацию (Медицина, Поиск, Еда, Ремонт): ";
        cin >> v.skill;
        
        cout << "Введите группу крови (1-4): ";
        v.bloodType = getSafeIntInput();
        while (v.bloodType < 1 || v.bloodType > 4) {
            cout << " [!] Ошибка: Группа крови должна быть от 1 до 4. Повторите ввод: ";
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
            waitForEnter();
            return;
        }
        Request r;
        r.id = (requestCount == 0) ? 1 : requests[requestCount - 1].id + 1;

        cout << "Введите описание проблемы (через_подчеркивание): ";
        cin >> r.description;
        
        cout << "Введите категорию помощи (Медицина, Поиск, Еда, Ремонт): ";
        cin >> r.category;
        
        cout << "Уровень критичности (1 - Низкий, 2 - Средняя, 3 - ЧС): ";
        r.urgency = getSafeIntInput();
        while (r.urgency < 1 || r.urgency > 3) {
            cout << " [!] Ошибка: Уровень ЧС может быть только от 1 до 3. Повторите ввод: ";
            r.urgency = getSafeIntInput();
        }
        
        cout << "Необходимое количество часов работы (1-24): ";
        r.requiredHours = getSafeIntInput();
        while (r.requiredHours <= 0 || r.requiredHours > 24) {
            cout << " [!] Ошибка: Часы должны быть от 1 до 24. Повторите ввод: ";
            r.requiredHours = getSafeIntInput();
        }
        
        r.isClosed = false;
        requests[requestCount] = r;
        requestCount++;
        cout << " -> Успешно! Заявка добавлена с ID: " << r.id << "\n";
    }
    saveRequests();
    saveVolunteers();
    waitForEnter();
}

// ============================================================================
// ЭТАП 5. ЭКСТРЕННЫЙ МЭТЧИНГ (ПОИСК КАДРОВ ПО КАТЕГОРИИ)
// ============================================================================

void emergencyMatching() {
    system("cls"); // Очищаем экран
    cout << "\n=========================================\n";
    cout << "       АЛГОРИТМ ЭКСТРЕННОГО МЭТЧИНГА\n";
    cout << "=========================================\n";
    cout << "Введите ID заявки для подбора команды: ";
    int targetId = getSafeIntInput();

    int foundIdx = -1;
    for (int i = 0; i < requestCount; ++i) {
        if (requests[i].id == targetId) { foundIdx = i; break; }
    }

    if (foundIdx == -1) {
        cout << " [!] Заявка с таким ID не найдена.\n";
        waitForEnter();
        return;
    }

    cout << " Анализ задачи: [" << requests[foundIdx].description << "]\n";
    cout << " Требуемый навык: " << requests[foundIdx].category << "\n";
    cout << "-----------------------------------------\n";
    cout << "| " << setw(4) << left << "ID" 
         << "| " << setw(20) << left << "Подходящий волонтер" 
         << "| " << setw(10) << left << "Навык" << "|\n";
    cout << "-----------------------------------------\n";

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
        cout << "|        Свободные специалисты не найдены        |\n";
    }
    cout << "-----------------------------------------\n";
    waitForEnter(); 
}

// ============================================================================
// ЭТАП 6. ФИКСАЦИЯ ПОМОЩИ И НАЧИСЛЕНИЕ ЧАСОВ
// ============================================================================

void closeRequestAndReward() {
    system("cls");
    cout << "\n=========================================\n";
    cout << "          ФИКСАЦИЯ ВЫПОЛНЕНИЯ ПОМОЩИ\n";
    cout << "=========================================\n";
    cout << "Введите ID закрываемой заявки: ";
    int reqId = getSafeIntInput();

    int reqIdx = -1;
    for (int i = 0; i < requestCount; ++i) {
        if (requests[i].id == reqId) { reqIdx = i; break; }
    }

    if (reqIdx == -1 || requests[reqIdx].isClosed) {
        cout << " [!] Заявка не найдена или уже была закрыта.\n";
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
        cout << " [!] Волонтер с таким ID не найден.\n";
        waitForEnter();
        return;
    }

    requests[reqIdx].isClosed = true;
    volunteers[volIdx].totalHours += requests[reqIdx].requiredHours;

    cout << " -> Статус изменен! Волонтеру " << volunteers[volIdx].name 
         << " успешно начислено " << requests[reqIdx].requiredHours << " ч.\n";
    saveRequests();
    saveVolunteers();
    waitForEnter();
}

// ============================================================================
// ЭТАП 7. ПУЗЫРЬКОВАЯ СОРТИРОВКА ПО КРИТИЧНОСТИ
// ============================================================================

void printSortedRequests() {
    system("cls");
    cout << "\n=========================================================================\n";
    cout << "            АКТУАЛЬНЫЕ ЗАЯВКИ (СОРТИРОВКА ПО УРОВНЮ ЧС)\n";
    cout << "=========================================================================\n";

    if (requestCount == 0) {
        cout << " База данных заявок пуста.\n";
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

    cout << "-------------------------------------------------------------------------\n";
    cout << "| " << setw(4) << left << "ID" 
         << "| " << setw(6) << left << "КРИТ." 
         << "| " << setw(15) << left << "КАТЕГОРИЯ" 
         << "| " << setw(6) << left << "ЧАСЫ" 
         << "| " << setw(10) << left << "СТАТУС" 
         << "| " << setw(20) << left << "ОПИСАНИЕ" << "|\n";
    cout << "-------------------------------------------------------------------------\n";
    
    for (int i = 0; i < requestCount; ++i) {
        cout << "| " << setw(4) << left << temp[i].id 
             << "| " << setw(6) << left << temp[i].urgency 
             << "| " << setw(15) << left << temp[i].category 
             << "| " << setw(6) << left << temp[i].requiredHours 
             << "| " << setw(10) << left << (temp[i].isClosed ? "ЗАКРЫТА" : "ОТКРЫТА") 
             << "| " << setw(20) << left << temp[i].description << "|\n";
    }
    cout << "-------------------------------------------------------------------------\n";
    waitForEnter();
}

// ============================================================================
// ЭТАП 8. АНАЛИТИКА ЭФФЕКТИВНОСТИ ШТАБА
// ============================================================================

void showAnalytics() {
    system("cls");
    cout << "\n=========================================\n";
    cout << "      АНАЛИТИКА ЭФФЕКТИВНОСТИ ШТАБА\n";
    cout << "=========================================\n";

    if (requestCount == 0) {
        cout << " Нет данных для анализа.\n";
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

    cout << " Активных (открытых) заявок в ЧС: " << activeCount << "\n";
    cout << " Успешно закрытых задач населения: " << closedCount << "\n";
    cout << " Процент выполнения задач штаба:   " << closedPercent << " %\n";
    cout << " Суммарный вклад работы волонтеров: " << totalVolHours << " человеко-часов\n";
    cout << "=========================================\n";
    waitForEnter();
}

// ============================================================================
// ЭТАП 9. ЭКСПОРТ КРАСИВОГО REPORT.TXT
// ============================================================================

void exportReport() {
    system("cls");
    ofstream report("report.txt");
    if (!report.is_open()) {
        cout << " [!] Не удалось создать файл отчета.\n";
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
    cout << " -> Успешно! Сводная таблица сохранена в файл 'report.txt'.\n";
    waitForEnter();
}

// ============================================================================
// ДОПОЛНИТЕЛЬНО: ИНСТРУКЦИЯ ПОЛЬЗОВАТЕЛЯ
// ============================================================================

void showInstructions() {
    system("cls");
    cout << "\n=========================================================\n";
    cout << "          ИНСТРУКЦИЯ ПОЛЬЗОВАТЕЛЯ VOLUNTEER HUB          \n";
    cout << "=========================================================\n";
    cout << " Добро пожаловать в систему координации штаба ЧС!\n\n";
    cout << " 1. Правила ввода текстовых данных:\n";
    cout << "    ВАЖНО: Вводите ФИО и описания строго через подчеркивание\n";
    cout << "    (например: Ivanov_Ivan). Программа автоматически отклонит\n";
    cout << "    простые одиночные имена без фамилии ради чистоты базы.\n\n";
    cout << " 2. Система умной фильтрации мусора:\n";
    cout << "    Приложение автоматически отклоняет некорректные данные.\n";
    cout << "    Группа крови должна быть от 1 до 4, уровень ЧС от 1 до 3,\n";
    cout << "    а время одной задачи — не более 24 часов.\n\n";
    cout << " 3. Сохранение данных:\n";
    cout << "    База данных сохраняется автоматически при нажатии '0'\n";
    cout << "    и корректном выходе из программы.\n";
    cout << "=========================================================\n";
    waitForEnter();
}

// ============================================================================
// ГЛАВНЫЙ ЦИКЛ ПРОГРАММЫ И МЕНЮ SWITCH
// ============================================================================

int main() {
    // Настраиваем кодировку Windows-1251 (для полноценного русского языка и в консоли, и в блокноте)
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian"); 

    loadRequests();
    loadVolunteers();

    int choice = -1;
    while (choice != 0) {
        system("cls"); // Очищаем экран перед каждым показом главного меню
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
                waitForEnter();
        }
    }
    return 0;
}
