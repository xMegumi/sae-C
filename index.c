// Lorenzo Vandenkoornhuyse
// Berat Sirin 

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/select.h>
 
#define MAX 5
#define MAX_ROBOT 6
#define DELAY_LIMIT 5
 
typedef struct
{
    short room_id;             // Fixed value
    unsigned short door_count; // Fixed value
    unsigned short trap;       // Random value
    unsigned short monster;    // Random value
    unsigned short chest;      // Boolean + Random value
    unsigned short reserve;    // Boolean + Fixed value, can only appear in one room
    unsigned short boss;       // Boolean + True if the room is the reserve
    unsigned short chest_content; // Boolean + Random value
} room;
 
typedef struct
{
    int row;
    int column;
} position;
 
typedef struct
{
    char nom[10];
    char nationalite; // F : French A : american
    char race;        // R : robot H human
    int hearts;
    unsigned short weapon; // Boolean
    position player_position; // position of the player
} Player;
 
typedef room room_array[MAX][MAX];
 
typedef int array[2];
 
position position_boss;
 
// Function to generate random values for a room
void random_room_values(room *r)
{
    int result;
 
    result = rand() % 2;
    (*r).trap = (rand() % 3); // 0 or 1 to represent false or true
 
    result = rand() % 5;
    if (result == 0)
        (*r).chest = 1; // 1/5 chance of being true
    else if (result != 0)
        (*r).chest = 0; // 4/5 chance of being false
 
    result = rand() % 3;
    if (result == 0)
        (*r).monster = 1;
    else if (result != 0)
        (*r).monster = 0;
 
    if ((*r).chest == 1) (*r).chest_content = 1;
    else if ((*r).chest == 0) (*r).chest_content = 0;
 
    (*r).reserve = 0;
    (*r).boss = 0;
}
 
// Function to calculate the number of doors in a room based on its position
int door_count(int row, int column)
{
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
 
// Function to display text character by character with a delay
void display_text(char *texte)
{
    printf("\n");
    for (int i = 0; texte[i] != '\0'; i++)
    {
        printf("%c", texte[i]);
        fflush(stdout); // Pour vider le buffer et afficher immédiatement
 
        usleep(20000);
    }
    printf("\n");
}
 
// use to clear the inputs
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}
 
// Function to create a new room
room create_room(int row, int column)
{
    room new_room;
    new_room.room_id = (row * MAX) + column ;
    new_room.door_count = door_count(row, column);
 
    if ((row == 0 && column == 0))
    {
        new_room.trap = 0;
        new_room.chest = 0;
        new_room.boss = 0;
        new_room.reserve = 0;
        new_room.monster = 0;
    }
 
    else
    {
        random_room_values(&new_room);
    }
 
    return new_room;
}
 
// Function to create a reserve room with special properties
void create_reserve(room_array *room_array)
{
    int random_number = rand() % 24 + 1; 
    room(*rooms)[MAX] = room_array;
 
    rooms[random_number / MAX][random_number % MAX].reserve = 1;
    rooms[random_number / MAX][random_number % MAX].boss = 1;
    rooms[random_number / MAX][random_number % MAX].monster = 0;
 
    position_boss.row = random_number / MAX;
    position_boss.column = random_number % MAX;
}
 
// Function to generate a sword in a room with a chest
void generate_sword(room_array *rooms) {
    position room_position = {-1, -1};
 
    for (int i = 0; i < MAX ; i++) {
        for (int j = 0; j < MAX; j++) {
            if ((*rooms)[i][j].chest_content == 1 && (*rooms)[i][j].boss != 1) {
                room_position.row = i;
                room_position.column = j;
            }
        }
    }
 
 
        (*rooms)[room_position.row][room_position.column].chest_content = 2;
        (*rooms)[room_position.row][room_position.column].monster = 1;
}
 
// Function to create all rooms in the game
void create_rooms(room_array *room_array)
{
    room(*rooms)[MAX] = room_array;
 
    for (int i = 0; i < MAX; i++)
    {
        for (int j = 0; j < MAX; j++)
        {
            rooms[i][j] = create_room(i, j);
        }
    }
 
    create_reserve(*room_array);
    generate_sword(*room_array);
}
 
