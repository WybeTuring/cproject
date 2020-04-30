
/* Authour: Lemfon Karl Ndze'dzenyuy
 * Purpose: An implementation of a graph data structure that will be used throughout the next stages of the in-class C project
 * Design: The graph is implemented using an adjacency list. The list is used to avoid inefficient use of memory that can arise 
           from using an adjacency matrix in a sparse graph. The graph is stored in a vector structure. 
 * 	       A class called Person, that holds information about the persons is created. For each person object, the attribute that
           is most notable is a vector that holds pointers to other people objects that the person can infect. By simply keeping 
           records to the People objects that a given Person is capable of infecting, we minimise the usage of memory and also reduce
           the number of searches that will be done during the simulation face. 
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

// This function takes a probability and returns true or false.
bool simulate_prob(float prob){
    prob = prob*1000.0;
    srand(time(NULL));
    if (rand()%1000 <= prob)
        return true;
    return false;
}

/* The Person class tries to represent the four states in which a person finds himself/herself in the following way;
    1. If the person is uninefected, then the boolean member infection_status is false;
    2. If the person is infected but not sick, then the boolean member infection_status is true, and the number of days infected is 0.
    3. If the person is infected and sick, then the boolean member infection_status is true, and the number of days infected is 1 or more. 
*/
class Person{
	public:
		string name;
		int age;
		float spread_prob;
		float disease_prob;
		float recovery_prob;
		int number_of_days;
		bool infection_status; //Infection status is true when the person is infected and false otherwise.
        vector<Person*> victims;
	
		Person(string name, int age, float spread_prob): name(name), age(age), spread_prob(spread_prob), number_of_days(0), infection_status(false){}

		bool equals(Person person){
			return (this->name == person.name && this->age == person.age && this->spread_prob == person.spread_prob);
		}

        void print_people_list(){
            cout << name;
            if(victims.size() !=0){
                cout << "  |  ";
                for(int i = 0; i < victims.size() - 1; i++){
                    cout << victims[i]->name << ",";
                }
                cout << victims[victims.size() - 1]->name << ".";
            }
            cout << endl;
        }
};

class Graph{
	public:
		int largest_age = 1;
        int num_cases_recorded = 0; // This simply takes count of the number of infections recorded. 
        int num_sick = 0; // Takes note of the number of infected people who actually got sick;
        int recoveries_recorded = 0;
		vector<Person> adjlist;

		// This function adds a new node into the graph. 
		void add_node(Person person){
			adjlist.push_back(person);
			if(person.age > largest_age){largest_age = person.age;}
		}

		// This function inserts an edge to the graph. The key difference here is that it uses only the names as arguments.
		void add_edge(string person1_name, string person2_name){
			// The grand idea is to traverse the vector in the graph till we find the node that holds the persons 1 and 2, and then we add a node of person two to the linked list
			int index1;
            int index2;
			for(int i = 0; i < adjlist.size(); i++){
				if(adjlist[i].name == person1_name){
					index1 = i;
                    break;
				}
			}

            for(int i = 0; i < adjlist.size(); i++){
				if(adjlist[i].name == person2_name){
					index2 = i;
                    break;
				}
			}
			adjlist[index1].victims.push_back(&adjlist[index2]);
		}
	
        // This function prints the persons in the graph, with the people they can infect.
		void printPeople(){
			for(int i = 0; i < adjlist.size(); i++){
				adjlist[i].print_people_list();
			}
		}

        // This function prints the people in the graph, and the persons they can infect.
		void printLengths(){
			for(int i = 0; i < adjlist.size(); i++){
				cout << adjlist[i].name << " can infect " << adjlist[i].victims.size() << " people" << endl;
			}
		}

        // This function sets the disease probability and recovery probability for each member of the population.
        void set_disease_prob(){
            for(int i = 0; i < adjlist.size(); i++){
                adjlist[i].disease_prob = adjlist[i].age / (float) largest_age;
                adjlist[i].recovery_prob = 1 - adjlist[i].disease_prob;
            }
        } 

        // This function infects the first person after choosing them randomly
        bool infect_patient_zero(){
            srand(time(NULL));
            int i = rand() % adjlist.size();

            if(adjlist[i].victims.size() > 0){
               adjlist[i].infection_status = true;
               num_cases_recorded++; 
               return true;
            }
            return false;
            
        }

        // This function goes through and lets infected people infect other people
        void infection_simulation(){
            for(int i = 0; i < adjlist.size(); i++){
                if(adjlist[i].infection_status && (adjlist[i].number_of_days <= 7)){  
                    // Only carry this loop for infected people and not dead people. As the rest cannot infect others. 
                    Person* temp = &adjlist[i];
                    for(int j = 0; j < temp->victims.size(); j++){
                        // Important to remember that the victims array is an array of Person pointers.
                        if(!(temp->victims[j]->infection_status)){
                            if(simulate_prob(temp->spread_prob)){
                                temp->victims[j]->infection_status = true;
                                num_cases_recorded++;
                            }
                        }
                    }
                }
            }
        }

