// CMSC 341 - Fall 2023 - Project 4
#include "dealer.h"
CarDB::CarDB(int size, hash_fn hash, prob_t probing = DEFPOLCY){\
    if (size < MINPRIME) {
        m_currentCap = MINPRIME;
    }
    else if (size > MAXPRIME) {
        m_currentCap = MAXPRIME;
    }
    else if (!isPrime(size)) {
        m_currentCap = findNextPrime(size);
    }
    else {
        m_currentCap = size;
    }
    m_hash = hash;
    m_currProbing = probing;
    m_currentSize = 0;
    m_currNumDeleted = 0;
    m_oldTable = nullptr;
    m_oldCap = 0;
    m_oldSize = 0;
    m_oldNumDeleted = 0;
    m_newPolicy = DEFPOLCY;
    m_oldProbing = DEFPOLCY;

    m_currentTable = new Car[m_currentCap];
    for (int i = 0; i < m_currentCap; i++) {
        Car emptyCar;
        m_currentTable[i] = emptyCar;
    }
}

CarDB::~CarDB(){
    delete [] m_currentTable;

    if (m_oldTable != nullptr) {
        delete [] m_oldTable;
    }
}

void CarDB::changeProbPolicy(prob_t policy){
    m_newPolicy = policy;
}

void CarDB::newInsert(Car car) {
    int index = m_hash(car.m_model) % m_currentCap;

    if (m_currentTable[index].m_used == false) {
        if (m_currentTable[index].m_model == "") m_currentSize++;
        m_currentTable[index] = car;
    }
    else {
        int i = 0;

        while (m_currentTable[index].m_used == true) {
            if (m_currProbing == DOUBLEHASH) {
                index = ((m_hash(car.m_model) % m_currentCap) + i * (11-(m_hash(car.m_model) % 11))) % m_currentCap;
                i++;
            }
            else if (m_currProbing == QUADRATIC) {
                index = ((m_hash(car.m_model) % m_currentCap) + (i * i)) % m_currentCap;
                i++;
            }
        }
        if (m_currentTable[index].m_model == "") m_currentSize++;
        m_currentTable[index] = car;
    }
}

bool CarDB::insert(Car car){

    // case of duplicate, but still continues to remake the table
    if (getCar(car.m_model, car.m_dealer) == car) {
        if (m_oldTable) {
            remakeTable();
        }
        return false; 
    }

    // if ID is not in the correct range
    if (car.m_dealer < MINID || car.m_dealer > MAXID) {
        if (m_oldTable) {
            remakeTable();
        }
        return false;   
    }

    newInsert(car);

    // rehashes table if load factor is > 0.5
    if (lambda() > 0.5) {
        rehash();
    }

    // runs if anything left in old table to transfer
    if (m_oldTable) {
        remakeTable();
    }

    return true;
}

void CarDB::rehash() {

    // sets old table to new table
    m_oldTable = m_currentTable;
    m_oldCap = m_currentCap;
    m_oldProbing = m_currProbing;
    m_oldSize = m_currentSize;
    m_oldNumDeleted = m_currNumDeleted;

    // sets new table variables
    m_currentCap = findNextPrime(4 * (m_oldSize - m_oldNumDeleted));
    m_currentSize = 0;
    m_currNumDeleted = 0;
    if (m_newPolicy != DEFPOLCY) {
        m_currProbing = m_newPolicy;
    }

    // initializes new table to empty cars
    m_currentTable = new Car[m_currentCap];
    for (int i = 0; i < m_currentCap; i++) {
        Car emptyCar;
        m_currentTable[i] = emptyCar;
    }
}

void CarDB::remakeTable() {
    int counter = 0;

    // fills in 1/4 of new table
    for (int i = 0; i < m_oldCap && counter < (int)(m_oldSize / 4); i++) {
        if (m_oldTable[i].m_used == true) {
            newInsert(m_oldTable[i]);
            m_oldTable[i].m_used = false;
            m_oldNumDeleted++;
            counter++;
        }
    }
    // cout << (m_oldTable == nullptr) << endl;

    // if 100% of old table has been transferred it deletes the old table
    if (m_oldNumDeleted == m_oldSize) {
        delete [] m_oldTable;
        m_oldTable = nullptr;
    }
}

bool CarDB::remove(Car car){

    // if car not in either hash table return false
    if (!(getCar(car.m_model, car.m_dealer) == car)) {
        if (m_oldTable) {
            remakeTable();
        }
        return false;
    }

    int index = m_hash(car.m_model) % m_currentCap;


    // checks current table
    if (m_currentTable[index] == car && m_currentTable[index].m_used == true) {
        m_currentTable[index].m_used = false;
        m_currNumDeleted++;
    }
    else {
        int i = 0;

        while (m_currentTable[index].m_model != "") {
            if (m_currProbing == DOUBLEHASH) {
                index = ((m_hash(car.m_model) % m_currentCap) + i * (11-(m_hash(car.m_model) % 11))) % m_currentCap;
                i++;
                if (m_currentTable[index] == car) {
                    m_currentTable[index].m_used = false;
                    m_currNumDeleted++;
                }
            }
            else if (m_currProbing == QUADRATIC) {
                index = ((m_hash(car.m_model) % m_currentCap) + (i * i)) % m_currentCap;
                i++;
                if (m_currentTable[index] == car) {
                    m_currentTable[index].m_used = false;
                    m_currNumDeleted++;
                }
            }
        }
    }


    if (m_oldTable != nullptr) {
        index = m_hash(car.m_model) % m_oldCap;

        if (m_oldTable[index] == car) {
            m_oldTable[index].m_used = false;
            m_oldNumDeleted++;
        }
        else {
            int i = 0;

            while (m_oldTable[index].m_model != "") {
                if (m_oldProbing == DOUBLEHASH) {
                    index = ((m_hash(car.m_model) % m_oldCap) + i * (11-(m_hash(car.m_model) % 11))) % m_oldCap;
                    i++;
                    if (m_oldTable[index] == car) {
                        m_oldTable[index].m_used = false;
                        m_oldNumDeleted++;
                    }
                }
                else if (m_oldProbing == QUADRATIC) {
                    index = ((m_hash(car.m_model) % m_oldCap) + (i * i)) % m_oldCap;
                    i++;
                    if (m_oldTable[index] == car) {
                        m_oldTable[index].m_used = false;
                        m_oldNumDeleted++;
                    }
                }
            }
        }
    }

    // rehashes if needed
    if (deletedRatio() > 0.8) {
        rehash();
    }

    // continues to remake the table if needed
    if (m_oldTable) {
        remakeTable();
    }

    return true;
}

