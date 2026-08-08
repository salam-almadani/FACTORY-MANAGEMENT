class ProductionUnit
{
private:
    static int cnt;
    int unitID;
    string productType;
    string status;
    IndexedPriorityQueue localOrders;
    unordered_map<string, int> localWarehouse;
    vector<string> events;

public:
    ProductionUnit() : unitID(0), productType(""), status("Ready") {}
    ProductionUnit(string t)
    {
        this->unitID = cnt++;
        this->productType = t;
        status = "Ready";
    }

    void receiveOrder(const Order &o)
    {
        localOrders.addOrder(o);
        setStatus("Busy");
        events.push_back("Received order ID: " + to_string(o.getID()));
    }

    bool hasEnoughMaterial(string materialName, int quan)
    {
        if (localWarehouse.find(materialName) != localWarehouse.end())
        {
            return localWarehouse[materialName] >= quan;
        }
        return false;
    }

    void addMaterialToLocalStore(string name, int quan)
    {
        localWarehouse[name] += quan;
    }

    bool processCycle(vector<string> requiredMaterials)
    {
        if (status == "Broken")
        {
            cout << "Unit " << unitID << " is BROKEN and cannot process orders.\n";
            return false;
        }

        if (localOrders.isEmpty())
        {
            if (status != "Ready")
                setStatus("Ready");
            return false;
        }

        Order o = localOrders.peekNext();
        bool allAvailable = true;
        for (const string &mat : requiredMaterials)
        {
            if (!hasEnoughMaterial(mat, o.getQuantity()))
            {
                allAvailable = false;

                if (status != "Waiting for Materials")
                {
                    setStatus("Waiting for Materials");
                    cout << "Unit " << unitID << ": Waiting for " << mat << " to finish order " << o.getID() << "\n";
                }
                break;
            }
        }

        if (allAvailable)
        {
            for (const string &mat : requiredMaterials)
            {
                localWarehouse[mat] -= o.getQuantity();
            }

            cout << "Unit " << unitID << ": Processed Order " << o.getID() << " (" << o.getProductType() << ")\n";
            events.push_back("Processed order " + to_string(o.getID()));

            if (localOrders.isEmpty())
                setStatus("Ready");
            else
                setStatus("Busy");

            return true;
        }

        return false;
    }

    void printEvents()
    {
        if (events.empty())
        {
            cout << "There are no events related to this unit.\n";
            return;
        }
        cout << "\n--- Unit " << this->unitID << " Events ---\n";
        int c = 1;
        for (const string &s : events)
        {
            cout << c++ << ". " << s << endl;
        }
        cout << "----------------------------------\n";
    }

    void setStatus(const string &newStatus) { status = newStatus; }
    void setRawQuantity(const string &matirial, int quan)
    {
        localWarehouse[matirial] = quan;
    }
    string getStatus() const { return status; }
    string getProductType() const { return productType; }
    int getUnitID() const { return unitID; }
    int getRawQuantity(string s) { return localWarehouse[s]; }
    IndexedPriorityQueue *getLocalOrders()
    {
        return &localOrders;
    }
};

int ProductionUnit::cnt = 1;
