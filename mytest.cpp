#include "dealer.h"
#include <random>
#include <vector>
#include <algorithm>
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE};
class Random {
public:
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor 
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
        else { //the case of SHUFFLE to generate every number only once
            m_generator = std::mt19937(m_device());
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }

    void getShuffle(vector<int> & array){
        // the user program creates the vector param and passes here
        // here we populate the vector using m_min and m_max
        for (int i = m_min; i<=m_max; i++){
            array.push_back(i);
        }
        shuffle(array.begin(),array.end(),m_generator);
    }

    void getShuffle(int array[]){
        // the param array must be of the size (m_max-m_min+1)
        // the user program creates the array and pass it here
        vector<int> temp;
        for (int i = m_min; i<=m_max; i++){
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it=temp.begin(); it != temp.end(); it++){
            array[i] = *it;
            i++;
        }
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result*100.0)/100.0;
        return result;
    }
    
    private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};


unsigned int hashCode(const string str);

string carModels[5] = {"challenger", "stratos", "gt500", "miura", "x101"};
string dealers[5] = {"super car", "mega car", "car world", "car joint", "shack of cars"};


class Tester {

public:

    // tests the constructor
    bool constructorNormal() {
        bool result = true;

        CarDB table(MINPRIME, hashCode, QUADRATIC);

        result = result && (table.m_currentSize = MINPRIME);
        result = result && (table.m_hash = hashCode);
        result = result && (table.m_currProbing = QUADRATIC);

        return result;
    }

    // tests insert function
    bool insertTest() {
            
        vector<Car> dataList;
        Random RndID(MINID,MAXID);
        Random RndCar(0,4);// selects one from the carModels array
        Random RndQuantity(0,50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);
        bool result = true;
        
        for (int i=0;i<49;i++){
            // generating random data
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), 
                            RndID.getRandNum(), true);
            // saving data for later use
            dataList.push_back(dataObj);
            // inserting data in to the CarDB object
            cardb.insert(dataObj);  
        }

        result = result && (cardb.m_currentSize == 49);

        return result;
    }

    // getCar function
    bool getCarNormal() {

        vector<Car> dataList;
        Random RndID(MINID,MAXID);
        Random RndCar(0,4);// selects one from the carModels array
        Random RndQuantity(0,50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);
        bool result = true;

        for (int i=0;i<43;i++){
            // generating random data
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), 
                            RndID.getRandNum(), true);
            // saving data for later use
            dataList.push_back(dataObj);
            // inserting data in to the CarDB object
            cardb.insert(dataObj);  
        }

        result = result && (cardb.getCar(dataList.at(4).m_model, dataList.at(4).m_dealer).m_model != "");
        result = result && (cardb.getCar(dataList.at(13).m_model, dataList.at(13).m_dealer).m_model != "");
        result = result && (cardb.getCar(dataList.at(2).m_model, dataList.at(2).m_dealer).m_model != "");

        return result;
    }

    // getCar case with object not in table
    bool getCarError() {

        vector<Car> dataList;
        Random RndID(MINID,MAXID);
        Random RndCar(0,4);// selects one from the carModels array
        Random RndQuantity(0,50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        for (int i=0;i<3;i++){
            // generating random data
            Car dataObj = Car(carModels[2], RndQuantity.getRandNum(), 
                            RndID.getRandNum(), true);
            // saving data for later use
            dataList.push_back(dataObj);
            // inserting data in to the CarDB object
            cardb.insert(dataObj);  
        }

        return (cardb.getCar(carModels[1], 23).m_model == "");
    }

    // remove with non-colliding keys
    bool removeNormal() {

        vector<Car> dataList;
        Random RndID(MINID,MAXID);
        Random RndCar(0,4);// selects one from the carModels array
        Random RndQuantity(0,50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);
        bool result = true;

        for (int i=0;i<43;i++){
            // generating random data
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), 
                            RndID.getRandNum(), true);
            // saving data for later use
            dataList.push_back(dataObj);
            // inserting data in to the CarDB object
            cardb.insert(dataObj);  
        }

        // removes cars then makes sure they're not still in the table
        cardb.remove(dataList.at(3));
        result = result && (cardb.getCar(dataList.at(3).m_model, dataList.at(3).m_dealer).m_model == "");

        cardb.remove(dataList.at(7));
        result = result && (cardb.getCar(dataList.at(7).m_model, dataList.at(7).m_dealer).m_model == "");

        cardb.remove(dataList.at(12));
        result = result && (cardb.getCar(dataList.at(12).m_model, dataList.at(12).m_dealer).m_model == "");

        return result;
    }

    // remove case with colliding keys
    bool removeCollide() {

        vector<Car> dataList;
        Random RndID(MINID,MAXID);
        Random RndCar(0,4);// selects one from the carModels array
        Random RndQuantity(0,50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        int prevCap = cardb.m_currentCap;

        for (int i=0;i<43;i++){
            // generating random data
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), 
                            RndID.getRandNum(), true);
            // saving data for later use
            dataList.push_back(dataObj);
            // inserting data in to the CarDB object
            cardb.insert(dataObj);  
        }

        // removes the same element 40 times, which would trigger
        // a rehash if it all went through
        for (int i = 0; i < 40; i++) {
            cardb.remove(dataList.at(14));
        }

        // shows that a rehash was not triggered
        return (cardb.m_currentCap == prevCap);
    }

    // rehash after certain amount of insertions
    bool rehashInsert() {

        vector<Car> dataList;
        Random RndID(MINID,MAXID);
        Random RndCar(0,4);// selects one from the carModels array
        Random RndQuantity(0,50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        // verifies that old table does not exist before insertions
        bool result = (!cardb.m_oldTable);

        for (int i=0;i<51;i++){
            // generating random data
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), 
                            RndID.getRandNum(), true);
            // saving data for later use
            dataList.push_back(dataObj);
            // inserting data in to the CarDB object
            cardb.insert(dataObj);  
        }

        // verifies that old table (and rehash) exist after 51 insertions
        result = result && (cardb.m_oldTable);

        return result;
    }

    bool completeRehashInsert() {

        vector<Car> dataList;
        Random RndID(MINID,MAXID);
        Random RndCar(0,4);// selects one from the carModels array
        Random RndQuantity(0,50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        // verifies that old table does not exist before insertions
        bool result = (!cardb.m_oldTable);

        for (int i=0;i<55;i++){
            // generating random data
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), 
                            RndID.getRandNum(), true);
            // saving data for later use
            dataList.push_back(dataObj);
            // inserting data in to the CarDB object
            cardb.insert(dataObj);

            // shows that rehash happened and old table exists
            // after the 51st insertion
            if (i == 50) {
                result = result && (cardb.m_oldTable != nullptr);
            }
        }

        // verifies that old table no longer exists after 55 insertions
        // and m_currentSize is the same which means that all data was transferred
        result = result && (cardb.m_oldTable == nullptr); 
        result = result && (cardb.m_currentSize == 55);

        return result;
    }

    // rehash after certain amount of deletions
    bool rehashDelete() {

        vector<Car> dataList;
        Random RndID(MINID,MAXID);
        Random RndCar(0,4);// selects one from the carModels array
        Random RndQuantity(0,50);
        CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);

        bool result = true;

        for (int i=0;i<49;i++){
            // generating random data
            Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), 
                            RndID.getRandNum(), true);
            // saving data for later use
            dataList.push_back(dataObj);
            // inserting data in to the CarDB object
            cardb.insert(dataObj);  
        }

        // after this deletion factor is .7 and hasn't rehashed yet
        // result shows that deleted buckets still in table
        for (int i = 0; i < 39; i++) {
            cardb.remove(dataList.at(i));
        }
        result = result && (cardb.m_currNumDeleted == 39);

        // after this deletion the deletion factor is .8 and it rehashes
        // result shows that deleted buckets no longer in the table and was rehashed
        cardb.remove(dataList.at(39));
        result = result && (cardb.m_currNumDeleted == 0);

        return result;
    }

};