        // This function goes through and simulates the phase in which infected people get sick
        // For the persons who are already sick and not dead, the number of days sick increases.
        void get_sick_simulation(){
            for(int i = 0; i < adjlist.size(); i++){
                // Check if the person is infected but not yet sick, and then determine if they finally get sick based on the disease probability
                if(adjlist[i].infection_status && (adjlist[i].number_of_days == 0)){
                    if(simulate_prob(adjlist[i].disease_prob)){
                        adjlist[i].number_of_days = 1;
                        num_sick++;
                    }
                }
                // Check if the person is already sick and increase the number of days sick
                else if((adjlist[i].number_of_days > 0) && (adjlist[i].number_of_days <= 7)){
                    adjlist[i].number_of_days++;
                }
            }
        }

        // This function determines if a person that is sick will get well. 
        void recovery_simulation(){
            for(int i = 0; i < adjlist.size(); i++){
                // Checking if the person is sick and ali
                if((adjlist[i].infection_status) && (adjlist[i].number_of_days <= 7)){
                    if(simulate_prob(adjlist[i].recovery_prob)){
                        recoveries_recorded++;
                        adjlist[i].infection_status = false;
                        adjlist[i].number_of_days = 0;
                    }
                }
            }
        }

        // This function carries out the simulation. Please before carrying out the simulation, always ensure that a random person has been infected.
        void simulate_spread(){
            infection_simulation();
            infection_simulation();
            get_sick_simulation();
            get_sick_simulation();
            recovery_simulation();
            recovery_simulation();
        }

        // This function gets the current number of infected people
        int get_num_infected(){
            int num = 0;
            for(int i = 0; i < adjlist.size(); i++){
                if(adjlist[i].infection_status && (adjlist[i].number_of_days == 0)){
                    num++;
                }
            }
            return num;
        }

        // This function gets the current number of people that are infected and sick
        int get_num_sick(){
            int num = 0;
            for(int i = 0; i < adjlist.size(); i++){
                if((adjlist[i].number_of_days > 0) && (adjlist[i].number_of_days <= 7)){
                    num++;
                }
            }
            return num;
        }

        // This function gets the number of dead people
        int get_num_dead(){
            int num = 0;
            for(int i = 0; i < adjlist.size(); i++){
                if(adjlist[i].number_of_days > 7){
                    num++;
                }
            }
            return num;
        }

        // This function prints the summary statistics of the population.
        void printStatistics(){
            int current_infections = get_num_infected();
            int current_sick = get_num_sick();
            int num_dead = get_num_dead();

            
            string labels[] = {"Total number of cases recorded", "Total sick cases recorded", "Current Number Infected but not Sick", "Current Number Sick",
                               "Current Healthy Persons", "Total number of Recoveries", "Number of Deaths"};
            
            int numbers[] = {num_cases_recorded, num_sick, get_num_infected(), get_num_sick(), ((int) adjlist.size()-get_num_dead()-get_num_sick()-get_num_infected()) , recoveries_recorded, get_num_dead()};
            float percentages[] = {0, 0, 0, 0, 100, 0, 0};
            if(num_cases_recorded != 0){
                                 percentages[0] = ((float)num_cases_recorded / adjlist.size()) * 100;
                                 percentages[1] = ((float)num_sick / num_cases_recorded) * 100;
                                 percentages[2] = ((float) get_num_infected() / adjlist.size()) * 100;
                                 percentages[3] = ((float) get_num_sick() / adjlist.size()) * 100;
                                 percentages[4] = ((float) (adjlist.size() - get_num_dead() - get_num_sick() - get_num_infected()) / adjlist.size()) * 100;
                                 percentages[5] = ((float)recoveries_recorded / num_cases_recorded) * 100;
                                 percentages[6] = ((float)get_num_dead() / num_cases_recorded) * 100;
                                };

            cout << "  \n*******************************Statistics for the spread of Corona Virus in the population******************************* \n" << endl;
            cout << "                                   Total number of Persons: " << adjlist.size() << "                                             " << endl;
            for(int i = 0; i < 7; i++){
                const char* tempString = labels[i].c_str();
                printf("%40s %10d %10s %6.4f %%", tempString, numbers[i], " ", percentages[i]);
                switch (i)
                {
                case 0:
                    printf(" of the Population.");
                    break;
                case 1:
                    printf(" of Total Number of Cases Recorded.");
                    break;
                case 2:
                    printf(" of the Population.");
                    break;
                case 3:
                    printf(" of the Population.");
                    break;
                case 4:
                    printf(" of the Population.");
                    break;
                case 5:
                    printf(" of Total Number of Cases Recorded.");
                    break;
                case 6:
                    printf(" of Total Number of Cases Recorded.");
                    break;
                default:
                    break;
                }
                cout << endl;
            }
            cout << "\n" << endl;

        }


};

/*

int main(){
	Graph corona_graph = Graph();

    ifstream population("population.dat");
    ifstream connections("connections.dat");

    if (!population)
    {
        std::cerr << "Uh oh, population.dat could not be opened for reading!" << std::endl;
        return 1;
    }
    if (!connections)
    {
        std::cerr << "Uh oh, connections.dat could not be opened for reading!" << std::endl;
        return 1;
    }

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

    // Printing the statistics before the simulation
    corona_graph.printStatistics(false);

    // Simulating
    while(!corona_graph.infect_patient_zero()){
        corona_graph.infect_patient_zero();
    }
    
    for(int i = 0; i < 100; i++){
        corona_graph.simulate_spread();
    }

    // Printing the statistics after the simulation
    cout << "After the simulation.";
    corona_graph.printStatistics(true);

   

}
*/