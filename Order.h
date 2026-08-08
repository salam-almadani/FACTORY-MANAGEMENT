#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
using namespace std;

class Order
{
private:
    static int cnt;
    int orderID;
    string productType;
    int quantity;
    int priority;

public:
    Order(string t, int q, int pr)
    {
        this->orderID = cnt++;
        this->productType = t;
        this->quantity = q;
        this->priority = pr;
    }

    bool isBetterThan(const Order &order) const
    {
        if (this->priority != order.priority)
        {
            return this->priority > order.priority;
        }
        return this->orderID < order.orderID;
    }
    int getID() const { return orderID; }
    int getPriority() const { return priority; }
    void setPriority(int pr) { priority = pr; }
    void setProductType(string type) { productType = type; }
    void setQuantity(int quan){ quantity = quan; }
    string getProductType() const { return productType; }
    int getQuantity() const { return quantity; }
};

int Order::cnt = 1;
