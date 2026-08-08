#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <stdexcept>
#include "FactoryManager.h"
using namespace std;

int main()
{
    FactoryManager factoryManager;

    factoryManager.registerUnit(ProductionUnit("Phone"));
    factoryManager.registerUnit(ProductionUnit("Laptop"));

    factoryManager.addRawMaterials("Phone_Kit", 100);
    factoryManager.addRawMaterials("Motherboard", 50);

    int userChoice = -1;
    while (userChoice != 0)
    {
        try
        {
            cout << "\n---------- FACTORY MANAGEMENT SYSTEM ----------" << endl;
            cout << "1. Create New Customer Order (Phone/Laptop)" << endl;
            cout << "2. Assign Pending Orders (Start Process)" << endl;
            cout << "3. Execute Production Cycle (Work on Orders)" << endl;
            cout << "4. Add Raw Materials (Phone_Kit/Motherboard/Battery/Keyboard/Screen)" << endl;
            cout << "5. Shipping and Delivery" << endl;
            cout << "6. Report Unit Failure" << endl;
            cout << "7. View Stock & Unit Status" << endl;
            cout << "8. repair Unit" << endl;
            cout << "9. request For Order Modification" << endl;
            cout << "10.request To Delete An Order" << endl;
            cout << "11.View Production Unit Events" << endl;
            cout << "0. Exit System" << endl;
            cout << "-------------------------------------------------" << endl;
            cout << "Select option: ";
            cin >> userChoice;

            switch (userChoice)
            {
            case 1:
            {
                int q, p;
                string t;
                cout << "Product Type (Phone/Laptop): ";
                cin >> t;
                cout << "Quantity: ";
                cin >> q;
                cout << "Priority: ";
                cin >> p;
                factoryManager.addNewOrder(Order(t, q, p));
                break;
            }
            case 2:
            {
                factoryManager.distributionOrder();
                break;
            }
            case 3:
            {
                factoryManager.runProductionCycle();
                break;
            }
            case 4:
            {
                string n;
                int q;
                cout << "Material Name (Phone_Kit/Motherboard/Keyboard/Battery/Screen): ";
                cin >> n;
                cout << "Quantity: ";
                cin >> q;
                factoryManager.addRawMaterials(n, q);
                break;
            }
            case 5:
            {
                string n;
                int q;
                cout << "Product to Ship: ";
                cin >> n;
                cout << "Quantity: ";
                cin >> q;
                factoryManager.shipFinishedProducts(n, q);
                break;
            }
            case 6:
            {
                int id;
                cout << "Broken Unit ID: ";
                cin >> id;
                factoryManager.reportFailure(id);
                break;
            }
            case 7:
            {
                factoryManager.printStock();
                factoryManager.printUnitStatus();
                break;
            }
            case 8:
            {
                int id;
                cout << "Enter unit ID to repair: ";
                cin >> id;
                factoryManager.repairUnit(id);
                break;
            }
            case 9:
            {
                int id;
                cout << "Enter Order ID To Modify: ";
                cin >> id;
                factoryManager.requestForOrderModification(id);
                break;
            }
            case 10:
            {
                int id;
                cout << "Enter Order ID To Delete: " << endl;
                cin >> id;
                factoryManager.requestToDeleteAnOrder(id);
                break;
            }
            case 11:
            {
                int id;
                cout << "Enter Unit ID to print events: " << endl;
                cin >> id;
                factoryManager.printProductionUnitEvents(id);
                break;
            }
            case 0:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid selection!\n";
            }
        }
        catch (const exception &e)
        {
            cout << "\n[!] ERROR: " << e.what() << endl;
            cout << "Please try again with valid data.\n";
            cin.clear();
            cin.ignore(1000, '\n');
        }
    }

    return 0;
}

