#include <iostream>
#include <string>
#include <iomanip>
#include <cctype>
#include <cstdlib>
using namespace std;

const int MAX_APPEALS = 50;

string violationName[6] = {"", "No Parking", "Overtime", "Disabled Spot",
                           "Fire Lane", "Double Parking"};
double violationFine[6] = {0, 500, 300, 2000, 1500, 800};

class Appeal
{
private:
    string appealId;
    string ticketNo;
    string vehicleNo;
    string ownerName;
    int violationType;
    double fine;
    string reason;
    char status;
    static int count;

public:
    void setData(string appealId, string ticketNo, string vehicleNo,
                 string ownerName, int violationType, string reason);

    string getId() { return appealId; }
    string getTicket() { return ticketNo; }
    string getVehicle() { return vehicleNo; }
    int getType() { return violationType; }
    double getFine() { return fine; }

    inline string statusText() const;

    static int getCount() { return count; }
    static void increaseCount() { count++; }

    friend void printAppeal(const Appeal &a);
};

int Appeal::count = 0;

void Appeal::setData(string appealId, string ticketNo, string vehicleNo,
                     string ownerName, int violationType, string reason)
{
    this->appealId = appealId;
    this->ticketNo = ticketNo;
    this->vehicleNo = vehicleNo;
    this->ownerName = ownerName;
    this->violationType = violationType;
    this->reason = reason;
    this->fine = violationFine[violationType];
    this->status = 'P';
}

inline string Appeal::statusText() const
{
    if (status == 'A')
        return "Approved";
    if (status == 'R')
        return "Rejected";
    return "Pending";
}

void printAppeal(const Appeal &a)
{
    cout << left << setw(8) << a.appealId
         << setw(9) << a.ticketNo
         << setw(12) << a.vehicleNo
         << setw(26) << a.ownerName
         << setw(16) << violationName[a.violationType]
         << setw(8) << fixed << setprecision(0) << a.fine
         << setw(10) << a.statusText() << endl;
    cout << "        Reason: " << a.reason << endl;
}

void printHeader()
{
    cout << left << setw(8) << "Appeal"
         << setw(9) << "Ticket"
         << setw(12) << "Vehicle"
         << setw(26) << "Owner"
         << setw(16) << "Violation"
         << setw(8) << "Fine"
         << setw(10) << "Status" << endl;
    cout << string(89, '-') << endl;
}

string readLine(string msg)
{
    string s;
    cout << msg;
    if (!getline(cin, s))
        exit(0);
    return s;
}

// removes spaces from the start and end of the text
string trim(string s)
{
    int start = 0;
    int end = (int)s.length() - 1;
    while (start <= end && s[start] == ' ')
        start++;
    while (end >= start && s[end] == ' ')
        end--;
    return s.substr(start, end - start + 1);
}

// accepts only whole numbers, so 2.5 or 6xyz are rejected
int readInt(string msg)
{
    while (true)
    {
        string s = trim(readLine(msg));
        bool valid = s.length() >= 1 && s.length() <= 6;
        for (int i = 0; i < (int)s.length(); i++)
        {
            if (!isdigit(s[i]))
                valid = false;
        }
        if (valid)
            return atoi(s.c_str());
        cout << "  Please enter a whole number.\n";
    }
}

string toUpper(string s)
{
    for (int i = 0; i < (int)s.length(); i++)
        s[i] = toupper(s[i]);
    return s;
}

string removeSpaces(string s)
{
    string result = "";
    for (int i = 0; i < (int)s.length(); i++)
    {
        if (s[i] != ' ')
            result = result + s[i];
    }
    return result;
}

bool isValidCode(string code, string prefix, int digits)
{
    if ((int)code.length() != (int)prefix.length() + digits)
        return false;
    if (code.substr(0, prefix.length()) != prefix)
        return false;
    for (int i = (int)prefix.length(); i < (int)code.length(); i++)
    {
        if (!isdigit(code[i]))
            return false;
    }
    return true;
}

