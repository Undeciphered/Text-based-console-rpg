#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <map>

class entity {
    protected:
        bool can_attack{true};
        int health{0};
        int max_health{0};
        std::string name{""};
        bool healed{false};
        int damage{10};
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
};

class monster;
std::vector<std::unique_ptr<monster>> monsters;

class player : public entity {
    private:
        std::map<std::string, std::pair<bool, int>> debuffs {
        {"weakened",{false, 0}},
        {"prone",{false, 0}},
        {"ablaze",{false, 0}},
        {"drenched",{false, 0}},
        {"acidified",{false, 0}},
        {"hexed",{false, 0}},
        {"bloodied",{false, 0}}
        };
        int experience_points{0};
        int level{0};
        int max_mana{50};
        int mana{50};
        bool was_mana_restored{false};
        bool can_use_mana{true};
        double weakened_amount{1};
    
    public:
        player(const std::string &name) : entity(name) {
            max_health = 100;
            health = max_health;
        }
        
        void attack(std::unique_ptr<monster> &target, int damage);
        
        void set_debuff(std::string debuff, int info) {
            debuffs[debuff] = {true, info};
        }
        
        void handle_debuffs() {
            if(debuffs["drenched"].first == true) {
                debuffs["acidified"].first = false;
                debuffs["ablaze"].first = false;
                mana -= 10;
                debuffs["drenched"].first = false;
            }
            if(debuffs["weakened"].first == true) {
                weakened_amount = 0.5;
                debuffs["weakened"].second -= 1;
                if (debuffs["weakened"].second == 0) {
                    debuffs["weakened"].first = false;
                }
            }
            if(debuffs["prone"].first == true) {
                can_attack = false;
                debuffs["prone"].second -= 1;
                if (debuffs["prone"].second == 0) {
                    debuffs["prone"].first = false;
                    can_attack = true;
                }
            }
            if(debuffs["ablaze"].first == true) {
                health -= 10;
                debuffs["ablaze"].second -= 1;
                if (debuffs["ablaze"].second == 0) {
                    debuffs["ablaze"].first = false;
                }
            }
            if(debuffs["acidified"].first == true) {
                health -= 10;
                debuffs["acidified"].second -= 1;
                if (debuffs["acidified"].second == 0) {
                    debuffs["acidified"].first = false;
                }
            }
            if(debuffs["hexed"].first == true) {  
                can_use_mana = false;
                debuffs["hexed"].second -= 1;
                if (debuffs["hexed"].second == 0) {
                    debuffs["hexed"].first = false;
                    can_use_mana = true;
                }
            }
            if(debuffs["bloodied"].first == true) {  
                health -= 5;
                debuffs["bloodied"].second -= 1;
                if (debuffs["bloodied"].second == 0) {
                    debuffs["bloodied"].first = false;
                }
            }
        }
        
        void print_debuffs() {
            for(auto &c : debuffs) {
                std::cout << c.first << ": " << c.second.first << "," << c.second.second << '\n'; 
            }
            
        }
        
        void check_level_up() {
            int experience_points_needed{((level+1) * (level+1) * 3)};
            while(experience_points >= experience_points_needed) {
                level++;
                max_health += 15;
                max_mana += 5;
                mana_restore(1000);
                heal(1000);
                damage = 20 + level * level * 0.15;
                experience_points -= experience_points_needed;
                experience_points_needed = ((level+1) * (level+1) * 3);
            }
        }
        
        void gain_experience_points(int &experience_points_gained) {
            experience_points += experience_points_gained;
            check_level_up();
            
        }
        
        int get_level() {
            return level;   
        }
        
        void mana_restore(int restore_amount) {
            if(restore_amount >= 0) {
                mana += restore_amount;
                if(mana > max_mana) {
                    mana = max_mana; 
                }
                was_mana_restored = true;
            }
        }
        
        int get_mana() {
            return mana;
        }

};

class monster : public entity {
    public:
        monster(const std::string &name) : entity(name) {}
        
        void attack(std::unique_ptr<player> &target) {
            target->hit(damage);
        }
};

void player::attack(std::unique_ptr<monster> &target, int damage) {
    target->hit(damage * weakened_amount);
    std::cout<< this->get_name() << " Attacks " << target->get_name() << "!\n";
}

class hydra : public monster {
    public:
        hydra(const std::string &name) : monster(name) {
            max_health = 250;
            health = max_health;
            damage = 15;
        }
        
        void attack(std::unique_ptr<player> &target) {
            int random = (std::rand() % 4); // 0 1 2 | 3
            if(random < 3) {
                target->hit(damage); // stomp
            } else if(random > 2) {
                target->hit(damage); // acid spray
                target->set_debuff("acidified", 4);
            } 
        }
};

class wraith : public monster {
    public:
        wraith(const std::string &name) : monster(name) {
            max_health = 150;
            health = max_health;
            damage = 0;
        }
        
        void attack(std::unique_ptr<player> &target) {
            target->hit(damage); // haunt
            target->set_debuff("hexed", 2);
        }
};