int main() {
    cout << endl;
    
    Tester test;

    // constructor
    if (test.constructorNormal()) {
        cout << "Constructor test passed" << endl;
    }
    else {
        cout << "Constructor test failed" << endl;
    }
    cout << endl;

    // insert 
    if (test.insertTest()) {
        cout << "Insert case passed" << endl;
    }
    else {
        cout << "Insert case failed" << endl;
    }
    cout << endl;

    // getCar cases
    if (test.getCarNormal()) {
        cout << "Get car normal case passed" << endl;
    }
    else {
        cout << "Get car normal case failed" << endl;
    }

    if (test.getCarError()) {
        cout << "Get car error case passed" << endl;
    }
    else {
        cout << "Get car error case failed" << endl;
    }
    cout << endl;

    // remove cases
    if (test.removeNormal()) {
        cout << "Remove normal case passed" << endl;
    }
    else {
        cout << "Remove normal case failed" << endl;
    }

    if (test.removeCollide()) {
        cout << "Remove collide case passed" << endl;
    }
    else {
        cout << "Remove collide case failed" << endl;
    }
    cout << endl;

    // rehash cases
    if (test.rehashInsert()) {
        cout << "Rehash trigger test for insertions passed" << endl;
    }
    else {
        cout << "Rehash trigger test for insertions failed" << endl;
    }

    if (test.completeRehashInsert()) {
        cout << "Rehash insert completion test passed" << endl;
    }
    else {
        cout << "Rehash insert completion test failed" << endl;
    }

    if (test.rehashDelete()) {
        cout << "Rehash trigger test for deletions passed" << endl;
    }
    else {
        cout << "Rehash trigger test for deletions failed" << endl;
    }
    cout << endl;

    return 0;
}

unsigned int hashCode(const string str) {
   unsigned int val = 0 ;
   const unsigned int thirtyThree = 33 ;  // magic number from textbook
   for (unsigned int i = 0 ; i < str.length(); i++)
      val = val * thirtyThree + str[i];
   return val;
}