// Function to display the layout of a room
void display_room( int room_row, int room_column)
{
    for (int row = 0; row < MAX; row++)
    {
        char room_layout[5][5] = {
            {'+', '-', '-', '-', '+'},
            {'|', ' ', ' ', ' ', '|'},
            {'|', ' ', ' ', ' ', '|'},
            {'|', ' ', ' ', ' ', '|'},
            {'+', '-', '-', '-', '+'}};
 
        for (int i = 0; i < 5; ++i)
        {
            for (int k = 0; k < 5; ++k)
            { // Repeat 5 times for each square
                if (k == room_column && row == room_row)
                    room_layout[2][2] = 'X'; //
                else
                    room_layout[2][2] = ' '; //
                for (int j = 0; j < 5; ++j)
                {
                    printf("%c ", room_layout[i][j]);
                }
                printf(""); // Add space between squares
            }
            printf("\n");
        }
    }
}
 
// Function to display player information
void show_player(Player player)
{
    printf("\nnationality : %c, name : %s, race : %c, heart : %i, weapon : , position : %i - %i \n",
           player.nationalite, player.nom, player.race, player.hearts, player.weapon,
           player.player_position.row, player.player_position.column);
}
 
// Function to display the player's health bar
void show_health_bar(int hearts, Player *player) 
{
 
    int filled_length = (int)(((float)hearts / MAX));
    printf("\n\033[1;31m["); // Couleur rouge pour la barre de vie
 
    if(player->race == 'h')
    {
        for (int i = 0; i < MAX; ++i) {
            if (i < hearts) {
                printf("\xe2\x99\xa5"); // Code ASCII heart
            } else {
                printf("-");
        }
    }
    }
 
    else 
    {
        for (int i = 0; i < MAX_ROBOT; ++i) {
            if (i < hearts) {
                printf("\xe2\x99\xa5"); // Code ASCII heart
            } else {
                printf("-");
        }
    }
 
    }
    printf("]\033[0m\n"); // Réinitialisation de la couleur
}
 
// Function to print details of a room
void print_room(room r)
{
    printf("Room %d / %d-%d - Trap: %hu, Chest: %hu, chest content : %hu, Door count: %hu, Monster: %hu, Boss: %hu, Reserve: %hu\n",
           r.room_id, (r.room_id / MAX), (r.room_id % MAX), r.trap,
           r.chest, r.chest_content, r.door_count, r.monster, r.boss, r.reserve);
}
 
// Function to read a specific line from a file
char *get_line(const char *filename, int desired_line_number)
{
    FILE *file = fopen(filename, "r");
 
    char line[1024];
    int current_line_number = 0;
 
    struct stat sb;
    if (stat(filename, &sb) != 0)
    {
        perror("Error retrieving file information");
        fclose(file);
        return NULL;
    }
 
    char *line_allocation = malloc(sb.st_size);
 
    while (fgets(line, sizeof line, file) != NULL)
    {
        current_line_number += 1;
 
        if (current_line_number == desired_line_number)
        {
            fclose(file);
 
            // Remove the end-of-line character if present
            size_t line_length = strlen(line);
            if (line_length > 0 && line[line_length - 1] == '\n')
            {
                line[line_length - 1] = '\0';
            }
 
            char *copied_line = malloc(strlen(line) + 1);
            if (copied_line == NULL)
            {
                perror("Memory allocation error");
                return NULL;
            }
 
            strcpy(copied_line, line);
            return copied_line;
        }
    }
 
    fclose(file);
}
 
// Function to display content from specific lines in a file
int between_lines(int start, int end, char filename[30])
{
    for (int i = start; i <= end; i++)
    {
        display_text(get_line(filename, i));
    }
}
 