class bat : public monster {
    public:
        bat(const std::string &name) : monster(name) {
            max_health = 50;
            health = max_health;
            damage = 5;
        }
};

class imp : public monster {
    public:
        imp(const std::string &name) : monster(name) {
            max_health = 70;
            health = max_health;
            damage = 10;
        }
    
};

class wyrm : public monster {
    public:
        wyrm(const std::string &name) : monster(name) {
            max_health = 100;
            health = max_health;
            damage = 10;
        }
    
};

class stone_golem : public monster {
    public:
        stone_golem(const std::string &name) : monster(name) {
            max_health = 500;
            health = max_health;
            damage = 50;
        }
        
        void attack(std::unique_ptr<player> &target) {
            int random = (std::rand() % 4); // 0 1 2 | 3
            if(random < 3) {
                target->hit(damage); // smash
                random = (std::rand() % 2); // 1 | 2
                if(random > 1) {
                    target->set_debuff("weakened", 2);   
                }
            } else if(random > 2) {
                target->hit(damage + 25); // crush
                target->set_debuff("prone", 2);
            } 
        }
};


class armored_dragon : public monster {
    public:
        armored_dragon(const std::string &name) : monster(name) {
            max_health = 500;
            health = max_health;
            damage = 50;
        }
        
        void attack(std::unique_ptr<player> &target) {
            int random = (std::rand() % 4); // 0 1 | 2 3
            if(random < 2) {
                target->hit(damage); // breathe fire
                target->set_debuff("ablaze", 4);
            } else if(random > 1) {
                target->hit(damage); // stomp
            } 
        }
    
};

class lord_cthulhu : public monster {
    public:
        lord_cthulhu(const std::string &name) : monster(name) {
            max_health = 400;
            health = max_health;
            damage = 25;
        }
        
        void attack(std::unique_ptr<player> &target) {
            int random = (std::rand() % 5); // 0 1 | 2 3 4
            if(random < 2) {
                target->hit(damage); // water torrent
                target->set_debuff("drenched", 0);
            } else if(random > 1) {
                target->hit(damage + 5); // magic missile
            } 
        }
};

class the_reaper : public monster {
    public:
        the_reaper(const std::string &name) : monster(name) {
            max_health = 10000;
            health = max_health;
            damage = 1000;
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
            max_health = 150;
            health = max_health;
            damage = 10;
        }
        
        void attack(std::unique_ptr<player> &target) {
            int random = (std::rand() % 5); // 0 1 | 2
            if(random < 2) { // takle
                target->hit(damage);
            } else if(random > 1) {
                target->hit(damage); // scratch
                target->set_debuff("bloodied", 3);
            } 
        }
};

class warlock : public monster {
    public:
        warlock(const std::string &name) : monster(name) {
            max_health = 400;
            health = max_health;
        }
        
        void attack(std::unique_ptr<player> &target) {
            int random = (std::rand() % 5); // 0 1 | 2 3 4
            if(random < 2) { // heals alies
                heal_allies(); 
            } else if(random > 1) {
                target->hit(damage); // magic missile
                random = (std::rand() % 3); // 0 1 | 2
                if(random > 1) {
                    target->set_debuff("hexed", 2);   
                }
            } 
        }
        
        void heal_allies() { // heals at max the five lowest health monsters for 50HP, but no monster can get healed twice in a turn
            bool are_all_healed{std::all_of(monsters.begin(), monsters.end(),[](const std::unique_ptr<monster>& a) {return (a->was_healed());})}; // makes sure you dont heal any monster twice / you cant heal an already healed monster
                for(int i = 0; i < 5 && !are_all_healed; i++) {
                    auto it = std::min_element(monsters.begin(), monsters.end(),[](const std::unique_ptr<monster>& a, const std::unique_ptr<monster>& b) { // finds the goblin with the lowest unhealed health
                        if(!a->was_healed() && b->was_healed()) { // if "b" was healed and "a" was not then "a" is automaticaly the smalest unhealed
                            return true;    
                        }
                        if(a->was_healed() && !b->was_healed()) { // if "a" was healed and "b" was not then "a" is not the smalest unhealed
                            return false;   
                        }
                        return(a->get_health() < b->get_health()); // is "a" the smallest neither have been healed
                    });
                    (*it)->heal(50);
                    are_all_healed = std::all_of(monsters.begin(), monsters.end(),[](const std::unique_ptr<monster>& a) {return (a->was_healed());}); // re-check if all monsters are healed
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
    
    //monsters = spawn_monsters(4); // spawns monsters
    std::unique_ptr<stone_golem> testmonster = std::make_unique<stone_golem>("golem 1"); // spawns a warlock
    
    std::unique_ptr<player> the_player = std::make_unique<player>("Undeciphered");
    
    testmonster->attack(the_player);
    testmonster->attack(the_player);
    testmonster->attack(the_player);
    testmonster->attack(the_player);
    the_player->heal(1000);
    the_player->print_debuffs();
    
    return 0;
}