// Bharat series, for example 22BH1234AA
bool isBharatSeries(string v)
{
    int len = (int)v.length();
    if (len != 9 && len != 10)
        return false;
    if (!isdigit(v[0]) || !isdigit(v[1]) || v.substr(2, 2) != "BH")
        return false;
    for (int i = 4; i < 8; i++)
    {
        if (!isdigit(v[i]))
            return false;
    }
    for (int i = 8; i < len; i++)
    {
        if (!isalpha(v[i]))
            return false;
    }
    return true;
}

// normal format: 2 letters, 1-2 digits, 1-3 letters, 4 digits
bool isValidVehicle(string v)
{
    if (isBharatSeries(v))
        return true;

    int len = (int)v.length();
    if (len < 8 || len > 11)
        return false;
    if (!isalpha(v[0]) || !isalpha(v[1]))
        return false;

    int i = 2;
    int digits = 0;
    while (i < len && isdigit(v[i]))
    {
        i++;
        digits++;
    }
    if (digits < 1 || digits > 2)
        return false;

    int letters = 0;
    while (i < len && isalpha(v[i]))
    {
        i++;
        letters++;
    }
    if (letters < 1 || letters > 3)
        return false;

    if (len - i != 4)
        return false;
    for (; i < len; i++)
    {
        if (!isdigit(v[i]))
            return false;
    }
    return true;
}

bool isValidName(string name)
{
    if (name.length() == 0 || name.length() > 25)
        return false;
    if (!isalpha(name[0]))
        return false;
    for (int i = 0; i < (int)name.length(); i++)
    {
        if (!isalpha(name[i]) && name[i] != ' ' && name[i] != '.')
            return false;
    }
    return true;
}

// operations

// returns address of the appeal, or NULL if not found
Appeal *findById(Appeal list[], string id)
{
    Appeal *a = list;
    for (int i = 0; i < Appeal::getCount(); i++)
    {
        if ((a + i)->getId() == id)
            return a + i;
    }
    return NULL;
}

Appeal *findByTicket(Appeal list[], string ticket)
{
    Appeal *a = list;
    for (int i = 0; i < Appeal::getCount(); i++)
    {
        if ((a + i)->getTicket() == ticket)
            return a + i;
    }
    return NULL;
}

void showViolationTypes()
{
    for (int i = 1; i <= 5; i++)
        cout << "  " << i << ". " << left << setw(16) << violationName[i]
             << "Fine Rs " << violationFine[i] << endl;
}

void addAppeal(Appeal list[])
{
    if (Appeal::getCount() == MAX_APPEALS)
    {
        cout << "Storage is full. Cannot add more appeals.\n";
        return;
    }

    string id;
    while (true)
    {
        id = toUpper(readLine("Appeal ID (e.g. AP1001): "));
        if (!isValidCode(id, "AP", 4))
            cout << "  Invalid ID. Use AP followed by 4 digits.\n";
        else if (findById(list, id) != NULL)
            cout << "  This appeal ID already exists.\n";
        else
            break;
    }

    string ticket;
    while (true)
    {
        ticket = toUpper(readLine("Ticket number (e.g. TK10001): "));
        if (!isValidCode(ticket, "TK", 5))
            cout << "  Invalid ticket. Use TK followed by 5 digits.\n";
        else if (findByTicket(list, ticket) != NULL)
            cout << "  An appeal for this ticket is already filed.\n";
        else
            break;
    }

    string vehicle = toUpper(removeSpaces(readLine("Vehicle number (e.g. HR10AB1234): ")));
    while (!isValidVehicle(vehicle))
    {
        cout << "  Invalid number. Examples: HR10AB1234, DL3CAB1234, 22BH1234AA.\n";
        vehicle = toUpper(removeSpaces(readLine("Vehicle number: ")));
    }

    string owner = trim(readLine("Owner name: "));
    while (!isValidName(owner))
    {
        cout << "  Use letters, spaces and dots, max 25 characters.\n";
        owner = trim(readLine("Owner name: "));
    }

    cout << "Violation type:\n";
    showViolationTypes();
    int type = readInt("Choose 1 to 5: ");
    while (type < 1 || type > 5)
    {
        cout << "  Enter a number from 1 to 5.\n";
        type = readInt("Choose 1 to 5: ");
    }

    string reason = trim(readLine("Reason for appeal (10 to 60 characters): "));
    while (reason.length() < 10 || reason.length() > 60)
    {
        cout << "  Reason must be 10 to 60 characters. You typed "
             << reason.length() << ".\n";
        reason = trim(readLine("Reason for appeal: "));
    }

    Appeal *a = &list[Appeal::getCount()];
    a->setData(id, ticket, vehicle, owner, type, reason);
    Appeal::increaseCount();

    cout << "\nAppeal " << id << " filed against ticket " << ticket
         << ". Status: Pending.\n";
}

