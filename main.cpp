#include <iostream>
#include <random>

enum type { Player = 0, Animal = 1, Human = 2 };
enum state { Clean = 0, Dirty = 2 };
enum action { Cleaned = -1, Dirtied = 1 };
enum direction { North = 0, South = 1, East = 2, West = 3 };

class Creature {
public:
    int num = 0;
    int type = 0;
    int position = 0;
};

class Room {
public:
    int num = 0;
    int state = 0;
    int occupancy = 0;

    Room *rooms[4] = {};
    Creature *creatures[10] = {};
};

void initializeRooms(Room rooms[], int numRooms);
void initializeCreatures(Room rooms[], Creature creatures[], int numCreatures);

void look();
void move(Creature *creature, int direction);
void react(Creature *creature, int action);
void leave(Creature *creature);
void erase(Creature *creature);

int pickDirection();

int respect = 40;

Room *location = nullptr;
Creature *player = nullptr;

int main() {
    int numRooms, numCreatures;

    std::cin >> numRooms;
    Room *rooms = new Room[numRooms];
    initializeRooms(rooms, numRooms);

    std::cin >> numCreatures;
    Creature *creatures = new Creature[numCreatures];
    initializeCreatures(rooms, creatures, numCreatures);

    while (true) {
        Creature *actor = player;
        std::string command;

        std::cout << std::endl << "> ";
        std::cin >> command;
        std::cout << std::endl;

        if (command.find(':') != std::string::npos) {
            actor = &creatures[std::stoi(command.substr(0, command.find(':')))];
            command = command.substr(command.find(':') + 1);
        }

        if (command == "help") {
            std::cout << "help  - displays this help message" << std::endl;
            std::cout << "look  - displays the current room's state" << std::endl;
            std::cout << "north - moves to the north" << std::endl;
            std::cout << "south - moves to the south" << std::endl;
            std::cout << "east  - moves to the east" << std::endl;
            std::cout << "west  - moves to the west" << std::endl;
            std::cout << "clean - cleans the room" << std::endl;
            std::cout << "dirty - dirties the room" << std::endl;
            std::cout << "exit  - exits the game" << std::endl;
            std::cout << "#:    - makes that creature act" << std::endl;

        } else if (command == "look") {
            look();

        } else if (command == "north") {
            move(actor, North);

        } else if (command == "south") {
            move(actor, South);

        } else if (command == "east") {
            move(actor, East);

        } else if (command == "west") {
            move(actor, West);

        } else if (command == "clean") {
            react(actor, Cleaned);

        } else if (command == "dirty") {
            react(actor, Dirtied);

        } else if (command == "exit") {
            std::cout << "Goodbye!" << std::endl;
            break;
        }

        if (respect < 1) {
            std::cout << "Shame on you! You lose." << std::endl;
            break;

        } else if (respect > 80) {
            std::cout << "Congratulation, you are praised!" << std::endl;
            break;
        }
    }

    delete[] rooms;
    delete[] creatures;

    return 0;
}

void initializeRooms(Room rooms[], int numRooms) {
    for (int i = 0; i < numRooms; i++) {
        int state, north, south, east, west;
        std::cin >> state >> north >> south >> east >> west;

        Room &room = rooms[i];

        room.num = i;
        room.state = state;

        if (north != -1) { room.rooms[North] = &rooms[north]; }
        if (south != -1) { room.rooms[South] = &rooms[south]; }
        if (east != -1) { room.rooms[East] = &rooms[east]; }
        if (west != -1) { room.rooms[West] = &rooms[west]; }
    }
}

void initializeCreatures(Room rooms[], Creature creatures[], int numCreatures) {
    for (int i = 0; i < numCreatures; i++) {
        int type, roomNum;
        std::cin >> type >> roomNum;

        Creature &creature = creatures[i];
        Room &room = rooms[roomNum];

        creature.num = i;
        creature.type = type;
        creature.position = room.occupancy;

        room.creatures[room.occupancy++] = &creature;

        if (type == Player) {
            player = &creature;
            location = &room;
        }
    }
}

