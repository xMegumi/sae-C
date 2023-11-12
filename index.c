#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define MAX 5

typedef struct {
    short room_id; // Fixed value
    unsigned short door_count; // Fixed value 
    unsigned short trap; // Random value
    unsigned short monster; // Random value
    unsigned short chest; // Boolean + Random value
    unsigned short reserve; // Boolean + Fixed value, can only appear in one room
    unsigned short boss; // Boolean + True if the room is the reserve
} room;

typedef room room_array[MAX][MAX];

typedef int array[2];

void random_room_values(room *r) {
    int result;

    result = rand() % 2 ;
    (*r).trap = (rand() % 2); // 0 or 1 to represent false or true

    result = rand() % 5;
    if (result == 0) (*r).chest = 1; // 1/5 chance of being true
    else if (result != 0) (*r).chest = 0; // 4/5 chance of being false

    result = rand() % 3;
    if (result == 0) (*r).monster = 1; 
    else if (result != 0) (*r).monster = 0;

    (*r).reserve = 0;
    (*r).boss = 0; 
}

int door_count(int row, int column) {
    int door_count_output = 0;

    if ((row == 0 && column == 0) || (row == 0 && column == 4) || (row == 4 && column == 0) || (row == 4 && column == 4))
        door_count_output = 2;
    else if ((row == 1 && column == 0) || (row == 2 && column == 0) || (row == 3 && column == 0) ||
             (row == 1 && column == 4) || (row == 2 && column == 4) || (row == 3 && column == 4))
        door_count_output = 3;
    else if ((row == 0 && column == 1) || (row == 0 && column == 2) || (row == 0 && column == 3) ||
             (row == 4 && column == 1) || (row == 4 && column == 2) || (row == 4 && column == 3))
        door_count_output = 3;
    else
        door_count_output = 4;

    return door_count_output;
}

room create_room(int row, int column) {
    room new_room;
    new_room.room_id = (row * MAX) + column;
    new_room.door_count = door_count(row, column);

    if ((row == 0 && column == 0)) {
        new_room.trap = 0;
        new_room.chest = 0;
        new_room.boss = 0;
        new_room.reserve = 0;
    }

    else {
        random_room_values(&new_room);
    }

    return new_room;
}

void create_reserve(room_array *room_array) {
    int random_number = rand() % 25;
    room (*rooms)[MAX] = room_array;

    while ((random_number == 0 / MAX != 0) && (random_number % MAX != 0)) {
        random_number = rand() % 25;
    }

    rooms[random_number / MAX][random_number / MAX % MAX].reserve = 1;
    rooms[random_number / MAX][random_number / MAX % MAX].boss = 1;
}

void create_rooms(room_array *room_array) {
    room (*rooms)[MAX] = room_array;

    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            rooms[i][j] = create_room(i, j);
        }
    }
    create_reserve(*room_array);
}

void display_room(int num_rooms) {
    int check = 0;

    for (int row = 0; row < num_rooms; row++) {
        char room_layout[5][5] = {
            {'+', '-', '-', '-', '+'},
            {'|', ' ', ' ', ' ', '|'},
            {'|', ' ', ' ', ' ', '|'},
            {'|', ' ', ' ', ' ', '|'},
            {'+', '-', '-', '-', '+'}
        };

        for (int i = 0; i < 5; ++i) {
            for (int k = 0; k < 5; ++k) {  // Repeat 5 times for each square
                if (k == 0 && row == 0) room_layout[2][2] = 'X'; //
                else room_layout[2][2] = ' '; //
                for (int j = 0; j < 5; ++j) {
                    printf("%c ", room_layout[i][j]);
                }
                printf("");  // Add space between squares
            }
            printf("\n");
        }
    }
}

void print_room(room r) {
    printf("Room %d-%d - Trap: %hu, Chest: %hu, Door count: %hu, Monster: %hu, Boss: %hu, Reserve: %hu\n",
           (r.room_id / MAX), (r.room_id % MAX), r.trap,
           r.chest, r.door_count, r.monster, r.boss, r.reserve);
}

void play_sound() {
    printf("\a");  // Emit a control character to generate a beep
}

char *get_line(const char *filename, int desired_line_number) {
    FILE *file = fopen(filename, "r");

    char line[1024];
    int current_line_number = 0;

    struct stat sb;
    if (stat(filename, &sb) != 0) {
        perror("Error retrieving file information");
        fclose(file);
        return NULL;
    }

    char *line_allocation = malloc(sb.st_size);

    while (fgets(line, sizeof line, file) != NULL) {
        current_line_number += 1;

        if (current_line_number == desired_line_number) {
            fclose(file);

            // Remove the end-of-line character if present
            size_t line_length = strlen(line);
            if (line_length > 0 && line[line_length - 1] == '\n') {
                line[line_length - 1] = '\0';
            }

            char *copied_line = malloc(strlen(line) + 1);
            if (copied_line == NULL) {
                perror("Memory allocation error");
                return NULL;
            }

            strcpy(copied_line, line);
            return copied_line;
        }
    }

    fclose(file);
}

void between_lines(int start, int end, char filename[30]) {
    // Read the content of the file "filename" between the start and end
    for (int i = start; i <= end; i++) {
        printf("> %s\n", get_line(filename, i));
        printf("\n");
        // sleep(3);
    }
}