// Function to display the game introduction in a selected language
void display_introduction(char *language)
{
    if (strcmp(language, "en") == 0|| strcmp(language, "EN") == 0) {
    printf("======================================================================\n\n");
    printf("\033[0;33m\n");
    printf(" (  (     ( /(  )\\ ))\\ ))\\ )    (  (      (     )\\ )       ) \n"); 
    printf(" )\\))(   ')\\())(()/(()/(()/(    )\\))(   ' )\\   (()/(    ( /( \n"); 
    printf("((_)()\\ )((_)\\  /(_))(_))(_))  ((_)()\\ |(((_)(  /(_))   )\\())\n"); 
    printf("_(())\\_)() ((_)(_))(_))(_))_   _(())\\_)()\\_ )\\(_))    ((_)\\ \033[0m\n");                                        
    printf("\\ \\((_)/ // _ \\| _ \\ |  |  \\ \\ \\((_)/ (_)_\\(_)_ \\  | | (_)\n"); 
    printf(" \\ \\/\\ //| (_) |   / |__| |) |  \\ \\/\\/ / / _ \\|   /  |_  _| \n"); 
    printf("  \\_/\\_// \\___/|_|_\\____|___/   \\_/\\_//_/ \\_\\|_|_\\    |_|  \n\n"); 
    printf("1. New Game\n");    
    printf("2. Continue\n");
    printf("3. Quit\n\n");
    printf("======================================================================\n\n");
    printf("Choose an option by typing the corresponding number and press Enter:\n");
    }
 
    else if (strcmp(language, "fr") == 0 || strcmp(language, "FR") == 0) 
    {printf("======================================================================\n\n");
    printf("\033[0;33m\n");
    printf(" (  (     ( /(  )\\ ))\\ ))\\ )    (  (      (     )\\ )       ) \n"); 
    printf(" )\\))(   ')\\())(()/(()/(()/(    )\\))(   ' )\\   (()/(    ( /( \n"); 
    printf("((_)()\\ )((_)\\  /(_))(_))(_))  ((_)()\\ |(((_)(  /(_))   )\\())\n"); 
    printf("_(())\\_)() ((_)(_))(_))(_))_   _(())\\_)()\\_ )\\(_))    ((_)\\ \033[0m\n");                                        
    printf("\\ \\((_)/ // _ \\| _ \\ |  |  \\ \\ \\((_)/ (_)_\\(_)_ \\  | | (_)\n"); 
    printf(" \\ \\/\\ //| (_) |   / |__| |) |  \\ \\/\\/ / / _ \\|   /  |_  _| \n"); 
    printf("  \\_/\\_// \\___/|_|_\\____|___/   \\_/\\_//_/ \\_\\|_|_\\    |_|  \n\n"); 
    printf("1. Nouvelle partie\n");    
    printf("2. Continuer\n");
    printf("3. Quitter\n\n");
    printf("======================================================================\n\n");
    printf("Choisir une option en tapant le numéro correspondant et appuyer sur Entrée :\n");
 
    }
}
 
// Function to generate a sentence from a file based on a specified type
char *generate_sentence(int type, const char *filename)
{
    int line_number;
    char *sentence = malloc(1024);
 
    FILE *file = fopen(filename, "r");
 
    if (file == NULL)
    {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(EXIT_FAILURE);
    }
 
    switch (type)
    {
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
        line_number = rand() % (85 - 76 + 1) + 76; 
        sentence = get_line(filename, line_number);
        break;
 
    case 5:
        line_number = rand() % (97 - 88 + 1) + 88;
        sentence = get_line(filename, line_number);
        break;
 
    default:
        fprintf(stderr, "Invalid type\n");
        exit(EXIT_FAILURE);
    }
 
    fclose(file);
    return sentence;
}
 
// Function to create a file with specific content based on room properties
void create_room_folder(char *folder_name, room *room, char *language)
{
    FILE *folder = fopen(folder_name, "w");
 
    if (folder == NULL)
    {
        fprintf(stderr, "Error creating file %s\n", folder_name);
        exit(EXIT_FAILURE);
    }
 
    fprintf(folder,"%s %d\n", get_line(language, 131), room->room_id);
 
 
    if (room->boss != 1)
    {
        if (room->trap == 1 )
        {
            char *trap_sentence = generate_sentence(1, language);
            fprintf(folder, "%s\n", trap_sentence);
            free(trap_sentence);
        }
 
        if (room->monster == 1)
        {
            char *monster_sentence = generate_sentence(2, language);
            fprintf(folder, "%s\n", monster_sentence);
            free(monster_sentence);
        }
 
        if (room->chest == 1)
        {
            char *chest_sentence = generate_sentence(3, language);
            fprintf(folder, "%s\n", chest_sentence);
            free(chest_sentence);
        }
    }
    else
    {
        char *boss_sentence = generate_sentence(4, language);
        fprintf(folder, "%s\n", boss_sentence);
        free(boss_sentence);
    }
 
    if (room->trap != 1 && room->boss != 1 && room->monster != 1 && room->chest != 1)
    {
        // Generate a sentence for empty rooms (type 5)
        char *empty_room_sentence = generate_sentence(5, language);
        fprintf(folder, "%s\n", empty_room_sentence);
        free(empty_room_sentence);
    }
 
    fclose(folder);
}
 
