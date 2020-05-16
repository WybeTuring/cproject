
/* Authour: Lemfon Karl Ndze'dzenyuy
 * Purpose: An implementation of a graph data structure that will be used throughout the next stages of the in-class C project
 * Design: The graph is implemented using an adjacency list. The list is used to avoid inefficient use of memory that can arise 
           from using an adjacency matrix when the graph is sparse. The graph is stored in a vector structure. 
 * 	       A class called Person, that holds information about the persons is created. For each person object, the attribute that
           is most notable is a vector that holds pointers to other people objects that the person can infect. By simply storing
           pointers to the People objects that a given Person is capable of infecting, we minimise the usage of memory and also reduce
           the number of searches that will be done during the simulation face. 
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <random>

using namespace std;

// This function takes a probability and returns true or false.
bool simulate_prob(float prob){

  prob = prob*1000;

  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> random_number(1,1000); // distribution in range [1, 1000]

  if ( random_number(rng) <= prob)
    return true;

  return false;
}

/* The Person class tries to represent the four states in which a person finds himself/herself in the following way;
    1. If the person is uninefected, then the boolean member infection_status is false;
    2. If the person is infected but not sick, then the boolean member infection_status is true, and the number of days infected is 0.
    3. If the person is infected and sick, then the boolean member infection_status is true, and the number of days infected is 1 or more. 
    4. It is very important that the test for whether the number of days a person is sick is > 7. 
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

        // This method prints the list of people that a given person can infect in the population.
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

// The graph class is stores the population as nodes and contains the methods for the various simulations and control policy.
class Graph{
	public:
		int largest_age = 1;
        int num_cases_recorded = 0; // This simply takes count of the number of infections recorded. 
        int num_sick = 0; // Takes note of the number of infected people who actually got sick;
        int recoveries_recorded = 0;
		vector<Person> adjlist;

		// This method adds a new node into the graph. 
		void add_node(Person person){
			adjlist.push_back(person);
			if(person.age > largest_age){largest_age = person.age;}
		}

		// This method inserts an edge to the graph. The key difference here is that it uses only the names as arguments.
		void add_edge(string person1_name, string person2_name){
			// The grand idea is to traverse the vector in the graph till we find the node that holds the persons 1 and 2, and then add the pointer pointing to person two in the "victims" attribute of person 1.
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
	
        // This methods prints the persons in the graph, alongside the people they can infect.
		void printPeople(){
			for(int i = 0; i < adjlist.size(); i++){
				adjlist[i].print_people_list();
			}
		}

        /*
        // This method prints the people in the graph, and the persons they can infect.
		void printLengths(){
			for(int i = 0; i < adjlist.size(); i++){
				cout << adjlist[i].name << " can infect " << adjlist[i].victims.size() << " people" << endl;
			}
		}
        */

        // This method sets the disease probability and recovery probability for each member of the population. Once the graph has been populated
        // and the maximum age has been determined, then the function is called.
        void set_disease_prob(){
            for(int i = 0; i < adjlist.size(); i++){
                adjlist[i].disease_prob = adjlist[i].age / (float) largest_age;
                adjlist[i].recovery_prob = 1 - adjlist[i].disease_prob;
            }
        } 

        // This methods infects the first person after choosing them randomly
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

        // This method goes through and lets infected people infect other people, using their spread probability.
        void infection_simulation(){
            for(int i = 0; i < adjlist.size(); i++){
                if(adjlist[i].infection_status && (adjlist[i].number_of_days <= 7)){  // The check of whether number of days is <= 7 confirms that the person is not dead.
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
                // Checking if the person is sick and alive.
                if((adjlist[i].number_of_days > 0) && (adjlist[i].number_of_days <= 7)){
                    if(simulate_prob(adjlist[i].recovery_prob)){
                        recoveries_recorded++;
                        adjlist[i].infection_status = false;
                        adjlist[i].number_of_days = 0; // Number of days is set to zero and the infection-status to false since the person can be infected again.
                    }
                }
            }
        }

        // This function carries out the simulation. Please before carrying out the simulation, always ensure that a random person has been infected.
        void simulate_spread(){
            /* It is possible that a person who cannot infect any one in the population will be  the random person infected.
                If that is the case, then the virus will not spread in the population. To prevent this scencario
                1. The infect_patient_zero() method only infects a random person and returns true when the victim size of the random person is not zero. 
                2. The call to infect a random person is repeated till someone who can infect others is infected.
            */
            while (num_cases_recorded == 0){
                infect_patient_zero();
            }
            infection_simulation();
            get_sick_simulation();
            recovery_simulation();
        }

        // This function resets the parameters of the population to enable a new simulation.
        void reset(){
            for(int i = 0; i < adjlist.size(); i++){
               adjlist[i].infection_status = false;
               adjlist[i].number_of_days = 0;
           }
           num_cases_recorded = 0;
           num_sick = 0;
           recoveries_recorded = 0;
        }
        /* Implementing a policy for mitigating the the coronavirus. The policy is described as follows;
            By definition, the disease probability is the age divided by the largest age in the population. 
            As we know, only people who are sick can die. Therefore, one way in which we can reduce the 
            number of deaths in the population is trying to make sure that old people are leasy affected. 
            One way of doing this, is by identifying the people that are in contact with old people, and then 
            reducing their spread probability by half. In practical situations, this reduction in spread pro-
            bability can come in the form of respecting social distancing measures, by isolating once symptoms
            are realised or by staying indoors. The idea is to promote herd immunity, by making sure that we
            protect the most vulnearable population, while allowing others to get infected. 
            The definition of an old person, is someone who is 35 percent as old as the oldest person. 
        */
       void control_policy(){
           // Making sure that no one is infected
           reset();
           // setting the spread probability of those who are in contact with old people to half it's current value 
           for(int i = 0; i < adjlist.size(); i++){
               for(int j = 0; j < adjlist[i].victims.size(); i++){
                   if( adjlist[i].victims[j]->age / (float)largest_age > 0.35){
                       adjlist[i].spread_prob = adjlist[i].spread_prob / 2.0;
                   }
                   break;
                   
               }
           }
           simulate_spread();
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

        // This function prints the summary statistics of the population in a user-friendly manner.
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


  