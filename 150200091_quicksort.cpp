#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;

/*
//// Hakan Duran 150200091
*/

// Node class which is used for linked-list.

class node
{

private:
    string pcity;
    int ppopulation;
    node* pback;
    node* pforward;

public:
    node(string city, int population, node* back) : pcity{city}, ppopulation{population}, pback {back}
    {};

    ~node(){};

    void set_population(int population){
        ppopulation = population;
    }

    int get_population(){
        return this->ppopulation;
    }

    void set_city(string city){
        pcity = city;
    }

    string get_city(){
        return this->pcity;
    }

    void set_back(node* back){
        pback = back;
    }

    node* get_back(){
        return this->pback;
    }

    void set_forward(node* forward){
        pforward = forward;
    }

    node* get_forward(){
        return this->pforward;
    }
};

/* Function declarations */

/* Functions used to transfer csv file into the linked-list */
node* csv_to_node(string csv);
void node_to_csv(node* onode, string outfile);

/* Linked list traversal operation */
node* forward_for(node* wnode, int i);

/* Immediate operations on linked-list */

// Swap two node's values
void swap (node* init, int i, int j, node** sc);

// Finds size of linked list
int size_of(node* init);

// Finds searched node
node* find(node* init, int i, node** sc);

/* Sort operations */
int partition(node* init, int low, int high, string v, node** sc);
int partition_random(node* init, int low, int high, string v, node** sc);
int partition_random3(node* init, int low, int high, string v, node** sc);
void quicksort(node* init, int low, int high, int threshold, string v, node** sc);
void quicksort_random(node* init, int low, int high, int threshold, string v, node** sc);
void quicksort_random3(node* init, int low, int high, int threshold, string v, node** sc);
void insertion(node* init, int size, node** sc, int order);

/* Intermediate functions */

// Creates an array which holds some nodes in an array in order to make traversal faster.
node** shortcut_list_generate(node* init, node** sc, int size);

// Creates log.txt file when it is used.
void verbose(node* init, int pivot);




int main(int argc, char* argv[]){

    /* Argumant control */

    if ((argc < 5) || (argc > 6)) {
        cerr << "Usage: " << argv[0] << " <input_csv_file> <mode> <threshold> <output_csv_file> <v>" << endl;
        return 1;
    }
    string input = argv[1];
    string mode = argv[2];
    int threshold = stoi(argv[3]);
    string output = argv[4];
    string v;
    if(argc == 5){
        v="f";
    }
    else{
        v = argv[5];
    }

    /* Creates log.txt file when it is defined. */
    
    if (v == "v"){
        string outfile = "log.txt";
        ofstream file;
        file.open(outfile, ios::out);
        if (file) {
            file << "";
            file.close();
        }
        else {
            cerr << "Unable to open file: " << outfile << endl;
        }
    }

    /* Input CSV file is transferring into linked-list*/

    node* initial = csv_to_node(input);
    int size = size_of(initial);

    /* An array is creating here in order for making traversal faster */

    node** sc = new node*[1000];
    shortcut_list_generate(initial, sc, size);

    /* Determining which sort algorithm going to be used */

    auto start = std::chrono::high_resolution_clock::now();

    if (size > threshold){
        if(mode=="l"){
            quicksort(initial, 0, size-1, threshold, v, sc);
        }
        else if(mode=="r"){
            quicksort_random(initial, 0, size-1, threshold, v, sc);
        }
        else if(mode=="m"){
            quicksort_random3(initial, 0, size-1, threshold, v, sc);
        }
        else{
            cerr << "Unvalid mode.";
            return 1;
        }
    }
    else {
        insertion(initial, size, sc, 0);
    }

    /* Duration time is determined and showed here */

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time taken by QuickSort with pivot strategy ' "<< mode << " ' and threshold " << threshold << ": " 
    << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count() << " ns." << std::endl;

    /* Sorted linked-list are transferring into a CSV file here */

    node_to_csv(initial, output);

    /* Delete operations for deallocating memory */

    node* tnode;
    while(initial->get_forward() != nullptr){
        tnode = initial->get_forward();
        delete initial;
        initial = tnode;
    }
    delete initial;
    delete[] sc;
    
    return 0;
}

