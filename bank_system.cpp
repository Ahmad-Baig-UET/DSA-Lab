#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

string xorCipher(const string& data)
{
    string result = data;
    for (size_t i = 0; i < result.size(); i++)
        result[i] ^= 0x4B;
    return result;
}

class Account
{
protected:
    int accountId;
    string ownerName;
    double balance;
    unsigned int permissions;
    vector<double> transactions;

public:
    virtual void deposit(double amount)  = 0;
    virtual void withdraw(double amount) = 0;
    virtual void showAccount()           = 0;
    virtual void saveToFile(ofstream& f) = 0;
    virtual string getType()             = 0;

    void showHistory()
    {
        if (transactions.empty())
        {
            cout << "  No transactions yet.\n";
            return;
        }
        cout << "\n  --- Transaction History ---\n";
        for (size_t i = 0; i < transactions.size(); i++)
        {
            if (transactions[i] > 0)
                cout << "  [" << i+1 << "] Deposit  : +Rs " << fixed << setprecision(2) << transactions[i] << "\n";
            else
                cout << "  [" << i+1 << "] Withdraw : -Rs " << fixed << setprecision(2) << -transactions[i] << "\n";
        }
    }

    int    getId()      { return accountId; }
    string getName()    { return ownerName; }
    double getBalance() { return balance; }
    unsigned int getPermissions() { return permissions; }
    void setPermissions(unsigned int p) { permissions = p; }
    vector<double>& getTransactions() { return transactions; }

    virtual ~Account()
    {
        cout << "  [Account of " << ownerName << " removed]\n";
    }
};


class SavingsAccount : public Account
{
private:
    double interestRate;

public:
    SavingsAccount(int id, string name, double startBalance, unsigned int perms = 1 | 2 | 4)
    {
        accountId    = id;
        ownerName    = name;
        balance      = startBalance;
        interestRate = 0.05;
        permissions  = perms;
    }

    string getType() { return "SAVINGS"; }

    void deposit(double amount)
    {
        if (!(permissions & 2)) { cout << "  Deposit not permitted.\n"; return; }
        if (amount <= 0)        { cout << "  Enter a valid amount.\n";  return; }
        balance += amount;
        transactions.push_back(amount);
        cout << "  Rs " << fixed << setprecision(2) << amount << " deposited. Balance: Rs " << balance << "\n";
    }

    void withdraw(double amount)
    {
        if (!(permissions & 1)) { cout << "  Withdrawal not permitted.\n"; return; }
        if (amount <= 0)        { cout << "  Enter a valid amount.\n";     return; }
        if (amount > balance)   { cout << "  Insufficient balance.\n";     return; }
        balance -= amount;
        transactions.push_back(-amount);
        cout << "  Rs " << fixed << setprecision(2) << amount << " withdrawn. Balance: Rs " << balance << "\n";
    }

    void addInterest()
    {
        double interest = balance * interestRate;
        balance += interest;
        transactions.push_back(interest);
        cout << "  Interest Rs " << fixed << setprecision(2) << interest << " added. Balance: Rs " << balance << "\n";
    }

    void showAccount()
    {
        cout << "\n  === SAVINGS ACCOUNT ===\n";
        cout << "  ID        : " << accountId << "\n";
        cout << "  Name      : " << ownerName << "\n";
        cout << "  Balance   : Rs " << fixed << setprecision(2) << balance << "\n";
        cout << "  Interest  : " << interestRate * 100 << "%\n";
        cout << "  Access    : ";
        if (permissions & 2) cout << "[Deposit] ";
        if (permissions & 1) cout << "[Withdraw] ";
        if (permissions & 4) cout << "[Transfer] ";
        if (permissions & 8) cout << "[VIP] ";
        cout << "\n";
    }

    void saveToFile(ofstream& f)
    {
        f << "SAVINGS\n" << accountId << "\n" << ownerName << "\n";
        f << balance << "\n" << permissions << "\n" << interestRate << "\n";
        f << transactions.size() << "\n";
        for (size_t i = 0; i < transactions.size(); i++)
            f << transactions[i] << "\n";
    }
};


