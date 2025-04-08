#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <map>
#include <queue>
#include <cmath>

class entity {
    protected:
        bool can_attack{true};
        int health{0};
        int max_health{0};
        std::string name{""};
        bool healed{false};
        int damage{30};
    public:
        entity(const std::string &Name) {
            name = Name;
        }
        
        virtual ~entity() = default;

        void heal(int heal_amount) {
            if(heal_amount >= 0) {
                health += heal_amount;
                if(health > max_health) {
                    health = max_health; 
                }
                healed = true;
            }
        }

        void hit(int specified_damage) {
            health -= specified_damage;
            std::cout << " dealing " << specified_damage << " damage\n";
        }
    
        std::string get_name() {
            return name;
        }
        
        int get_damage() {
            return damage;   
        }

        int get_health() {
            return health;
        }
        
        int get_max_health() {
            return max_health;   
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
        
        virtual ~player() = default;
        
                int get_max_mana() {
            return max_mana;   
        }
        
        int get_max_health() {
            return max_health;   
        }
        
        void use_mana(int amount_used) {
            mana-= amount_used;   
        }
        
        void attack(std::unique_ptr<monster> &target, int damage_boost);
        
        void set_debuff(std::string debuff, int info) {
            debuffs[debuff] = {true, info};
        }
        
        void handle_debuffs() {
            if(debuffs["drenched"].first == true) {
                debuffs["acidified"].first = false;
                debuffs["ablaze"].first = false;
                std::cout << "10 Mana was washed away\n";
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
                std::cout << "the flames lick away at your skin dealing 10 damage\n";
                health -= 10;
                debuffs["ablaze"].second -= 1;
                if (debuffs["ablaze"].second == 0) {
                    debuffs["ablaze"].first = false;
                }
            }
            if(debuffs["acidified"].first == true) {
                std::cout << "the acid eats away at your flesh dealing 10 damage\n";
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
                std::cout << "your open wounds sear in pain dealing 5 damage\n";
                health -= 5;
                debuffs["bloodied"].second -= 1;
                if (debuffs["bloodied"].second == 0) {
                    debuffs["bloodied"].first = false;
                }
            }
        }
        
        void check_level_up() {
            int experience_points_needed{(25+(25 * (level+1)))};
            while(experience_points >= experience_points_needed) {
                level++;
                max_health += 15;
                max_mana += 5;
                mana_restore(1000);
                heal(1000);
                damage = 20 + level * level * 0.15;
                experience_points -= experience_points_needed;
                experience_points_needed = (25+(25 * (level+1)));
                std::cout << name << " leveled up to level " << level << '\n';
            }
        }
        
        void gain_experience_points(int experience_points_gained) {
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
        
        virtual void attack(std::unique_ptr<player> &target) {
            target->hit(damage);
            std::cout << "should be overwriten";
        }
        
        virtual ~monster() = default;
};

void player::attack(std::unique_ptr<monster> &target, int damage_boost) {
    std::cout<< this->get_name() << " Attacked " << target->get_name();
    target->hit((this->damage + damage_boost) * weakened_amount);
    if(target->get_health() <= 0) {
        int experiance_gained{static_cast<int>(pow(target->get_max_health(), (1+(target->get_damage()/150))))};
        std::cout << this->get_name() << " killed " << target->get_name() << " gaining " << experiance_gained << "EXP\n";
        gain_experience_points(experiance_gained);
    }
}

class hydra : public monster {
    public:
        hydra(const std::string &name) : monster(name) {
            max_health = 250;
            health = max_health;
            damage = 15;
        }
        
        void attack(std::unique_ptr<player> &target) override {
            int random = (std::rand() % 4); // 0 1 2 | 3
            if(random < 3) {
                std::cout << this->get_name() << " stomped on " << target->get_name();
                target->hit(damage); // stomp
            } else if(random > 2) {
                std::cout << this->get_name() << " sprayed " << target->get_name() << " with its deady acid";
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
            damage = 15;
        }
        
        void attack(std::unique_ptr<player> &target) override {
            std::cout << this->get_name() << " envelopes " << target->get_name() << " in cursed spirits";
            target->hit(damage-15); // haunt
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
        
        void attack(std::unique_ptr<player> &target) override {
            std::cout << this->get_name() << " tormented " << target->get_name();
            target->hit(damage);
        }
};

class imp : public monster {
    public:
        imp(const std::string &name) : monster(name) {
            max_health = 70;
            health = max_health;
            damage = 10;
        }
        
        void attack(std::unique_ptr<player> &target) override {
            std::cout << this->get_name() << " bit " << target->get_name() << " with its sharp teeth";
            target->hit(damage);
        }
    
};

class wyrm : public monster {
    public:
        wyrm(const std::string &name) : monster(name) {
            max_health = 100;
            health = max_health;
            damage = 15;
        }
        
        void attack(std::unique_ptr<player> &target) override {
            std::cout << this->get_name() << " bashed its thick head into " << target->get_name();
            target->hit(damage);
        }
    
};

class stone_golem : public monster {
    public:
        stone_golem(const std::string &name) : monster(name) {
            max_health = 500;
            health = max_health;
            damage = 50;
        }
        
        void attack(std::unique_ptr<player> &target) override {
            int random = (std::rand() % 4); // 0 1 2 | 3
            if(random < 3) {
                std::cout << this->get_name() << " smashed " << target->get_name() << "into the ground";
                target->hit(damage); // smash
                random = (std::rand() % 2); // 1 | 2
                if(random > 1) {
                    target->set_debuff("weakened", 2);   
                }
            } else if(random > 2) {
                std::cout << this->get_name() << " crushed " << target->get_name() << " with a large boulder";
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
        
        void attack(std::unique_ptr<player> &target) override {
            int random = (std::rand() % 4); // 0 1 | 2 3
            if(random < 2) {
                std::cout << this->get_name() << " set " << target->get_name() << "on fire with its firey breath";
                target->hit(damage); // breathe fire
                target->set_debuff("ablaze", 4);
            } else if(random > 1) {
                std::cout << this->get_name() << " stomped on " << target->get_name();
                target->hit(damage); // stomp
            } 
        }
    
};

class lord_cthulhu : public monster {
    public:
        lord_cthulhu(const std::string &name) : monster(name) {
            max_health = 400;
            health = max_health;
            damage = 20;
        }
        
        void attack(std::unique_ptr<player> &target) override {
            int random = (std::rand() % 5); // 0 1 | 2 3 4
            if(random < 2) {
                std::cout << this->get_name() << " doused " << target->get_name() << "with a torrent of water";
                target->hit(damage); // water torrent
                target->set_debuff("drenched", 0);
            } else if(random > 1) {
                std::cout << this->get_name() << "shot a magic missile at " << target->get_name();
                target->hit(damage + 10); // magic missile
            } 
        }
};

class goblin : public monster {
    public:
        goblin(const std::string &name) : monster(name) {
            max_health = 100;
            damage = 10;
            health = max_health;
        }
        
        void attack(std::unique_ptr<player> &target) override {
            std::cout << this->get_name() << " clubbed " << target->get_name() << " whith a bat";
            target->hit(damage); 
        }
};

class ghoul : public monster {
    public:
        ghoul(const std::string &name) : monster(name) {
            max_health = 150;
            health = max_health;
            damage = 10;
        }
        
        void attack(std::unique_ptr<player> &target) override {
            int random = (std::rand() % 5); // 0 1 | 2
            if(random < 2) { // tackle
                std::cout << this->get_name() << " tackled " << target->get_name();
                target->hit(damage);
            } else if(random > 1) {
                std::cout << this->get_name() << "scratched " << target->get_name();
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
        
        void attack(std::unique_ptr<player> &target) override {
            int random = (std::rand() % 5); // 0 1 | 2 3 4
            if(random < 2) { // heals alies
                heal_allies(); 
            } else if(random > 1) {
                std::cout << this->get_name() << "shot a magic missile at " << target->get_name();
                target->hit(damage); // magic missile
                random = (std::rand() % 3); // 0 1 | 2
                if(random > 1) {
                    target->set_debuff("hexed", 2);   
                }
            } 
        }
        
        void heal_allies() { // heals at max the two lowest health monsters for 50HP, but no monster can get healed twice in a turn
            bool are_all_healed{std::all_of(monsters.begin(), monsters.end(),[](const std::unique_ptr<monster>& a) {return (a->was_healed());})}; // makes sure you dont heal any monster twice / you cant heal an already healed monster
            for(int i = 0; i < 2 && !are_all_healed; i++) {
                auto it = std::min_element(monsters.begin(), monsters.end(),[](const std::unique_ptr<monster>& a, const std::unique_ptr<monster>& b) { // finds the goblin with the lowest unhealed health
                    if(!a->was_healed() && b->was_healed()) { // if "b" was healed and "a" was not then "a" is automaticaly the smalest unhealed
                        return true;    
                    }
                    if(a->was_healed() && !b->was_healed()) { // if "a" was healed and "b" was not then "a" is not the smalest unhealed
                        return false;   
                    }
                    return(a->get_health() < b->get_health()); // is "a" the smallest?
                });
                (*it)->heal(50);
                std::cout << "healed " << (*it)->get_name() << " for 50HP\n";
                are_all_healed = std::all_of(monsters.begin(), monsters.end(),[](const std::unique_ptr<monster>& a) {return (a->was_healed());}); // re-check if all monsters are healed
            }
        }
};

void print_seperator() {
    std::cout << "^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v\n";
}

void print_character(std::unique_ptr<player> &the_player) {
    print_seperator();
    int monster_amount{static_cast<int>(monsters.size())};
    for(int i = 0; i < monster_amount; i++) {
        std::cout << "| <" << i << ">" << monsters[i]->get_name() << ": " << monsters[i]->get_health() << "HP "; 
    }
    std::cout << "|\n" << "<Level:" << the_player->get_level() << ">" << the_player->get_name() << ": " << the_player->get_health() << "/" << the_player->get_max_health() << "HP, ";
    std::cout << the_player->get_mana() << "/" << the_player->get_max_mana() << "Mana\n\n";
}

std::queue<int> initialise_enemy_lineup() {
    std::queue<int> enemy_lineup;
    int random = (std::rand() % 2) + 2; // 0 1 -> 2 3
    while(random > 0) {  // some ones
        enemy_lineup.push(1);
        random--;
    }
    random = (std::rand() % 2) + 2; // 0 1 -> 2 3
    while(random > 0) { // some twos some ones
        if((std::rand() % 4) < 1) {
            enemy_lineup.push(1);
        } else {
            enemy_lineup.push(2);
        }
        random--;    
    }
    random = (std::rand() % 2) + 1; // 0 1 -> 1 2
    while(random > 0) {  // some twos
        enemy_lineup.push(2);
        random--;
    }
    random = (std::rand() % 2); // 0 1
    while(random > 0) {  // maybe a three
        enemy_lineup.push(3);
        random--;
    }
    random = (std::rand() % 2) + 1; // 0 1 -> 1 2
    while(random > 0) {  // some more twos
        enemy_lineup.push(2);
        random--;
    }
    random = (std::rand() % 3) + 1; // 0 1 2 -> 1 2 3
    while(random > 0) { // some threes some twos
        if((std::rand() % 3) < 1) {
            enemy_lineup.push(2);
        } else {
            enemy_lineup.push(3);
        }
        random--;    
    }
    random = (std::rand() % 2) + 1; // 0 1 -> 1 2
    while(random > 0) {  // some threes
        enemy_lineup.push(3);
        random--;
    }
    random = (std::rand() % 1) + 1; // 0 -> 1
    while(random > 0) {  // a boss
        enemy_lineup.push(4);
        random--;
    }
    return enemy_lineup;
}

void spawn_monster_type_one(int spawnable_amount) {
    int random{0};
    while(spawnable_amount != 0) {
        random = std::rand() % 3;
        if(random == 0) {monsters.push_back(std::make_unique<bat>("Bat"));}
        if(random == 1) {monsters.push_back(std::make_unique<imp>("Imp"));}
        if(random == 2) {monsters.push_back(std::make_unique<goblin>("Goblin"));}
        spawnable_amount--;
    }
}

void spawn_monster_type_two(int spawnable_amount) {
    int random{0};
    while(spawnable_amount != 0) {
        random = std::rand() % 3;
        if(random == 0) {monsters.push_back(std::make_unique<wyrm>("Wyrm"));}
        if(random == 1) {monsters.push_back(std::make_unique<wraith>("Wraith"));}
        if(random == 2) {monsters.push_back(std::make_unique<ghoul>("Ghoul"));}
        spawnable_amount--;
    }
}

void spawn_monster_type_three(int spawnable_amount) {
    int random{0};
    while(spawnable_amount != 0) {
        random = std::rand() % 5;
        if(random == 0) {monsters.push_back(std::make_unique<lord_cthulhu>("Lord Cthulhu"));}
        if(random == 1 || random == 2) {monsters.push_back(std::make_unique<hydra>("Hydra"));}
        if(random == 3 || random == 4) {monsters.push_back(std::make_unique<warlock>("Warlock"));}
        spawnable_amount--;
    }
}

void spawn_monster_type_four(int spawnable_amount) {
    int random{0};
    while(spawnable_amount != 0) {
        random = std::rand() % 3;
        if(random < 1) {monsters.push_back(std::make_unique<stone_golem>("Stone Golem"));}
        if(random > 0) {monsters.push_back(std::make_unique<armored_dragon>("Armored Dragon"));}
        spawnable_amount--;
    }
}

void spawn_monsters(int level, std::queue<int> &enemy_linup) {
    monsters.clear();
    int spawnable_amount = (std::round(pow(level,1.3) / 10) + 1);
    int monster_dificulty = enemy_linup.front();
    int random{0};
    
    if(monster_dificulty == 1) {
        spawn_monster_type_one(spawnable_amount);
    }
    if(monster_dificulty == 2) {
        spawn_monster_type_two(1);
        spawnable_amount--;
        for(int i = 0; i < spawnable_amount; i++) {
           random = std::rand() % 3; // 0 | 1 2 
           if(random > 0) {spawn_monster_type_one(1);}
           if(random < 1) {spawn_monster_type_two(1);}
        }
    }
    if(monster_dificulty == 3) {
        spawn_monster_type_three(1);
        spawnable_amount--;
        for(int i = 0; i < spawnable_amount; i++) {
           random = std::rand() % 4; // 0 | 1 2 3
           if(random > 0) {spawn_monster_type_two(1);}
           if(random < 1) {spawn_monster_type_three(1);}
        }
    }
    if(monster_dificulty == 4) {
        spawn_monster_type_four(1);
    }
    enemy_linup.pop();
}

void print_queue(std::queue<int> myqueue) {
    while(!myqueue.empty()) {
        std::cout << myqueue.front() << " ";
        myqueue.pop();
    }
    std::cout << '\n';
}

int get_input(int range_start, int range_end, int must_divisible_by) {
    std::string input{""};
    int chosen_number{0};
    while(true) {
        std::cin >> input;
        if(input == "exit") {return -1;}
        if(std::all_of(input.begin(), input.end(), ::isdigit)) {
            chosen_number = stoi(input);
        }
        if(chosen_number >= range_start && chosen_number <= range_end && chosen_number > -1 && chosen_number % must_divisible_by == 0) {
            return chosen_number;
        }
    } 
}

bool player_turn(std::unique_ptr<player> &the_player) {
    print_character(the_player);
    std::cout << the_player->get_name() << "'s turn\n1 : Attack\n2 : Heal\n";
    int player_action{get_input(1,2,1)};
    if(player_action == -1) {
        return true;
    }
    if(player_action == 1) {
        std::cout << "What monster will you attack?: ";
        int attack_action{get_input(0,(int)monsters.size()-1,1)};
        the_player->attack(monsters[attack_action], 0);
        the_player->mana_restore(5);
        if(monsters[attack_action]->get_health() <= 0) {
            monsters.erase(monsters.begin() + attack_action);
        }
    }
    if(player_action == 2) {
        std::cout << "how much will you heal for? (5 mana per 10HP): ";
        int heal_action{get_input(0,(the_player->get_mana()/5)*10,10)};
        the_player->heal(heal_action); 
        the_player->use_mana((heal_action/10)*5);
        std::cout << the_player->get_name() << " was healed for " << heal_action << "HP\n";
    }
    return false;
}

void monster_turn(std::unique_ptr<player> &the_player) {
    int monster_amount{static_cast<int>(monsters.size())};
    int amount_can_attack{(monster_amount/2)};
    if(amount_can_attack <= 0) {amount_can_attack = 1;}
    int random{0};
    for(int i = 0; i < amount_can_attack; i++) {
        random = std::rand() % monster_amount; 
        monsters[random]->attack(the_player);
    }
    print_seperator();
    std::cout << "\n\n\n";
}

void gameplay_loop() {
    std::queue<int> enemy_lineup = initialise_enemy_lineup();
    std::unique_ptr<player> the_player = std::make_unique<player>("Undeciphered");
    spawn_monsters(the_player->get_level(), enemy_lineup);
    bool game_over{false};
    while(!game_over){
        while(true){
            
            game_over = player_turn(the_player);
            if(monsters.size() <= 0 || game_over) {
                the_player->handle_debuffs();
                print_seperator();
                std::cout << "\n\n\n";
                break;   
            }
            monster_turn(the_player);
            the_player->handle_debuffs();
        
        }
        spawn_monsters(the_player->get_level(), enemy_lineup);
        
    }
}

int main() {
    std::srand(std::time(nullptr)); // seeds random generator
    gameplay_loop();
    

    return 0;
}