void display_introduction() {
    printf("\n======================================================================\n\n");
    printf("| |  | ||  _  | ___ \\ |   |  _  \\ | |  | |/ _ \\ | ___ \\    /   |\n");
    printf("| |  | || | | | |_/ / |   | | | | | |  | / /_\\ \\| |_/ /   / /| |\n");
    printf("| |\\| || | | |    /| |   | | | | | |\\| |  _  ||    /   / /_| |\n");
    printf("\\  /\\  /\\ \\_/ / |\\ \\| |___| |/ /  \\  /\\  / | | || |\\ \\   \\___  |\n");
    printf(" \\/  \\/  \\___/\\_| \\_\\_____/___/    \\/  \\/\\_| |_/\\_| \\_|     |_/\n\n");
    printf("1. New Game\n");
    printf("2. Continue\n\n");
    printf("======================================================================\n\n");
    printf("Choose an option by typing the corresponding number and press Enter:\n");
}

char *generate_sentence(int type, const char *filename) {
    int line_number;
    char *sentence = malloc(1024);

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    switch (type) {
    case 1:
        line_number = rand() % (61 - 52 + 1) + 52;
        sentence = get_line(filename, line_number);
        break;

    case 2:
        line_number = rand() % (49 - 40 + 1) + 40;
        sentence = get_line(filename, line_number);
        break;

    case 3:
        line_number = rand() % (73 - 64 + 1) + 64;
        sentence = get_line(filename, line_number);
        break;

    case 4:
        line_number = 76;  // Direct use of line 76 for case 4
        sentence = get_line(filename, line_number);
        break;

    case 5:
        line_number = rand() % (88 - 79 + 1) + 79;
        sentence = get_line(filename, line_number);
        break;

    default:
        fprintf(stderr, "Invalid type\n");
        exit(EXIT_FAILURE);
    }

    fclose(file);
    return sentence;
}

void create_room_folder(char *folder_name, room *room, char *language) {
    FILE *folder = fopen(folder_name, "w");

    if (folder == NULL) {
        fprintf(stderr, "Error creating file %s\n", folder_name);
        exit(EXIT_FAILURE);
    }

    fprintf(folder, "You arrive in the room: %d\n", room->room_id);

    if (room->boss != 1) {
        if (room->trap == 1) {
            char *trap_sentence = generate_sentence(1, language);
            fprintf(folder, "%s\n", trap_sentence);
            free(trap_sentence);
        }

        if (room->monster == 1) {
            char *monster_sentence = generate_sentence(2, language);
            fprintf(folder, "%s\n", monster_sentence);
            free(monster_sentence);
        }

        if (room->chest == 1) {
            char *chest_sentence = generate_sentence(3, language);
            fprintf(folder, "%s\n", chest_sentence);
            free(chest_sentence);
        }
    } else {
        char *boss_sentence = generate_sentence(4, language);
        fprintf(folder, "%s\n", boss_sentence);
        free(boss_sentence);
    }

    if (room->trap != 1 && room->boss != 1 && room->monster != 1) {
        // Generate a sentence for empty rooms (type 5)
        char *empty_room_sentence = generate_sentence(5, language);
        fprintf(folder, "%s\n", empty_room_sentence);
        free(empty_room_sentence);
    }

    fclose(folder);
}

int main(int argc, char *argv[]) {
    int user_choice = 0, game_choice = 0;
    char sentence[100], folder[10];
    room_array rooms;
    char exit[10], language[3]; // Increase the size of 'language' to store "fr" or "en"
    const char *file = "text-en.txt"; // Default to using the English file
    FILE *in_file = fopen(file, "r");

    if (in_file == NULL) {
        printf("Error opening file %s\n", file);
        return 1; // Exit the program in case of an error
    }

    do {
        printf("Choose your language (fr) or (en): ");
        scanf("%s", language);

        if (strcmp(language, "fr") == 0 || strcmp(language, "FR") == 0) {
            fclose(in_file);
            file = "text-fr.txt";
            in_file = fopen(file, "r");
        } else if (strcmp(language, "en") == 0 || strcmp(language, "EN") == 0) {
            fclose(in_file);
            file = "text-en.txt";
            in_file = fopen(file, "r");
        }

        srand(time(NULL)); // Initialize the random number generator with the current time

        // Read the size of the file
        struct stat sb;
        stat(file, &sb);

        // Allocate the file
        char *file_allocation = malloc(sb.st_size);

        // Display menu
        while (game_choice == 0) {
            display_introduction();
            scanf("%i", &game_choice);

            switch (game_choice) {
            case 1:
                break;

            case 2:
                break;

            default:
                break;
            }
        }

        // Read introduction
        between_lines(1, 11, file);

        // Information of the first room
        between_lines(28, 29, file);
        display_room(5);
        between_lines(31, 34, file);

        free(file_allocation);

        // Example of accessing a room and displaying its attributes
        printf("Do you want to create the rooms? (1 for yes, 0 for no): ");
        scanf("%i", &user_choice);

        if (user_choice == 1) {
            create_rooms(rooms);

            for (int i = 0; i < MAX; i++) {
                for (int j = 0; j < MAX; j++) {
                    print_room(rooms[i][j]);
                }
            }
        }

        for (int i = 0; i < 25; i++) {
            snprintf(folder, sizeof(folder), "room%d", i);

            create_room_folder(folder, &rooms[i / MAX][i % MAX], "text-en.txt");
        }

        scanf("%s", exit);

    } while (strcmp(exit, "quit") != 0 && strcmp(exit, "quitter") != 0);

    fclose(in_file);

    return 0;
}