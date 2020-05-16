#include <iostream>
#include <string>
#include <time.h>
#include "graphImplementation.cpp"

using namespace std;


int main(){

    /* The number of days simulation will run for */
    int days_to_simulate = 100;

    /* Ask the user for the population and
    connections files */
    cout << "Welcome to CoronaSim!" << endl;
    string pop_file, conn_file;
    cout << "Enter population file name: ";
    cin >> pop_file;
    cout << "Enter connections file name: ";
    cin >> conn_file;

    // TODO: Populate your graph data structure
    bool is_populated = false;
    Graph corona_graph = Graph();
    ifstream population(pop_file);
    ifstream connections(conn_file);
    if (!population)
    {
        std::cerr << "Uh oh, "<< pop_file << " could not be opened for reading! Please ensure that the name and extension are properly typed." << std::endl;
        return 1;
    }
    if (!connections)
    {
        std::cerr << "Uh oh, "<< conn_file << " could not be opened for reading! Please ensure that the name and extension are properly typed." << std::endl;
        return 1;
    }

    // Adding the nodes
    string name, age, spread_prob;
    while (population >> name >> age >> spread_prob)
    {
        int ageFinal = stoi(age);
        float dp = stof(spread_prob);
        corona_graph.add_node(Person(name,ageFinal,dp));
    }

    // Adding the edges
    string first, second;
    while (connections >> first >> second)
    {
        corona_graph.add_edge(first,second);
    }
    corona_graph.set_disease_prob();

    is_populated = true;
    if (is_populated)
        cout << "File Loaded!" << endl;

    while(true){

        int choice;

        /* Print menu of things that your application can do*/
        cout << "*** *** *** *** *** *** *** *** *" << endl;
        cout << "What would you like to do?" << endl;
        cout << "1. Print population" << endl;
        cout << "2. Simulate" << endl;
        cout << "3. Summarize population" << endl;
        cout << "4. Implement policy and simulate" << endl;
        cout << "5. Exit" << endl;
        cout << "*** *** *** *** *** *** *** *** *" << endl;
        cout << "Your selection: ";

        /* TODO: You should feel free to add other
        items to the menu above for your demo video */

        cin >> choice;

        /*Ideally inside the various cases of the switch
        statement below, you will call specific functions
        supported by your graph data structure and your
        simulation algorithm. You can design all these
        functions as you like.*/

        switch(choice){

            case 1:
                cout << "The population is as follows: \n" << endl;
                corona_graph.printPeople();
                break;

            case 2:
                corona_graph.reset();
                for(int i = 0; i < 100; i++){
                    corona_graph.simulate_spread();
                }
                cout << "Simulation over :)" << endl;
                break;

            case 3:
                corona_graph.printStatistics();
                break;

            case 4:
                corona_graph.control_policy();
                for(int i = 0; i < 100; i++){
                    corona_graph.simulate_spread();
                }
                break;

            case 5:
                return 0; // exiting
            default:
                cout << " Please your choice must respect 1 <= choice <= 5. " << endl;
        }
    }
}