void showAll(Appeal list[])
{
    if (Appeal::getCount() == 0)
    {
        cout << "No appeals filed yet.\n";
        return;
    }

    printHeader();
    Appeal *a = list;
    for (int i = 0; i < Appeal::getCount(); i++)
    {
        printAppeal(*(a + i));
    }
    cout << "\nTotal appeals: " << Appeal::getCount() << endl;
}

void searchById(Appeal list[])
{
    string id = toUpper(readLine("Enter appeal ID: "));
    Appeal *a = findById(list, id);

    if (a == NULL)
    {
        cout << "No appeal found with ID " << id << endl;
        return;
    }

    printHeader();
    printAppeal(*a);
}

// one vehicle can have many tickets, so show every match
void searchByVehicle(Appeal list[])
{
    string v = toUpper(removeSpaces(readLine("Enter vehicle number: ")));
    int found = 0;
    double total = 0;

    for (int i = 0; i < Appeal::getCount(); i++)
    {
        if (list[i].getVehicle() == v)
        {
            if (found == 0)
                printHeader();
            printAppeal(list[i]);
            total = total + list[i].getFine();
            found++;
        }
    }

    if (found == 0)
        cout << "No appeals for vehicle " << v << endl;
    else
        cout << "\nAppeals: " << found
             << "   Total fine under appeal: Rs " << total << endl;
}

void showSummary(Appeal list[])
{
    if (Appeal::getCount() == 0)
    {
        cout << "No appeals filed yet.\n";
        return;
    }

    int countOf[6] = {0};
    double grandTotal = 0;

    for (int i = 0; i < Appeal::getCount(); i++)
    {
        countOf[list[i].getType()]++;
        grandTotal = grandTotal + list[i].getFine();
    }

    cout << left << setw(18) << "Violation" << setw(10) << "Appeals"
         << "Fine total" << endl;
    cout << string(40, '-') << endl;
    for (int t = 1; t <= 5; t++)
    {
        cout << left << setw(18) << violationName[t]
             << setw(10) << countOf[t]
             << fixed << setprecision(0) << countOf[t] * violationFine[t] << endl;
    }
    cout << string(40, '-') << endl;
    cout << left << setw(18) << "TOTAL" << setw(10) << Appeal::getCount() << grandTotal << endl;
}

int main()
{
    Appeal appeals[MAX_APPEALS];
    int choice;

    cout << "=======================================\n";
    cout << "   PARKING VIOLATION APPEAL TRACKER\n";
    cout << "=======================================\n";

    do
    {
        cout << "\n1. File new appeal\n";
        cout << "2. Show all appeals\n";
        cout << "3. Search by appeal ID\n";
        cout << "4. Search by vehicle number\n";
        cout << "5. Summary by violation type\n";
        cout << "6. Exit\n";

        choice = readInt("Enter choice: ");
        cout << endl;

        switch (choice)
        {
        case 1:
            addAppeal(appeals);
            break;
        case 2:
            showAll(appeals);
            break;
        case 3:
            searchById(appeals);
            break;
        case 4:
            searchByVehicle(appeals);
            break;
        case 5:
            showSummary(appeals);
            break;
        case 6:
            cout << "Goodbye!\n";
            break;
        default:
            cout << "Wrong choice, enter 1 to 6.\n";
        }
    } while (choice != 6);

    return 0;
}
