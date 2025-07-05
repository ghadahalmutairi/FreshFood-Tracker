#include <iostream>
#include <string>
#include <ctime>

using namespace std;

const int MAX_SIZE = 100;

class FoodItem {
public:
    string name;
    string expiryDate;
    int quantity;
};

class Recipe {
public:
    string name;
    char ingredients[3][20];
};

class Fridge {
private:
    FoodItem items[MAX_SIZE];
    int front = 0;
    int rear = 0;

public:
    bool isEmpty() {
        return front == rear;
    }

    bool isFull() {
        return (rear + 1) % MAX_SIZE == front;
    }

    void addItem(const FoodItem& item) {
        if (!isFull()) {
            items[rear] = item;
            rear = (rear + 1) % MAX_SIZE;
        }
    }

    FoodItem removeItem() {
        FoodItem item = items[front];
        front = (front + 1) % MAX_SIZE;
        return item;
    }

    FoodItem peek() {
        return items[front];
    }

    int size() {
        return (rear - front + MAX_SIZE) % MAX_SIZE;
    }

    void listItems(const string& currentDate) {
        cout << "\nFridge Contents:\n";
        for (int i = front; i != rear; i = (i + 1) % MAX_SIZE) {
            FoodItem item = items[i];
            int daysLeft = getDaysLeft(item.expiryDate, currentDate);
            cout << "- " << item.name << " (Expiry: " << item.expiryDate << ") - " << daysLeft << " days left\n";
        }
    }

    bool hasIngredient(const string& ingredient) {
        for (int i = front; i != rear; i = (i + 1) % MAX_SIZE) {
            if (items[i].name == ingredient)
                return true;
        }
        return false;
    }

    void checkExpiry(class ExpiredItems& expiredStack, const string& currentDate);

    int getDaysLeft(const string& expiryDate, const string& currentDate) {
        struct tm expTime = {}, curTime = {};
        strptime(expiryDate.c_str(), "%Y-%m-%d", &expTime);
        strptime(currentDate.c_str(), "%Y-%m-%d", &curTime);
        time_t expT = mktime(&expTime);
        time_t curT = mktime(&curTime);
        return difftime(expT, curT) / (60 * 60 * 24);
    }

    friend class ExpiredItems;
};

class ExpiredItems {
private:
    FoodItem items[MAX_SIZE];
    int top = -1;

public:
    void push(FoodItem item) {
        if (top < MAX_SIZE - 1)
            items[++top] = item;
    }

    void showItems() {
        if (top == -1) {
            cout << "\nNo expired items.\n";
            return;
        }

        cout << "\nExpired Items:\n";
        for (int i = top; i >= 0; --i) {
            cout << "- " << items[i].name << " (Expired on: " << items[i].expiryDate << ")\n";
        }
    }
};

void Fridge::checkExpiry(ExpiredItems& expiredStack, const string& currentDate) {
    Fridge temp;

    while (!isEmpty()) {
        FoodItem item = removeItem();
        int daysLeft = getDaysLeft(item.expiryDate, currentDate);

        if (daysLeft <= 0) {
            expiredStack.push(item);
            cout << "Expired: " << item.name << " (Expired on: " << item.expiryDate << ")\n";
        } else {
            temp.addItem(item);
        }
    }

    *this = temp;

  
    cout << "\nItems Expiring Soon:\n";
    for (int i = front; i != rear; i = (i + 1) % MAX_SIZE) {
        FoodItem item = items[i];
        int daysLeft = getDaysLeft(item.expiryDate, currentDate);
        cout << "- " << item.name << " in " << daysLeft << " days\n";
    }
}

class RecipeManager {
public:
    void suggestRecipes(Recipe recipes[], int recipeCount, Fridge& fridge) {
        cout << "\nSuggested Recipes:\n";
        for (int i = 0; i < recipeCount; i++) {
            bool allAvailable = true;
            for (int j = 0; j < 3; j++) {
                if (!fridge.hasIngredient(recipes[i].ingredients[j])) {
                    allAvailable = false;
                    break;
                }
            }
            if (allAvailable) {
                cout << "- " << recipes[i].name << "\n";
            }
        }
    }
};

string getCurrentDate() {
    time_t now = time(0);
    struct tm* current = localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", current);
    return string(buffer);
}

int main() {
    Fridge fridge;
    ExpiredItems expiredStack;
    RecipeManager recipeManager;

    int choice;
    string name, expiryDate;
    int quantity;

    Recipe recipes[3] = {
        {"Omelette", {"egg", "cheese", "milk"}},
        {"Salad", {"lettuce", "tomato", "cucumber"}},
        {"Sandwich", {"bread", "cheese", "chicken"}}
    };

    do {
        cout << "\nFreshFood Tracker Menu\n";
        cout << "1. Add Item\n2. Check Expiry\n3. Display Fridge\n";
        cout << "4. Show Expired Items\n5. Suggest Recipes\n6. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Item name: ";
                cin >> name;
                cout << "Expiry Date (yyyy-mm-dd): ";
                cin >> expiryDate;
                cout << "Quantity: ";
                cin >> quantity;
                fridge.addItem({name, expiryDate, quantity});
                break;
            case 2:
                fridge.checkExpiry(expiredStack, getCurrentDate());
                break;
            case 3:
                fridge.listItems(getCurrentDate());
                break;
            case 4:
                expiredStack.showItems();
                break;
            case 5:
                recipeManager.suggestRecipes(recipes, 3, fridge);
                break;
        }

    } while (choice != 6);

    return 0;
}

