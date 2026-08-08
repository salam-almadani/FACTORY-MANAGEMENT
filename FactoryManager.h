#pragma once
#include "IndexedPriorityQueue.h"
#include "ProductionUnit.h"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
using namespace std;

class FactoryManager
{
private:
    unordered_map<string, queue<int>> productSpecializedUnits;
    unordered_map<int, ProductionUnit> units;
    IndexedPriorityQueue pendingOrders;
    unordered_map<string, int> rawMaterialsStock;
    unordered_map<string, int> finishedProductsStock;
    unordered_map<string, vector<string>> productRecipes;

public:
    FactoryManager()
    {
        productRecipes["Phone"] = {"Phone_Kit", "screen", "battery"};
        productRecipes["Laptop"] = {"Motherboard", "screen", "keyboard"};
        rawMaterialsStock["screen"] = 100;
        rawMaterialsStock["battery"] = 100;
        rawMaterialsStock["Phone_Kit"] = 100;
        rawMaterialsStock["Motherboard"] = 100;
        rawMaterialsStock["keyboard"] = 100;
    }

    void shipFinishedProducts(const string &productName, int quantity)
    {
        if (finishedProductsStock.count(productName) && finishedProductsStock[productName] >= quantity)
        {
            finishedProductsStock[productName] -= quantity;
            cout << "SUCCESS: Shipped " << quantity << " of " << productName << ". Remaining: " << finishedProductsStock[productName] << endl;
        }
        else
        {
            cout << "ERROR: Not enough stock to ship " << productName << " (Available: " << finishedProductsStock[productName] << ")" << endl;
        }
    }

    void registerUnit(ProductionUnit unitObj)
    {
        int id = unitObj.getUnitID();
        productSpecializedUnits[unitObj.getProductType()].push(id);
        units[id] = unitObj;
    }

    void addNewOrder(const Order &newOrder)
    {
        if (productRecipes.find(newOrder.getProductType()) == productRecipes.end())
        {
            cout << "Error: Unknown product type " << newOrder.getProductType() << "\n";
            return;
        }
        pendingOrders.addOrder(newOrder);
        cout << "Order added to pending queue.\n";
    }

    void runProductionCycle()
    {
        cout << "\n--- Running Production Cycle ---\n";
        bool workDone = false;
        for (auto &pair : units)
        {
            ProductionUnit &unit = pair.second;
            vector<string> neededMaterial = productRecipes[unit.getProductType()];
            if (unit.processCycle(neededMaterial))
            {
                Order finishedOrder = unit.getLocalOrders()->processNextOrder();
                storeFinishedProduct(finishedOrder.getProductType(), finishedOrder.getQuantity());
                workDone = true;
            }
        }
        if (!workDone)
        {
            cout << "No orders completed in this cycle (Units might be waiting for materials.).\n";
        }
        cout << "----------------------------------\n";
    }

    void distributionOrder()
    {
        if (pendingOrders.isEmpty())
        {
            cout << "No pending orders to distribute." << endl;
            return;
        }

        vector<Order> skippedOrders;
        bool assignedAny = false;

        while (!pendingOrders.isEmpty())
        {
            Order order = pendingOrders.peekNext();
            string product = order.getProductType();
            vector<string> neededMaterial = productRecipes[product];
            bool assignedThisOrder = false;

            if (productSpecializedUnits.count(product))
            {
                int unitsCount = productSpecializedUnits[product].size();
                for (int i = 0; i < unitsCount; i++)
                {
                    int unitID = productSpecializedUnits[product].front();
                    productSpecializedUnits[product].pop();
                    productSpecializedUnits[product].push(unitID);

                    ProductionUnit &u = units[unitID];
                    if (u.getStatus() == "Ready" || u.getStatus() == "Waiting for Materials")
                    {
                        int qtyNeeded = order.getQuantity();
                        bool allMaterialsAvailable = true;
                        for (const string &mat : neededMaterial)
                        {
                            int currentInUnit = u.getRawQuantity(mat);
                            int missingQty = qtyNeeded - currentInUnit;
                            if (missingQty > 0 && rawMaterialsStock[mat] < missingQty)
                            {
                                allMaterialsAvailable = false;
                                break;
                            }
                        }

                        if (allMaterialsAvailable)
                        {
                            for (const string &mat : neededMaterial)
                            {
                                int currentInUnit = u.getRawQuantity(mat);
                                int missingQty = qtyNeeded - currentInUnit;
                                if (missingQty > 0)
                                {
                                    rawMaterialsStock[mat] -= missingQty;
                                    u.addMaterialToLocalStore(mat, missingQty);
                                    cout << "Transferred " << missingQty << " " << mat << " to Unit " << unitID << endl;
                                }
                            }

                            u.receiveOrder(order);
                            pendingOrders.processNextOrder();
                            assignedThisOrder = true;
                            assignedAny = true;
                            cout << "Order " << order.getID() << " assigned to unit " << unitID << endl;
                            break;
                        }
                    }
                }
            }

            if (assignedThisOrder)
                continue;

            skippedOrders.push_back(order);
            pendingOrders.processNextOrder();
        }

        for (const auto &o : skippedOrders)
        {
            pendingOrders.addOrder(o);
        }

        if (!assignedAny)
        {
            cout << "Could not assign any orders due to material shortage or busy units.\n";
        }
    }

void reportFailure(int unitID) {
    ProductionUnit &unit = units[unitID];

    vector<string> materials = productRecipes[unit.getProductType()];

    for (const string& mat : materials) {
        int qtyToReturn = unit.getRawQuantity(mat);
        rawMaterialsStock[mat] += qtyToReturn;
        unit.addMaterialToLocalStore(mat, -qtyToReturn);
    }

    if (!unit.getLocalOrders()->isEmpty()) {
        Order currentOrder = unit.getLocalOrders()->processNextOrder();
        pendingOrders.addOrder(currentOrder);
        cout << "Order " << currentOrder.getID() << " returned to pending queue." << endl;
    }
    cout << "Unit " << unitID << " failed. All materials returned to stock." << endl;
    unit.setStatus("Broken");
}

