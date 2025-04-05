#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include <cstdlib>
#include <algorithm>

class entity {
    protected:
        int health{0};
        int max_health{0};
        std::string name{""};
        bool healed{false};
    public:
        entity(const std::string &Name) {
            name = Name;
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

        void hit(int damage) {
            health -= damage;
        }
    
        std::string get_name() {
            return name;
        }

        int get_health() {
            return health;
        }

        bool was_healed() {
            return healed;
        }
        
        template<typename T>
        void attack(std::unique_ptr<T> &target, int damage) {
            target->hit(damage);
            std::cout<< this->get_name() << " Attacks " << target->get_name() << "!\n";
        }

};

class monster : public entity {
    public:
        monster(const std::string &name) : entity(name) {}
        
        
};

class player : public entity {
    private:
        int experience_points{0};
        int level{0};
    
    public:
        player(const std::string &name) : entity(name) {
            max_health = 1000;
            health = max_health;
        }
        
    void check_level_up() {
        int experience_points_needed{((level+1) * (level+1) * 5)};
        while(experience_points >= experience_points_needed) {
            level++;
            experience_points -= experience_points_needed;
            experience_points_needed = ((level+1) * (level+1) * 5);
        }
    }
        
    void gain_experience_points(int &experience_points_gained) {
        experience_points += experience_points_gained;
        check_level_up();
        
    }
    
    int get_experience_points() {
        return experience_points;   
    }
    
    int get_level() {
        return level;   
    }

};

class goblin : public monster {
    public:
        goblin(const std::string &name) : monster(name) {
            max_health = 100;
            health = max_health;
        }
};

class ghoul : public monster {
    public:
        ghoul(const std::string &name) : monster(name) {
            max_health = 200;
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

std::vector<std::unique_ptr<monster>> spawn_monsters(int number_of_goblins) { // 0 1 2 3 4 | 5 6 7 8 9
    std::vector<std::unique_ptr<monster>> monsters;
    int random_number;
    for(int i = 1; i <= number_of_goblins; i++) {
        random_number = (std::rand() % 10);
        if(random_number > 4) {
            monsters.push_back(std::make_unique<goblin>("Goblin " + std::to_string(i)));
        } else if(random_number < 5) {
            monsters.push_back(std::make_unique<ghoul>("ghoul " + std::to_string(i)));
        }
    }
    return monsters;
}

void print_monsters(std::vector<std::unique_ptr<monster>> &monsters) {
    for(auto &c : monsters) {
        std::cout << "| " << c->get_name() << ": " << c->get_health() << "HP "; 
    }
    std::cout << "|\n";
}

void splash_attack(std::vector<std::unique_ptr<monster>> &monsters) {
    for(auto &c : monsters) {
        c -> hit((std::rand() % 11) * 10);
    }
    std::cout << "---Splash Attack!---\n";
}

int main() {
    std::srand(std::time(nullptr)); // seeds random generator
    std::vector<std::unique_ptr<monster>> monsters;
    monsters = spawn_monsters(4); // spawns monsters
    std::unique_ptr<goblin_warlock> warlock = std::make_unique<goblin_warlock>("warlock 1"); // spawns a warlock
    
    std::unique_ptr<player> the_player = std::make_unique<player>("Undeciphered");
    
    int gained{3250+57}; // temp varibale for testing
    
    the_player->gain_experience_points(gained);
    std::cout << the_player->get_experience_points() << " exp, level:" << the_player->get_level();
    
    
    return 0;
}