class CurrentAccount : public Account
{
private:
    double overdraftLimit;

public:
    CurrentAccount(int id, string name, double startBalance, unsigned int perms = 1 | 2 | 4)
    {
        accountId      = id;
        ownerName      = name;
        balance        = startBalance;
        overdraftLimit = 10000;
        permissions    = perms;
    }

    string getType() { return "CURRENT"; }

    void deposit(double amount)
    {
        if (!(permissions & 2)) { cout << "  Deposit not permitted.\n"; return; }
        if (amount <= 0)        { cout << "  Enter a valid amount.\n";  return; }
        balance += amount;
        transactions.push_back(amount);
        cout << "  Rs " << fixed << setprecision(2) << amount << " deposited. Balance: Rs " << balance << "\n";
    }

    void withdraw(double amount)
    {
        if (!(permissions & 1))               { cout << "  Withdrawal not permitted.\n"; return; }
        if (amount <= 0)                      { cout << "  Enter a valid amount.\n";     return; }
        if (amount > balance + overdraftLimit) { cout << "  Exceeds overdraft limit.\n"; return; }
        balance -= amount;
        transactions.push_back(-amount);
        cout << "  Rs " << fixed << setprecision(2) << amount << " withdrawn.\n";
        if (balance < 0)
            cout << "  Warning: Account in overdraft. Balance: Rs " << balance << "\n";
        else
            cout << "  Balance: Rs " << balance << "\n";
    }

    void showAccount()
    {
        cout << "\n  === CURRENT ACCOUNT ===\n";
        cout << "  ID            : " << accountId << "\n";
        cout << "  Name          : " << ownerName << "\n";
        cout << "  Balance       : Rs " << fixed << setprecision(2) << balance << "\n";
        cout << "  Overdraft Cap : Rs " << overdraftLimit << "\n";
        cout << "  Access        : ";
        if (permissions & 2) cout << "[Deposit] ";
        if (permissions & 1) cout << "[Withdraw] ";
        if (permissions & 4) cout << "[Transfer] ";
        if (permissions & 8) cout << "[VIP] ";
        cout << "\n";
    }

    void saveToFile(ofstream& f)
    {
        f << "CURRENT\n" << accountId << "\n" << ownerName << "\n";
        f << balance << "\n" << permissions << "\n" << overdraftLimit << "\n";
        f << transactions.size() << "\n";
        for (size_t i = 0; i < transactions.size(); i++)
            f << transactions[i] << "\n";
    }
};


Account* findById(vector<Account*>& accounts, int id)
{
    for (size_t i = 0; i < accounts.size(); i++)
        if (accounts[i]->getId() == id)
            return accounts[i];
    return NULL;
}


void transferFunds(vector<Account*>& accounts)
{
    int fromId, toId;
    double amount;
    cout << "  From Account ID: "; cin >> fromId;
    cout << "  To Account ID  : "; cin >> toId;

    Account* from = findById(accounts, fromId);
    Account* to   = findById(accounts, toId);

    if (!from || !to)     { cout << "  One or both accounts not found.\n"; return; }
    if (!(from->getPermissions() & 4)) { cout << "  Transfer not permitted on source account.\n"; return; }

    cout << "  Amount to transfer: "; cin >> amount;

    if (amount <= 0)                       { cout << "  Invalid amount.\n"; return; }
    if (amount > from->getBalance()) { cout << "  Insufficient balance.\n"; return; }

    from->getTransactions().push_back(-amount);
    to->getTransactions().push_back(amount);

    double newFrom = from->getBalance() - amount;
    double newTo   = to->getBalance()   + amount;

    // update balances via deposit/withdraw is cleaner but we use direct push here
    // so we manually adjust
    from->getTransactions().pop_back();
    to->getTransactions().pop_back();

    from->withdraw(amount);
    to->deposit(amount);
}


