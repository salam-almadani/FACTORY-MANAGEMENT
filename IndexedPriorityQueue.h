#pragma once
#include "Order.h"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <stdexcept>
using namespace std;

class IndexedPriorityQueue
{
private:
    vector<Order> heap;
    unordered_map<int, int> positions;

    void swapElements(int i, int j)
    {
        positions[heap[i].getID()] = j;
        positions[heap[j].getID()] = i;
        swap(heap[i], heap[j]);
    }

    void BubbleUp(int idx)
    {
        while (idx > 0)
        {
            int parent = (idx - 1) / 2;

            if (heap[idx].isBetterThan(heap[parent]))
            {
                swapElements(idx, parent);
                idx = parent;
            }
            else
            {
                break;
            }
        }
    }

    void BubbleDown(int index)
    {
        int size = heap.size();
        while (true)
        {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int best = index;

            if (left < size && heap[left].isBetterThan(heap[best]))
                best = left;
            if (right < size && heap[right].isBetterThan(heap[best]))
                best = right;

            if (best != index)
            {
                swapElements(index, best);
                index = best;
            }
            else
            {
                break;
            }
        }
    }

public:
    void addOrder(Order order)
    {
        heap.push_back(order);
        int index = heap.size() - 1;
        positions[order.getID()] = index;
        BubbleUp(index);
    }

    Order processNextOrder()
    {
        if (heap.empty())
        {
            throw runtime_error("No orders");
        }
        Order top = heap[0];
        remove(top.getID());
        return top;
    }

    void updatePriority(int id, int newPriority)
    {
        if (positions.find(id) == positions.end())
        {
            cout << "No order with this ID was found.\n";
            return;
        }

        int index = positions[id];
        int oldPriority = heap[index].getPriority();
        heap[index].setPriority(newPriority);

        if (newPriority > oldPriority)
            BubbleUp(index);
        else
            BubbleDown(index);
    }

    void remove(int id)
    {
        if (positions.find(id) == positions.end())
        {
            cout << "No order with this ID was found.\n";
            return;
        }

        int index = positions[id];
        int lastIndex = heap.size() - 1;

        if (index != lastIndex)
        {
            swapElements(index, lastIndex);
            positions.erase(id);
            heap.pop_back();
            BubbleUp(index);
            BubbleDown(index);
        }
        else
        {
            positions.erase(id);
            heap.pop_back();
        }
    }

    Order &find(int id){
        if(positions.find(id) != positions.end()){
        return heap[positions[id]];
        }
        else {
            throw runtime_error("No order with ID " + to_string(id) + " was found!");
        }
    }

    Order peekNext()
    {
        if (heap.empty())
        {
            throw runtime_error("No orders");
        }
        return heap[0];
    }

    bool isEmpty() { return heap.empty(); }

    int size(){
        return heap.size();
    }
};