/* FUNCTIONS */

node* csv_to_node(string csv){
    node* newnode;
    node* oldnode = nullptr;

    ifstream file(csv);

    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string value;

            int i = 0;

            while (getline(ss, value, ';')) {
                if(i%2 == 0){
                    newnode = new node(value, 0, oldnode);
                    if(oldnode){
                        oldnode->set_forward(newnode);
                    }
                }
                else {
                    newnode->set_population(stoi(value));
                    oldnode = newnode;
                }
                i++;
            }
        }

        oldnode->set_forward(nullptr);
        file.close();
    }

    while(newnode->get_back() != nullptr){
        newnode = newnode->get_back();
    }

    return newnode;

}

void node_to_csv(node* onode, string outfile){

    ofstream file;
    file.open(outfile, ios::out);
    if (file) {
        while(onode->get_forward() != nullptr) {
            file << onode->get_city() << ";" << onode->get_population() << endl;
            onode = onode->get_forward();
        }
        file << onode->get_city() << ";" << onode->get_population() << endl;
        file.close();
    } 
    
    else {
        cerr << "Unable to open file: " << outfile << endl;
    }
}

node* forward_for(node* wnode, int i){
    for (int j=0; j<i; j++){
        if(wnode->get_forward()){
            wnode = wnode->get_forward();
        }
    }
    return wnode;
}

void swap (node* init, int i, int j, node** sc){
    node* node1 = find(init, i, sc);
    node* node2 = find(init, j, sc);
    
    string tcity = node1->get_city();
    int tpopulation = node1->get_population();

    node1->set_city(node2->get_city());
    node1->set_population(node2->get_population());

    node2->set_city(tcity);
    node2->set_population(tpopulation);
}

/* Size of the linked-list */

int size_of(node* init){
    int i=1;
    while(init->get_forward() != nullptr){
        init = init->get_forward();
        i++;
    }

    return i;
}

/* 
Finds the wanted node by using sc.
sc means for Shortcut. sc has been created just after the linked-list creation.
Pointers in sc are in the ranks of 100 or coefficients of 100 in 
the linked-list. The purpose of this array is to provide you closest pointer on linked-list.
Details are in the report. 
*/

node* find(node* init, int i, node** sc){
    int mod = i%100;
    i = (i - mod) / 100;
    node* mod100pointer = sc[i];
    return forward_for(mod100pointer, mod);
}

/*

// Sorting algorithms

*/

/* Naive Quicksort */

int partition(node* init, int low, int high, string v, node** sc){

  int pivot = find(init, high, sc)->get_population();
  int i = low - 1;
   
  for(int j = low; j <= high; j++)
  {
    if(find(init, j, sc)->get_population() < pivot)
    {
      i++;
      swap(init, i, j, sc);
    }
  }
  swap(init, i+1, high, sc);
  if(v == "v"){ verbose(init, pivot); }
  return i+1;
}

void quicksort(node* init, int low, int high, int threshold, string v, node** sc){
    if(low<high){
        int pivot = partition(init, low, high, v, sc);

        if (threshold == 1){
            quicksort(init, low, pivot-1, threshold, v, sc);
            quicksort(init, pivot+1, high, threshold, v, sc);
        }
        else {
            if(pivot-1-low > threshold){
                quicksort(init, low, pivot-1, threshold, v, sc);
            }
            else{ 
                insertion(find(init, low , sc) , pivot-low , sc, low);
            }
            if(high-pivot-1 > threshold){ 
                quicksort(init, pivot+1, high, threshold, v, sc);
            }
            else{ 
                insertion(find(init, pivot+1 ,sc) , high-pivot , sc, pivot+1);
            }
        }
    }
}

/* Randomized Quicksort */

int partition_random(node* init, int low, int high, string v, node** sc)
{
    srand(time(NULL));
    int random = low + rand() % (high - low);
    swap(init, random, high, sc);
    return partition(init, low, high, v, sc);
}