Car CarDB::getCar(string model, int dealer) const{
    int index = m_hash(model) % m_currentCap;


    // checks current table
    if (m_currentTable[index].m_dealer == dealer && m_currentTable[index].m_model == model) {
        if (m_currentTable[index].m_used == true) {
            return m_currentTable[index];
        }
    }
    else {
        int i = 0;

        while (m_currentTable[index].m_model != "") {
            if (m_currProbing == DOUBLEHASH) {
                index = ((m_hash(model) % m_currentCap) + i * (11-(m_hash(model) % 11))) % m_currentCap;
                i++;
                if (m_currentTable[index].m_dealer == dealer && m_currentTable[index].m_model == model) {
                    if (m_currentTable[index].m_used == true) return m_currentTable[index];
                }
            }
            else if (m_currProbing == QUADRATIC) {
                index = ((m_hash(model) % m_currentCap) + (i * i)) % m_currentCap;
                i++;
                if (m_currentTable[index].m_dealer == dealer && m_currentTable[index].m_model == model) {
                    if (m_currentTable[index].m_used == true) {
                        return m_currentTable[index];
                    }
                }
            }
        }
    }

    // checks old table if needed
    if (m_oldTable != nullptr) {
        index = m_hash(model) % m_oldCap;
        
        if (m_oldTable[index].m_dealer == dealer && m_oldTable[index].m_model == model) {
            if (m_oldTable[index].m_used == true) return m_oldTable[index];
        }
        else {
            int i = 0;

            while (m_oldTable[index].m_model != "") {
                if (m_oldProbing == DOUBLEHASH) {
                    index = ((m_hash(model) % m_oldCap) + i * (11-(m_hash(model) % 11))) % m_oldCap;
                    i++;
                    if (m_oldTable[index].m_dealer == dealer && m_oldTable[index].m_model == model) {
                        if (m_oldTable[index].m_used == true) return m_oldTable[index];
                    }
                }
                else if (m_oldProbing == QUADRATIC) {
                    index = ((m_hash(model) % m_oldCap) + (i * i)) % m_oldCap;
                    i++;
                    if (m_oldTable[index].m_dealer == dealer && m_oldTable[index].m_model == model) {
                        if (m_oldTable[index].m_used == true) return m_oldTable[index];
                    }
                }
            }
        }
    }

    // return empty car if nothing found
    Car empty;
    return empty;
}

float CarDB::lambda() const {
    return (float)m_currentSize / (float)m_currentCap;
}

float CarDB::deletedRatio() const {
    return (float)m_currNumDeleted / (float)m_currentSize;
}

void CarDB::dump() const {
    cout << "Dump for the current table: " << endl;
    if (m_currentTable != nullptr)
        for (int i = 0; i < m_currentCap; i++) {
            cout << "[" << i << "] : " << m_currentTable[i] << endl;
        }
    cout << "Dump for the old table: " << endl;
    if (m_oldTable != nullptr)
        for (int i = 0; i < m_oldCap; i++) {
            cout << "[" << i << "] : " << m_oldTable[i] << endl;
        }
}

bool CarDB::updateQuantity(Car car, int quantity){
    if (getCar(car.m_model, car.m_dealer) == car) {
        car.m_quantity = quantity;
        return true;
    }
    else {
        return false;
    }
}

bool CarDB::isPrime(int number){
    bool result = true;
    for (int i = 2; i <= number / 2; ++i) {
        if (number % i == 0) {
            result = false;
            break;
        }
    }
    return result;
}

int CarDB::findNextPrime(int current){
    //we always stay within the range [MINPRIME-MAXPRIME]
    //the smallest prime starts at MINPRIME
    if (current < MINPRIME) current = MINPRIME-1;
    for (int i=current; i<MAXPRIME; i++) { 
        for (int j=2; j*j<=i; j++) {
            if (i % j == 0) 
                break;
            else if (j+1 > sqrt(i) && i != current) {
                return i;
            }
        }
    }
    //if a user tries to go over MAXPRIME
    return MAXPRIME;
}

ostream& operator<<(ostream& sout, const Car &car ) {
    if (!car.m_model.empty())
        sout << car.m_model << " (" << car.m_dealer << "," << car.m_quantity<< ")";
    else
        sout << "";
  return sout;
}

bool operator==(const Car& lhs, const Car& rhs){
    // since the uniqueness of an object is defined by model and delaer
    // the equality operator considers only those two criteria
    return ((lhs.m_model == rhs.m_model) && (lhs.m_dealer == rhs.m_dealer));
}