// Function to create a position object with given row and column
position create_position(int row, int column)
{
    position *position_p = malloc(sizeof(position));
 
    // Vérifier si l'allocation de mémoire a réussi
    if (position_p == NULL)
    {
        fprintf(stderr, "Erreur lors de l'allocation de mémoire.\n");
        exit(EXIT_FAILURE);
    }
 
    // Remplir les champs de la structure avec les valeurs fournies
    position_p->row = row;
    position_p->column = column;
 
    // Retourner un pointeur vers la nouvelle structure position
    return *position_p;
}
 
// use to save the player
void save_player(Player joueur)
{
 
    FILE *fichier = fopen("save.txt", "w");
 
    if (fichier != NULL)
    {
 
        fprintf(fichier, "Nom: %s\n", joueur.nom);
        fprintf(fichier, "Nationalite: %c\n", joueur.nationalite);
        fprintf(fichier, "Race: %c\n", joueur.race);
        fprintf(fichier, "Hearts: %d\n", joueur.hearts);
        fprintf(fichier, "Weapon: %hu\n", joueur.weapon);
        fprintf(fichier, "Position: (%d, %d)\n", joueur.player_position.row, joueur.player_position.column);
 
 
        fclose(fichier);
    }
}
 
// use to load the player
Player load_player()
{
    Player joueur; 
 
 
    FILE *fichier = fopen("save.txt", "r");
 
    if (fichier != NULL)
    {
        fscanf(fichier, "Nom: %s\n", joueur.nom);
        fscanf(fichier, "Nationalite: %c\n", &joueur.nationalite);
        fscanf(fichier, "Race: %c\n", &joueur.race);
        fscanf(fichier, "Hearts: %d\n", &joueur.hearts);
        fscanf(fichier, "Weapon: %hu\n", &joueur.weapon);
        fscanf(fichier, "Position: (%d, %d)\n", &joueur.player_position.row, &joueur.player_position.column);
 
 
        fclose(fichier);
 
        return joueur;
    }
}
 
// use to save the room
void save_room(room_array rooms) {
    FILE *file = fopen("rooms.dat", "wb"); 
 
    if (file != NULL) {
        fwrite(rooms, sizeof(room), MAX * MAX, file); 
        fclose(file); 
    } else {
        printf("Erreur lors de l'ouverture du fichier.\n");
    }
}
 
// use to load the rooms
void load_room(room_array rooms) {
    FILE *file = fopen("rooms.dat", "rb"); 
 
    if (file != NULL) {
        fread(rooms, sizeof(room), MAX * MAX, file); 
        fclose(file); 
    } else {
        printf("Erreur lors de l'ouverture du fichier.\n");
    }
}
 
// Function to get player's input for room direction
char direct_room(Player player, const char *langue[11], room_array rooms) 
{
    char direct;
    int id_room = (player.player_position.row * MAX) + player.player_position.column;
    int valid_input = 0;
    int scan_result;
 
    while (!valid_input) {
 
 
        switch (id_room)
        {
        case 0:
            printf("%s : ", get_line(langue, 100));
            break;
 
        case 4:
            printf("%s : ", get_line(langue, 101));
            break;
 
        case 20:
            printf("%s : ", get_line(langue, 102));
            break;
 
        case 24:
            printf("%s : ", get_line(langue, 103));
            break;
 
        case 1:
        case 2:
        case 3:
            printf("%s : ", get_line(langue, 106));
            break;
 
        case 5:
        case 10:
        case 15:
            printf("%s : ", get_line(langue, 104));
            break;
 
        case 21:
        case 22:
        case 23:
            printf("%s : ", get_line(langue, 107));
            break;
 
        case 9:
        case 14:
        case 19:
            printf("%s : ", get_line(langue, 105));
            break;
 
        case 6:
        case 7:
        case 8:
        case 11:
        case 12:
        case 13:
        case 16:
        case 17:
        case 18:
            printf("%s : ", get_line(langue, 108));
            break;
 
        default:
            printf("%s : ", get_line(langue, 109));
            break;
        }
 
        scan_result = scanf(" %c", &direct);
        clear_input_buffer();
 
 
 
        if (scan_result == 1) {
            if (direct == 'q' || direct == 'Q') {
                save_player(player);
                save_room(rooms);
                printf("Saving progress: ");
 
                printf("[");
                for (int progress = 0; progress <= 100; progress += 10) {
                    printf("#");
                    fflush(stdout); 
                    usleep(200000); 
                }
                printf("]\n\n");
                exit(0); 
            } else {
                valid_input = 1; 
            }
        }
    }
 
    return direct;
 
}
 