    void repairUnit(int unitID)
    {
        if (units.count(unitID))
        {
            if (units[unitID].getStatus() == "Broken")
            {
                units[unitID].setStatus("Ready");
                cout << "Unit " << unitID << " has been REPAIRED and is now Ready.\n";
            }
            else
            {
                cout << "Unit " << unitID << " is not broken.\n";
            }
        }
        else
        {
            cout << "Unit ID " << unitID << " not found.\n";
        }
    }

    void addRawMaterials(string material, int qty)
    {
        rawMaterialsStock[material] += qty;
        cout << "Added " << qty << " units of " << material << " to Raw Materials Stock.\n";
    }

    void storeFinishedProduct(string product, int qty)
    {
        finishedProductsStock[product] += qty;
        cout << ">> Stock Update: + " << qty << " " << product << " added to Finished Goods.\n";
    }

    void printStock()
    {
        cout << "\n--- WAREHOUSE STATUS ---\n";
        cout << "Raw Materials:\n";
        for (auto &p : rawMaterialsStock)
            cout << "  - " << p.first << ": " << p.second << endl;
        cout << "Finished Products:\n";
        for (auto &p : finishedProductsStock)
            cout << "  - " << p.first << ": " << p.second << endl;
        cout << "------------------------\n";
    }

    void printUnitStatus()
    {
        cout << "\n--- UNITS STATUS ---\n";
        for (auto &p : units)
        {
            cout << "Unit " << p.first << " [" << p.second.getProductType() << "]: " << p.second.getStatus() << endl;
        }
        cout << "--------------------\n";
    }

    void requestToDeleteAnOrder(int id)
    {
        pendingOrders.remove(id);
    }

    void requestForOrderModification(int id)
    {
        cout << "what do you want to change ?\n";
        cout << "1.Product Type.\n";
        cout << "2.Order Priority.\n";
        cout << "3.Order Quantity.\n";
        int choice = -1;
        cin >> choice;
        if (choice == 1)
        {
            string str;
            cout << "Enter new Product Type.\n";
            cin >> str;
            pendingOrders.find(id).setProductType(str);
            cout << "Order " << id << " was modified sucessfully!\n";
        }

        else if (choice == 2)
        {
            int pr;
            cout << "Enter new priority.\n";
            cin >> pr;
            pendingOrders.updatePriority(id, pr);
            cout << "Order " << id << " was modified sucessfully!\n";
        }
        else if (choice == 3)
        {
            int q;
            cout << "Enter new quantity.\n";
            cin >> q;
            pendingOrders.find(id).setQuantity(q);
            cout << "Order " << id << " was modified sucessfully!\n";
        }
        else
        {
            cout << "Invalid selection!\n please try again!\n\n";
            requestForOrderModification(id);
        }
        char str;
        cout << "Do you want to change anything else? (y/n)\n";
        cin >> str;
        if (str == 'y')
        {
            requestForOrderModification(id);
        }
        else if (str != 'n')
        {
            cout << "Invalid selection!\n";
        }
        return;
    }

    void printProductionUnitEvents(int id)
    {
        if (units.find(id) != units.end())
        {
            units[id].printEvents();
        }
        else
        {
            cout << "No unit with this ID found!" << endl;
        }
    }
};