void quicksort_random(node* init, int low, int high, int threshold, string v, node** sc){
    if(low<high){
        int pivot = partition_random(init, low, high, v, sc);

        if (threshold == 1){
            quicksort_random(init, low, pivot-1, threshold, v, sc);
            quicksort_random(init, pivot+1, high, threshold, v, sc);
        }
        else {
            if(pivot-1-low > threshold){
                quicksort_random(init, low, pivot-1, threshold, v, sc);
            }
            else{ 
                insertion(find(init, low , sc) , pivot-low , sc, low );
            }
            if(high-pivot-1 > threshold){ 
                quicksort_random(init, pivot+1, high, threshold, v, sc);
            }
            else{ 
                insertion(find(init, pivot+1 ,sc) , high-pivot , sc, pivot+1);
            }
        }

    }
}

/* 3-median Randomized Quicksort*/

int partition_random3(node* init, int low, int high, string v, node** sc)
{
    srand(time(NULL));
    int random = high;

    int r1 = low + rand() % (high - low);
    int r2 = low + rand() % (high - low);
    int r3 = low + rand() % (high - low);

    if ((r1 <= r2 && r2 <= r3) || (r3 <= r2 && r2 <= r1)){
        random = r2;
    }
    else if ((r2 <= r1 && r1 <= r3) || (r3 <= r1 && r1 <= r2)){
        random = r1;
    }
    else{
        random = r3;
    }

    swap(init, random, high, sc);
    return partition(init, low, high, v, sc);
}

void quicksort_random3(node* init, int low, int high, int threshold, string v, node** sc){
    if(low<high){
        int pivot = partition_random3(init, low, high, v, sc);

        if (threshold == 1){
            quicksort_random3(init, low, pivot-1, threshold, v, sc);
            quicksort_random3(init, pivot+1, high, threshold, v, sc);
        }
        else {
            if(pivot-1-low > threshold){
                quicksort_random3(init, low, pivot-1, threshold, v, sc);
            }
            else{ 
                insertion(find(init, low , sc) , pivot-low , sc , low);
            }
            if(high-pivot-1 > threshold){ 
                quicksort_random3(init, pivot+1, high, threshold, v, sc);
            }
            else{ 
                insertion(find(init, pivot+1 ,sc) , high-pivot , sc, pivot+1); 
            }
        }

    }
}

/* Insertion sort */

void insertion(node* init, int size, node** sc, int order){

    int i, pkey, j;
    string ckey;

    for (i = 1; i < size; i++) {

        pkey = find(init, i + order, sc)->get_population();
        ckey = find(init, i + order, sc)->get_city();
        j = i - 1;

        while (j >= 0 && find(init, j + order, sc)->get_population() > pkey) {
            find(init, j+1 + order, sc)->set_population( find(init, j + order, sc)->get_population() );
            find(init, j+1 + order, sc)->set_city( find(init, j + order, sc)->get_city() );
            j = j - 1;
        }

        find(init, j+1 + order, sc)->set_population( pkey );
        find(init, j+1 + order, sc)->set_city( ckey );

    }
}

/* Sc (shortcut-list) generator */

node** shortcut_list_generate(node* init, node** sc, int size){

    int order = 0;
    node* mod100node = forward_for(init, 0);
    for(int i = 0; i < size; i = i + 100){
        int order = i/100;
        sc[order] = mod100node;
        mod100node = forward_for(mod100node, 100);
    }

    return sc;
}

/* Verbose, creates log.txt */

void verbose(node* init, int pivot){
    string outfile = "log.txt";
    ofstream file;
    file.open(outfile, fstream::app);
    if (file) {

        file << "Pivot: " << pivot << " Array: [";
        while(init->get_forward() != nullptr){
            file << init->get_population() << " , ";
            init = init->get_forward();
        }
        file << init->get_population() << "]" << endl;

        file.close();
    } 
    
    else {
        cerr << "Unable to open file: " << outfile << endl;
    }
}