// Function to change player's position based on direction input
void change_room(Player *player, const char *language, room_array rooms) {
    Player player1 = *player;
 
    char direction = direct_room(player1, language, rooms);
 
    if (strcmp(language, "text-fr.txt") == 0) {
        if (direction == 'D' || direction == 'd') player->player_position.column += 1;
        if (direction == 'B' || direction == 'b') player->player_position.row += 1;
        if (direction == 'H' || direction == 'h') player->player_position.row -= 1;
        if (direction == 'G' || direction == 'g') player->player_position.column -= 1;
    } else if (strcmp(language, "text-en.txt") == 0) {
        if (direction == 'D' || direction == 'd') player->player_position.row += 1;
        if (direction == 'R' || direction == 'r') player->player_position.column += 1;
        if (direction == 'U' || direction == 'u') player->player_position.row -= 1;
        if (direction == 'L' || direction == 'l') player->player_position.column -= 1;
    }
 
    if (player->player_position.column > 4) {
        player->player_position.column = 4;
    } else if (player->player_position.column < 0) {
        player->player_position.column = 0;
    }
 
    if (player->player_position.row > 4) {
        player->player_position.row = 4;
    } else if (player->player_position.row < 0) {
        player->player_position.row = 0;
    }
 
 
    display_room(player->player_position.row, player->player_position.column);
}
 
// Function to handle game over scenario
void game_over(char language[11]) {
    printf("\n===========================\n");
    printf("%s\n", get_line(language, 111));
    printf("=============================\n");
}
 
// Function to check if game over conditions are met
int check_game_over(Player *player) {
    int output = 0;
    if (player->hearts == 0) {
        output = 1; 
    }
 
    return output; 
}
 
// Function to remove a heart from player (health decrement)
void remove_heart(Player *player, const char language[11]) {
 
    if (player->hearts > 0) {
        player->hearts -= 1;
        if (player->hearts == 1) {
            printf("%s\n", get_line(language, 113)); 
        }
    }
 
    if (player->hearts == 0) {
        game_over(language); 
    }
}
 
// Function to add a heart to player (health increment)
void add_heart(Player *player, const char language[11])
{
    if(player->race == 'h')
    {
 
    if (player->hearts < MAX)
    {
        player->hearts += 2;
        if (player->hearts == MAX)
        {
            printf("%s\n", get_line(language, 119)); // Show the MAX LIFE message
        }
    }
    }
 
    if(player->race == 'r')
    {
 
    if (player->hearts < MAX_ROBOT)
    {
        player->hearts += 1;
        if (player->hearts == MAX_ROBOT)
        {
            printf("%s\n", get_line(language, 119)); // Show the MAX LIFE message
        }
    }
    }
 
}
 
 
 
// Function to handle monster encounters in rooms
int fight_monster(Player *player, room *room, char *language)
{   
    int choice_monster;
    int prob_attack, output=1;
    FILE *folder = fopen(language, "r");
 
 
    if(room->boss != 1)
    {
        if(room->monster == 1)
        {
            while(room->monster != 0 && !check_game_over(player))
            {
 
 
            do {
                printf("%s\n", get_line(language, 124));
                printf("%s\n", get_line(language, 125));
                printf("%s\n", get_line(language, 126));
                printf("%s\n", get_line(language, 127));
                scanf("%i", &choice_monster);
            }while (choice_monster != 1 && choice_monster != 2);
 
            if(choice_monster == 2)
            {
                output = 0;
                break;
            }
 
            if(choice_monster == 1 )
            {
                srand(time(NULL));
                prob_attack = rand() % 2;
 
                if (prob_attack == 0)
                {
                    printf("%s", get_line(language, 128));
                    room->monster = 0;
                }
 
                else
                {
                    remove_heart(player, language);
                    if (player->hearts == 0) break;
                    printf("%s", get_line(language, 129));
                    show_health_bar(player->hearts, player);
                }
            }
            }
            return output;
            fclose(folder);            
        }
    }
}
 