void look() {
    std::string directions[] = {"north", "south", "east", "west"};
    std::string states[] = {"clean", "half-dirty", "dirty"};
    std::string types[] = {"player", "animal", "human"};

    std::cout << "Room " << location->num << ", " << states[location->state] << ", neighbors";

    for (int i = 0; i < 4; i++) {
        Room *room = location->rooms[i];
        if (room == nullptr) { continue; }

        std::cout << " " << room->num << " to the " << directions[i];
    }

    std::cout << ", contains: " << std::endl;

    for (int i = 0; i < location->occupancy; i++) {
        Creature *creature = location->creatures[i];
        std::cout << types[creature->type];

        if (creature != player) {
            std::cout << " " << creature->num;
        }

        std::cout << std::endl;
    }
}

void move(Creature *creature, int direction) {
    std::string directions[] = {"north", "south", "east", "west"};

    Room *room = location->rooms[direction];

    if (room == nullptr) {
        std::cout << "There is no room to the " << directions[direction] << "." << std::endl;
        return;
    }

    if (room != location && room->occupancy == 10) {
        std::cout << "Room " << room->num << " is full." << std::endl;
        return;
    }

    for (int i = creature->position; i < location->occupancy - 1; i++) {
        Creature *nextCreature = location->creatures[i + 1];

        nextCreature->position--;
        location->creatures[i] = nextCreature;
    }

    location->occupancy--;
    creature->position = room->occupancy;
    room->creatures[room->occupancy++] = creature;

    if (creature != player) {
        std::cout << creature->num << " leaves";

    } else {
        std::cout << "You leave";
        location = room;
    }

    std::cout << " towards the " << directions[direction] << "." << std::endl;
}

void react(Creature *creature, int action) {
    std::string reactions[] = {" licks your face", " smiles", " growls", " grumbles"};

    if (location->state == Clean && action == Cleaned) {
        std::cout << "The room is already clean." << std::endl;
        return;

    } else if (location->state == Dirty && action == Dirtied) {
        std::cout << "The room is already dirty." << std::endl;
        return;
    }

    location->state += action;

    if (creature != player) {
        std::cout << creature->num << " " << (action == Cleaned ? "cleans" : "dirties");

    } else {
        std::cout << "You " << (action == Cleaned ? "clean" : "dirty");
    }

    std::cout << " the room." << std::endl;

    for (int i = 0; i < location->occupancy; i++) {
        Creature *reactor = location->creatures[i];
        if (reactor == player) { continue; }

        respect = reactor->type == Animal ? respect - action : respect + action;
        std::cout << reactor->num << reactions[reactor->type == Animal ? 1 + action : 2 - action];

        if (reactor == creature) {
            respect = reactor->type == Animal ? respect - action * 2 : respect + action * 2;
            std::cout << " a lot";
        }

        std::cout << ". Respect is now " << respect << std::endl;

        if (reactor->type == Animal ? location->state == Dirty : location->state == Clean) {
            int occupancy = location->occupancy;
            leave(reactor);

            if (location->occupancy < occupancy) {
                i--;
            }
        }
    }
}

void leave(Creature *creature) {
    int direction = pickDirection();

    if (direction == -1) {
        erase(creature);
        return;
    }

    Room *newRoom = location->rooms[direction];
    move(creature, direction);

    if (creature->type == Animal && newRoom->state == Dirty) {
        newRoom->state--;

    } else if (creature->type == Human && newRoom->state == Clean) {
        newRoom->state++;
    }
}

void erase(Creature *creature) {
    std::string reactions[] = {" growls", " grumbles"};

    std::cout << creature->num << " leaves through the ceiling." << std::endl;

    for (int j = creature->position; j < location->occupancy - 1; j++) {
        Creature *nextCreature = location->creatures[j + 1];

        nextCreature->position--;
        location->creatures[j] = nextCreature;
    }

    location->occupancy--;

    for (int j = 0; j < location->occupancy; j++) {
        Creature *reactor = location->creatures[j];
        if (reactor == player) { continue; }

        std::cout << reactor->num << reactions[reactor->type - 1] << ". Respect is now " << --respect << std::endl;
    }
}

int pickDirection() {
    std::random_device rd;
    std::mt19937 mt(rd());

    int directions[4];
    int numDirections = 0;

    for (int i = 0; i < 4; i++) {
        Room *room = location->rooms[i];
        if (room == nullptr) { continue; }
        if (room != location && room->occupancy == 10) { continue; }

        directions[numDirections++] = i;
    }

    if (numDirections == 0) {
        return -1;
    }

    std::uniform_int_distribution<int> dist(0, numDirections - 1);
    return directions[dist(mt)];
}