void showMonthlySummary(vector<Account*>& accounts)
{
    double monthlyNet[12] = {0};
    string months[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

    for (size_t i = 0; i < accounts.size(); i++)
    {
        vector<double>& txns = accounts[i]->getTransactions();
        for (size_t j = 0; j < txns.size(); j++)
            monthlyNet[j % 12] += txns[j];
    }

    cout << "\n  === MONTHLY SUMMARY ===\n";
    for (int i = 0; i < 12; i++)
        cout << "  " << months[i] << " : Rs " << fixed << setprecision(2) << monthlyNet[i] << "\n";
}


void saveAll(vector<Account*>& accounts)
{
    ofstream file("bank_data.txt");
    if (!file) { cout << "  Could not open file.\n"; return; }
    file << accounts.size() << "\n";
    for (size_t i = 0; i < accounts.size(); i++)
        accounts[i]->saveToFile(file);
    file.close();

    ifstream rf("bank_data.txt", ios::binary);
    string raw((istreambuf_iterator<char>(rf)), istreambuf_iterator<char>());
    rf.close();

    string encrypted = xorCipher(raw);
    ofstream wf("bank_data.txt", ios::binary);
    wf.write(encrypted.c_str(), encrypted.size());
    wf.close();

    cout << "  Data saved and encrypted to bank_data.txt\n";
}


void loadAll(vector<Account*>& accounts)
{
    for (size_t i = 0; i < accounts.size(); i++)
        delete accounts[i];
    accounts.clear();

    ifstream file("bank_data.txt", ios::binary);
    if (!file) { cout << "  No saved data found.\n"; return; }

    string encrypted((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    string raw = xorCipher(encrypted);
    istringstream ss(raw);

    int count; ss >> count; ss.ignore();

    for (int i = 0; i < count; i++)
    {
        string type; getline(ss, type);
        int id, txnCount;
        string name;
        double bal, extra;
        unsigned int perms;

        ss >> id; ss.ignore();
        getline(ss, name);
        ss >> bal >> perms >> extra >> txnCount; ss.ignore();

        Account* acc = NULL;

        if (type == "SAVINGS")
        {
            SavingsAccount* sa = new SavingsAccount(id, name, bal, perms);
            for (int j = 0; j < txnCount; j++)
            {
                double t; ss >> t; ss.ignore();
                sa->getTransactions().push_back(t);
            }
            acc = sa;
        }
        else if (type == "CURRENT")
        {
            CurrentAccount* ca = new CurrentAccount(id, name, bal, perms);
            for (int j = 0; j < txnCount; j++)
            {
                double t; ss >> t; ss.ignore();
                ca->getTransactions().push_back(t);
            }
            acc = ca;
        }

        if (acc) accounts.push_back(acc);
    }

    cout << "  Loaded " << accounts.size() << " account(s).\n";
}


void adminMenu(vector<Account*>& accounts)
{
    string pass;
    cout << "  Admin Password: ";
    cin >> pass;

    if (pass != "admin123")
    {
        cout << "  Wrong password.\n";
        return;
    }

    int choice;
    cout << "\n  --- ADMIN MENU ---\n";
    cout << "  1. View All Accounts\n";
    cout << "  2. Grant VIP to Account\n";
    cout << "  3. Revoke Permission from Account\n";
    cout << "  0. Back\n";
    cout << "  Choice: "; cin >> choice;

    if (choice == 1)
    {
        for (size_t i = 0; i < accounts.size(); i++)
            accounts[i]->showAccount();
    }
    else if (choice == 2)
    {
        int id; cout << "  Account ID: "; cin >> id;
        Account* acc = findById(accounts, id);
        if (!acc) { cout << "  Not found.\n"; return; }
        acc->setPermissions(acc->getPermissions() | 8);
        cout << "  VIP granted.\n";
    }
    else if (choice == 3)
    {
        int id; cout << "  Account ID: "; cin >> id;
        Account* acc = findById(accounts, id);
        if (!acc) { cout << "  Not found.\n"; return; }
        cout << "  Remove: 1=Withdraw 2=Deposit 4=Transfer\n";
        unsigned int flag; cout << "  Flag: "; cin >> flag;
        acc->setPermissions(acc->getPermissions() & ~flag);
        cout << "  Permission removed.\n";
    }
}


int main()
{
    vector<Account*> accounts;
    int nextId = 1001;
    int choice;

    cout << "\n  ============================\n";
    cout << "     BANK MANAGEMENT SYSTEM\n";
    cout << "  ============================\n";

    do
    {
        cout << "\n  --- MENU ---\n";
        cout << "  1.  Create Account\n";
        cout << "  2.  Deposit\n";
        cout << "  3.  Withdraw\n";
        cout << "  4.  View Account\n";
        cout << "  5.  Transaction History\n";
        cout << "  6.  Add Interest (Savings)\n";
        cout << "  7.  Transfer Funds\n";
        cout << "  8.  Monthly Summary\n";
        cout << "  9.  Save to File\n";
        cout << "  10. Load from File\n";
        cout << "  11. Show All Accounts\n";
        cout << "  12. Admin Panel\n";
        cout << "  0.  Exit\n";
        cout << "  Choice: "; cin >> choice;

        if (choice == 1)
        {
            int type;
            cout << "  1. Savings  2. Current\n  Choose: "; cin >> type;
            string name; double bal;
            cout << "  Name: "; cin.ignore(); getline(cin, name);
            cout << "  Opening Balance: "; cin >> bal;

            Account* acc = NULL;
            if (type == 1)      { acc = new SavingsAccount(nextId, name, bal); cout << "  Savings account created.\n"; }
            else if (type == 2) { acc = new CurrentAccount(nextId, name, bal); cout << "  Current account created.\n"; }
            else                { cout << "  Invalid type.\n"; }

            if (acc) { accounts.push_back(acc); cout << "  Account ID: " << nextId++ << "\n"; }
        }
        else if (choice == 2)
        {
            int id; double amount;
            cout << "  Account ID: "; cin >> id;
            Account* acc = findById(accounts, id);
            if (!acc) { cout << "  Not found.\n"; continue; }
            cout << "  Amount: "; cin >> amount;
            acc->deposit(amount);
        }
        else if (choice == 3)
        {
            int id; double amount;
            cout << "  Account ID: "; cin >> id;
            Account* acc = findById(accounts, id);
            if (!acc) { cout << "  Not found.\n"; continue; }
            cout << "  Amount: "; cin >> amount;
            acc->withdraw(amount);
        }
        else if (choice == 4)
        {
            int id; cout << "  Account ID: "; cin >> id;
            Account* acc = findById(accounts, id);
            if (!acc) cout << "  Not found.\n";
            else acc->showAccount();
        }
        else if (choice == 5)
        {
            int id; cout << "  Account ID: "; cin >> id;
            Account* acc = findById(accounts, id);
            if (!acc) cout << "  Not found.\n";
            else acc->showHistory();
        }
        else if (choice == 6)
        {
            int id; cout << "  Savings Account ID: "; cin >> id;
            Account* acc = findById(accounts, id);
            if (!acc) { cout << "  Not found.\n"; continue; }
            SavingsAccount* sa = dynamic_cast<SavingsAccount*>(acc);
            if (sa) sa->addInterest();
            else    cout << "  Not a savings account.\n";
        }
        else if (choice == 7)  transferFunds(accounts);
        else if (choice == 8)
        {
            if (accounts.empty()) cout << "  No accounts yet.\n";
            else showMonthlySummary(accounts);
        }
        else if (choice == 9)  saveAll(accounts);
        else if (choice == 10) loadAll(accounts);
        else if (choice == 11)
        {
            if (accounts.empty()) cout << "  No accounts.\n";
            else for (size_t i = 0; i < accounts.size(); i++) accounts[i]->showAccount();
        }
        else if (choice == 12) adminMenu(accounts);
        else if (choice != 0)  cout << "  Invalid choice.\n";

    } while (choice != 0);

    for (size_t i = 0; i < accounts.size(); i++)
        delete accounts[i];
    accounts.clear();

    cout << "\n  Goodbye!\n\n";
    return 0;
}