// Function to handle boss encounters in rooms
int fight_boss(Player *player, room *room, char language[11]) {
    int boss_hearts = 2;
    int choice_monster;
    int prob_attack;
    FILE *folder = fopen(language, "r");
 
    if (room->boss == 1) {
        while (room->boss != 0 && !check_game_over(player)) {
            show_health_bar(player->hearts, player);
 
            printf("%s\n", get_line(language, 125));
            printf("%s\n", get_line(language, 126));
            printf("%s\n", get_line(language, 127));
            scanf("%i", &choice_monster);
            getchar();
 
            if (choice_monster == 2) {
                break;
            }
 
            if (choice_monster == 1) {
                if (player->weapon == 1) {
                    prob_attack = rand() % 2;
                    if (prob_attack == 0) {
                        if (boss_hearts == 0) {
                            printf("%s\n", get_line(language, 139));
                            room->boss = 0;
                        } else {
                            printf("%s\n", get_line(language, 138));
                            boss_hearts -= 1;
                        }
                    } else {
                        printf("%s\n", get_line(language, 140));
                        remove_heart(player, language);
                    }
                } else {
                    printf("%s\n", get_line(language, 147));
                    printf("%s\n", get_line(language, 140));
                    remove_heart(player, language);
                }
            }
        }
    }
 
    fclose(folder);
}
 
// Function to handle chest content interactions in rooms
void chest_content(Player *player, room *room, char *language)
{
    int choice;
 
 
    FILE *folder = fopen(language, "r"); 
 
    if (folder == NULL) {
        fprintf(stderr, "Error opening file %s\n", language[0]); 
        exit(EXIT_FAILURE);
    }
 
    if (room->chest != 0)
    {
        printf("%s\n", get_line(language, 134));
        printf("%s\n", get_line(language, 135));
        printf("%s\n", get_line(language, 136));
        scanf("%d", &choice);
 
        if (choice == 1) 
        {
            if (room->chest_content == 1)
            {
                printf("%s\n", get_line(language, 118)); 
                add_heart(player, language); 
            }
            else
            {
                printf("%s\n", get_line(language, 117)); 
                player->weapon = 1; 
            }
            room->chest = 0;
        }
    }
 
    fclose(folder);
}
 
 
 
// used to be able to set a time when typing the word for the trap
int attendreSaisie(const char *filename) {
    int output = 0;
 
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
 
    struct timeval tv;
    tv.tv_sec = DELAY_LIMIT;
    tv.tv_usec = 0;
 
    int ret = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
 
    if (ret == -1) {
        perror("select");
        output = -1;
    } else if (ret == 0) {
        display_text(get_line(filename, 151));
        output = 0;
    } else {
        output = 1;
    }
 
    return output;
}
 
// generate a word for the trap
char *generate_word_trap(const char *filename) {
    int line_number;
    char *sentence = malloc(1024 * sizeof(char)); // Allocation de mémoire pour la phrase
 
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(EXIT_FAILURE);
    }
 
    line_number = rand() % (168 - 154 + 1) + 154;
    char *line = get_line(filename, line_number);
    strcpy(sentence, line); 
    free(line); 
 
    fclose(file);
    return sentence;
}
 
 
// Allows to launch the mechanics of the trap
int trap_game(const char *filename) {
    char *word = generate_word_trap(filename);
    display_text(get_line(filename, 150));
    printf("%s\n", word);
    fflush(stdout);
 
    int saisieAttendue = attendreSaisie(filename);
    int output = 0;
 
    if (saisieAttendue == 1) {
        char valeurSaisie[1024];
 
        if (scanf("%s", valeurSaisie) != 1) {
            display_text(get_line(filename, 153));
            output = 1;
        } else {
 
            char normalizedValeurSaisie[1024];
            char normalizedWord[1024];
 
            int i = 0, j = 0;
            while (valeurSaisie[i]) {
                if (isalnum(valeurSaisie[i])) {
                    normalizedValeurSaisie[j++] = tolower(valeurSaisie[i]);
                }
                i++;
            }
            normalizedValeurSaisie[j] = '\0';
 
            i = 0, j = 0;
            while (word[i]) {
                if (isalnum(word[i])) {
                    normalizedWord[j++] = tolower(word[i]);
                }
                i++;
            }
            normalizedWord[j] = '\0';
 
            i = 0;
            while (normalizedValeurSaisie[i] && normalizedWord[i]) {
                if (normalizedValeurSaisie[i] != normalizedWord[i]) {
                    display_text(get_line(filename, 153));
                    output = 1;
                    break;
                }
                i++;
            }
 
            if (!output && normalizedValeurSaisie[i] != normalizedWord[i]) {
                display_text(get_line(filename, 153));
                output = 1;
            } else {
                if (normalizedValeurSaisie[i] == '\0' && normalizedWord[i] == '\0') {
                display_text(get_line(filename, 152));
                }
            }
        }
    } else {
        output = 1;
    }
 
    free(word);
    putchar('\n');
    return output;
}
 
