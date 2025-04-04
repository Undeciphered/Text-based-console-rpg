#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include <cstdlib>
#include <algorithm>


class monster {
    protected:
		int health{0};
        int max_health{0};
        std::string name{""};
        bool healed{false};
        
    public:
        bool was_healed() {
            return healed;
        }
    
        monster(std::string Name) {
            name = Name;
        }
    
        void hit(int damage) {
            health -= damage;
        }

        int get_health() {
            return health;
        }
        
        void heal(int heal_amount) {
            if(heal_amount >= 0) {
                health += heal_amount;
                if(health > max_health) {
                    health = max_health; 
                }
                healed = true;
            }
        }

        void print_health() {
            std::cout << name << " health: " << health << '\n'; 
        }
};

class goblin : public monster {
    public:
        goblin(const std::string &name) : monster(name) {
            max_health = 100;
            health = max_health;

        }
};

class goblin_warlock : public monster {
    public:
        goblin_warlock(const std::string &name) : monster(name) {
            max_health = 400;
            health = max_health;
        }
        
        void heal_allies(std::vector<std::unique_ptr<monster>> &allies) { // heals at max the five lowest health monsters for 50HP, but no monster can get healed twice in a turn
            bool are_all_healed{std::all_of(allies.begin(), allies.end(),[](const std::unique_ptr<monster>& a) {return (a->was_healed());})}; // makes sure you dont heal any monster twice / you cant heal an already healed monster
                for(int i = 0; i < 5 && !are_all_healed; i++) {
                    auto it = std::min_element(allies.begin(), allies.end(),[](const std::unique_ptr<monster>& a, const std::unique_ptr<monster>& b) { // finds the goblin with the lowest unhealed health
                        if(!a->was_healed() && b->was_healed()) { // if "b" was healed and "a" was not then "a" is automaticaly the smalest unhealed
                            return true;    
                        }
                        if(a->was_healed() && !b->was_healed()) { // if "a" was healed and "b" was not then "a" is not the smalest unhealed
                            return false;   
                        }
                        return(a->get_health() < b->get_health()); // is "a" the smallest neither have been healed
                    });
                    (*it)->heal(50);
                    are_all_healed = std::all_of(allies.begin(), allies.end(),[](const std::unique_ptr<monster>& a) {return (a->was_healed());}); // re-check if all monsters are healed
                }
                std::cout << "----Heal Allies!----\n";
            }
};

std::vector<std::unique_ptr<monster>> spawn_goblins(int number_of_goblins) {
    std::vector<std::unique_ptr<monster>> Goblins;
    for(int i = 1; i <= number_of_goblins; i++) {
        Goblins.push_back(std::make_unique<goblin>("Goblin " + std::to_string(i)));
    }
    return Goblins;
}

void print_goblins(std::vector<std::unique_ptr<monster>> &Goblins) {
    for(auto &c : Goblins) {
        c -> print_health();
    } 
}

void splash_attack(std::vector<std::unique_ptr<monster>> &Goblins) {
    for(auto &c : Goblins) {
        c -> hit((std::rand() % 11) * 10);
    }
    std::cout << "---Splash Attack!---\n";
    
}

int main() {
    std::srand(std::time(nullptr)); // seeds random generator
    
    std::vector<std::unique_ptr<monster>> goblins = spawn_goblins(7); // spawns goblins
    std::unique_ptr<goblin_warlock> warlock = std::make_unique<goblin_warlock>("warlock 1"); // spawns a goblin warlock
    
    splash_attack(goblins);  
    print_goblins(goblins);
    
    warlock -> heal_allies(goblins);
    print_goblins(goblins);
    
    return 0;
}