// Function to handle room protocol including various interactions
void protocol_room(char *folder_name, room *room, Player *player , char *language)
{
    int trap_win;
    int temporary_monster = 0, temporary_trap = 0;
    int choice_monster = 1;
    FILE *folder = fopen(folder_name, "r");
 
    if (folder == NULL)
    {
        fprintf(stderr, "Error creating file %s\n", folder_name);
        exit(EXIT_FAILURE);
    }
 
    display_text(get_line(folder_name, 1));
 
    if (room->boss != 1)
    {
        if (room->trap == 1 && check_game_over(player) == 0)
        {
 
            display_text(get_line(folder_name, 2));
            display_text(get_line(language, 130));
            trap_win = trap_game(language);
            if (trap_win == 1) remove_heart(player, language);
            show_health_bar(player->hearts, player);
            sleep(1);
            room->trap = 0;
            temporary_trap = 1;
        }
 
        if (room->monster == 1 && check_game_over(player) == 0) 
        {
            if (temporary_trap == 1) display_text(get_line(folder_name, 3));
            else display_text(get_line(folder_name, 2));
            choice_monster = fight_monster(player, room, language);
            temporary_monster = 1;
            show_health_bar(player->hearts, player);
            sleep(1);
        }
 
        if (room->chest == 1 && check_game_over(player) == 0 && choice_monster != 0)
        {
 
            if (temporary_trap == 1 && temporary_monster == 1) printf("%s \n", get_line(folder_name, 4));
            if (temporary_trap == 1 && temporary_monster == 0 || temporary_trap == 0 && temporary_monster == 1) printf("%s \n", get_line(folder_name, 3));
            if (temporary_trap == 0 && temporary_monster == 0) printf("%s \n", get_line(folder_name, 2));
 
            chest_content(player, room, language);
            show_health_bar(player->hearts, player);
            sleep(1);
        }
    }
    else
    {
        display_text(get_line(folder_name, 2));
        fight_boss(player, room, language);
    }
 
    if (temporary_trap != 1 && room->boss != 1 && room->monster != 1 && room->chest != 1)
    {
        display_text(get_line(language, 88));
    }
 
 
 
    fclose(folder);
}
 
// Function to create a player and initialize properties
void create_player(Player *new_player, const char *file)
{
    FILE *in_file = fopen(file, "r");
 
    if (in_file == NULL)
    {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier.\n");
        return;
    }
 
    display_text(get_line(file, 14));
    scanf("%9s", new_player->nom); // Limiting input to 9 characters for safety
    printf("\n%s %s \n\n", get_line(file, 16), new_player->nom);
 
while (new_player->nationalite != 'f' && new_player->nationalite != 'a')
{
    display_text(get_line(file, 19));
    scanf(" %c", &new_player->nationalite);
 
    // Nettoyer le tampon d'entrée
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
 
while (new_player->race != 'h' && new_player->race != 'r')
{
    display_text(get_line(file, 21));
    scanf(" %c", &new_player->race);
 
    // Nettoyer le tampon d'entrée
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
 
    if (new_player->race == 'h' || new_player->race == 'H')
    {
        new_player->hearts = MAX;
        display_text(get_line(file, 24));
 
    }
 
    if (new_player->race == 'r' || new_player->race == 'R')
    {
        new_player->hearts = MAX_ROBOT;
        display_text(get_line(file, 26));
    }
    new_player->player_position = create_position(0, 0);
    new_player->weapon = 0;
 
    fclose(in_file);
}
 
// Function to determine the language file based on input
char *get_language(const char nom[3])
{
    char *languages = malloc(30);
 
    if (strcmp(nom, "fr") == 0 || strcmp(nom, "FR") == 0)
    {
        strcpy(languages, "text-fr.txt");
    }
    else if (strcmp(nom, "en") == 0 || strcmp(nom, "EN") == 0)
    {
        strcpy(languages, "text-en.txt");
    }
    else
    {
        // Langue par défaut si aucune correspondance n'est trouvée
        strcpy(languages, "text-en.txt");
    }
 
    return languages;
}
 
// use to have the position of the boss
position find_boss_position(room_array rooms) {
    position boss_position;
 
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            if (rooms[i][j].boss == 1) {
                boss_position.row = i;
                boss_position.column = j;
                return boss_position;
            }
        }
    }
 
 
    boss_position.row = -1;
    boss_position.column = -1;
    return boss_position;
}
 
 
// Function to play the game
int play_game(char lang[3]) 
{
    int user_choice = 0, game_choice = 0, intro_choice = 0;
    char sentence[100], folder[10];
    room_array rooms;
    Player new_player;
    char exit[10], direct[1], room_name[10]; 
    const char *file = "text-en.txt";  
    FILE *in_file = fopen(file, "r");
 
    srand(time(NULL)); 
 
    do {
 
        if (strcmp(lang, "fr") == 0 || strcmp(lang, "FR") == 0) {
            file = "text-fr.txt";
        } else if (strcmp(lang, "en") == 0 || strcmp(lang, "EN") == 0) {
            file = "text-en.txt";
        }
 
        in_file = fopen(file, "r");
        if (in_file == NULL) {
            printf("Error opening file %s\n", file);
            return 0;
        }
 
        struct stat sb;
        stat(file, &sb);
        char *file_allocation = malloc(sb.st_size);
 
 
        while (1)
        {
            do {
            display_introduction(lang);
 
            if (scanf("%i", &game_choice) != 1) {
 
                while (getchar() != '\n'); 
            }
 
        } while (game_choice != 1 && game_choice != 2 && game_choice != 3);
 
            switch (game_choice)
            {
            case 1:
                create_player(&new_player, file);
                create_rooms(rooms);
 
                for (int i = 0; i < 25; i++) {
            snprintf(folder, sizeof(folder), "room%d", i);
            create_room_folder(folder, &rooms[i / MAX][i % MAX], get_language(lang));
        }
 
 
        while(1) {
            printf("%s\n", get_line(get_language(lang), 143));
            printf("%s\n", get_line(get_language(lang), 144));
            printf("%s\n", get_line(get_language(lang), 145));
            scanf("%i", &user_choice);
 
            switch (user_choice) {
 
            case 1:
                break;
 
            default:
                between_lines(1, 11, get_language(lang));
 
                // Information of the first room
                between_lines(28, 29, get_language(lang));
                display_room(new_player.player_position.row, new_player.player_position.column);
                between_lines(31, 34, get_language(lang));
 
                free(file_allocation);
                break;
            }
            break;
 
        }
 
        while (!check_game_over(&new_player) && rooms[position_boss.row][position_boss.column].boss == 1) {
            change_room(&new_player, get_language(lang), rooms);
            sprintf(room_name, "room%d", (new_player.player_position.row * MAX) + new_player.player_position.column );
            protocol_room(&room_name, &rooms[new_player.player_position.row][new_player.player_position.column], &new_player, get_language(lang));
        }
 
        free(file_allocation);
 
 
 
                break;
 
            case 2:
                load_room(rooms);
                new_player = load_player();
                position_boss = find_boss_position(rooms);
                display_room(new_player.player_position.row, new_player.player_position.column);
 
                while (!check_game_over(&new_player) && rooms[position_boss.row][position_boss.column].boss == 1) {
                    change_room(&new_player, get_language(lang), rooms);
                    sprintf(room_name, "room%d", (new_player.player_position.row * MAX) + new_player.player_position.column);
                    protocol_room(&room_name, &rooms[new_player.player_position.row][new_player.player_position.column], &new_player, get_language(lang));
                }
                break;
 
            case 3:
                return 0;
 
            default:
                break;
            }
 
 
        }
 
        scanf("%s", exit); 
 
    } while (strcmp(exit, "quit") != 0 && strcmp(exit, "quitter") != 0);
 
    fclose(in_file);
 
}
 
int main(int argc, char *argv[])
{
    char exit[10], lang[3];
 
    do {
        while(strcmp(lang, "fr") != 0 && strcmp(lang, "FR") != 0 && strcmp(lang, "en") != 0 && strcmp(lang, "EN") != 0)
        {
        printf("Choose your language (fr) or (en): "); 
        scanf("%s", lang);
 
        if (strcmp(lang, "fr") != 0 && strcmp(lang, "FR") != 0 && strcmp(lang, "en") != 0 && strcmp(lang, "EN") != 0)
        {
            printf("Error of typing\n");
        }
 
        }
 
        play_game(lang);    
 
        printf("%s\n", get_line(get_language(lang), 114));
        scanf("%9s", exit);
 
    } while (strcmp(exit, "quit") != 0 && strcmp(exit, "quitter") != 0);
 
    return 0;
}
