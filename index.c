#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define MAX 5

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

void show_player(Player player)
{
    printf("\nnationality : %c, name : %s, race : %c, heart : %i, weapon : , position : %i - %i \n",
           player.nationalite, player.nom, player.race, player.hearts, player.weapon,
           player.player_position.row, player.player_position.column);
}

void show_health_bar(int hearts) {


    int filled_length = (int)(((float)hearts / MAX));
    printf("\n\033[1;31m["); // Couleur rouge pour la barre de vie

    for (int i = 0; i < MAX; ++i) {
        if (i < hearts) {
           printf("\xe2\x99\xa5"); // Code ASCII heart
        } else {
            printf("-");
        }
    }

    printf("]\033[0m\n"); // Réinitialisation de la couleur
}

void print_room(room r)
{
    printf("Room %d / %d-%d - Trap: %hu, Chest: %hu, chest content : %hu, Door count: %hu, Monster: %hu, Boss: %hu, Reserve: %hu\n",
           r.room_id, (r.room_id / MAX), (r.room_id % MAX), r.trap,
           r.chest, r.chest_content, r.door_count, r.monster, r.boss, r.reserve);
}

void play_sound()
{
    printf("\a"); // Emit a control character to generate a beep
}

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

int between_lines(int start, int end, char filename[30])
{
    // Read the content of the file "filename" between the start and end
    for (int i = start; i <= end; i++)
    {
        display_text(get_line(filename, i));
        // sleep(1);
    }
}

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
    printf("Choisir une option en tapant le numéro correspondant et appuyer sur Entrée :\n");

    }
}

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

char direct_room(Player player, const char *langue[11])
{
    char direct;
    int id_room = (player.player_position.row * MAX) + player.player_position.column;
    int valid_input = 0;

    while (!valid_input )
    {
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


        int scan_result = scanf(" %c", &direct);

        if (scan_result == 1)
        {
            valid_input = 1; // Mettre à jour le drapeau pour sortir de la boucle
        }
        else
        {
            printf("Invalid input. Please enter a valid direction.\n");
            // Nettoyer le buffer pour éviter les boucles infinies en cas d'entrée incorrecte
            while (getchar() != '\n')
                ;
        }
    }
    return direct;
}

void change_room(Player *player, const char *language) {
    Player player1 = *player;

    char direction = direct_room(player1, language);

    // Mise à jour de la position du joueur en fonction de la direction
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


    // Affichage de la nouvelle position du joueur
    display_room(player->player_position.row, player->player_position.column);
}

void game_over(char language[11]) {
    printf("\n========================================\n");
    printf("%s\n", get_line(language, 111));
    printf("========================================\n");
}

int check_game_over(Player *player) {
    int output = 0;
    if (player->hearts == 0) {
        output = 1; // Game Over
    }

    return output; // Pas de Game Over
}

void remove_heart(Player *player, const char language[11]) {
    
    if (player->hearts > 0) {
        player->hearts -= 1;
        if (player->hearts == 1) {
            printf("%s\n", get_line(language, 113)); // Afficher le message pour une seule vie restante
        }
    }
    
    if (player->hearts == 0) {
        game_over(language); // Appeler la fonction game_over si le joueur n'a plus de vies
    }
}

void add_heart(Player *player, const char language[11]) {
    if (player->hearts < MAX) {
        player->hearts += 1;
        if (player->hearts == MAX) {
            printf("%s\n", get_line(language, 119)); // Show the MAX LIFE message
        }
    }
}




void fight_monster(Player *player, room *room, char *language)
{   

    int choice_monster;
    int prob_attack;
    FILE *folder = fopen(language, "r");
    

    if(room->boss != 1)
    {
        if(room->monster == 1)
        {
            while(room->monster != 0 && !check_game_over(player))
            {

            
            // If there is a boss in the room display
            printf("%s\n", get_line(language, 124));
            printf("%s\n", get_line(language, 125));
            printf("%s\n", get_line(language, 126));
            printf("%s\n", get_line(language, 127));
            scanf("%i", &choice_monster);
            getchar();

            // If choice of player is to run
            if(choice_monster == 2)
            {
                break;
            }

            // If choice of player is to fight
            if(choice_monster == 1 )
            {
                srand(time(NULL));
                prob_attack = rand() % 3;
                
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
                    show_health_bar(player->hearts);
                }
            }
            }

            fclose(folder);            
        }
    }
}

void fight_boss(Player *player, room *room, char language[11]) {
    int boss_hearts = 2;
    int choice_monster;
    int prob_attack;
    FILE *folder = fopen(language, "r");

    if (room->boss == 1) {
        while (room->boss != 0 && !check_game_over(player)) {
            // Si un boss est présent dans la pièce, affichage
            show_health_bar(player->hearts);

            printf("%s\n", get_line(language, 125));
            printf("%s\n", get_line(language, 126));
            printf("%s\n", get_line(language, 127));
            scanf("%i", &choice_monster);
            getchar();

            // Si le choix du joueur est de fuir
            if (choice_monster == 2) {
                break;
            }

            // Si le choix du joueur est de combattre
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
                }
            }
        }
    }
    fclose(folder);
}


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
                printf("%s\n", get_line(language, 118)); // Displaying the line that he got a heart
                add_heart(player, language); // Adding one heart to player
            }
            else
            {
                printf("%s\n", get_line(language, 117)); // Displaying the line that he got a gun
                player->weapon = 1; // Adding one gun to player gun slot
            }
            room->chest = 0;
        }
    }

    fclose(folder);
}

void protocol_room(char *folder_name, room *room, Player *player , char *language)
{
    int temporary_monster = 0;
    FILE *folder = fopen(folder_name, "r");

    if (folder == NULL)
    {
        fprintf(stderr, "Error creating file %s\n", folder_name);
        exit(EXIT_FAILURE);
    }

    display_text(get_line(folder_name, 1));

    if (room->boss != 1)
    {
        if (room->trap == 1 && check_game_over(&player) == 0)
        {
            
            display_text(get_line(folder_name, 2));
            display_text(get_line(language, 130));
            remove_heart(player, language);
            show_health_bar(player->hearts);
            sleep(1);
        }

        if (room->monster == 1 && check_game_over(&player) == 0) 
        {
            if (room->trap == 1) display_text(get_line(folder_name, 3));
            else display_text(get_line(folder_name, 2));
            fight_monster(player, room, language);
            temporary_monster = 1;
            show_health_bar(player->hearts);
            sleep(1);
        }

        if (room->chest == 1 && check_game_over(&player) == 0)
        {

            if (room->trap == 1 && temporary_monster == 1) printf("%s \n", get_line(folder_name, 4));
            if (room->trap == 1 && temporary_monster == 0 || room->trap == 0 && temporary_monster == 1) printf("%s \n", get_line(folder_name, 3));
            if (room->trap == 0 && temporary_monster == 0) printf("%s \n", get_line(folder_name, 2));

            chest_content(player, room, language);
            show_health_bar(player->hearts);
            sleep(1);
        }
    }
    else
    {
        display_text(get_line(folder_name, 2));
        fight_boss(player, room, language);
    }

    if (room->trap != 1 && room->boss != 1 && room->monster != 1 && room->chest != 1)
    {
        // display the empty room message
        display_text(get_line(language, 88));
    }

    room->trap = 0;

    fclose(folder);
}

void create_player(Player *new_player, const char *file)
{
    FILE *in_file = fopen(file, "r");

    if (in_file == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier.\n");
        return;
    }

    display_text(get_line(file, 14));
    scanf("%9s", new_player->nom);  // Limiting input to 9 characters for safety
    printf("\n%s %s \n\n", get_line(file, 16), new_player->nom);

    display_text(get_line(file, 19));
    scanf(" %c", &new_player->nationalite); // Use %c for single characters

    display_text(get_line(file, 21));
    scanf(" %c", &new_player->race); // Use %c for single characters

    if (new_player->race == 'h' || new_player->race == 'H') {
        display_text(get_line(file, 24));
    }

    if (new_player->race == 'r' || new_player->race == 'R') {
        display_text(get_line(file, 26));
    }
    new_player->player_position = create_position(0, 0);
    new_player->hearts = 5;
    new_player->weapon = 0;

    fclose(in_file);

}

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

int play_game(char lang[3]) 
{
    int user_choice = 0, game_choice = 0;
    char sentence[100], folder[10];
    room_array rooms;
    Player new_player;
    char exit[10], direct[1], room_name[10]; // Increase the size of 'language' to store "fr" or "en"
    const char *file = "text-en.txt";  // Default to using the English file
    FILE *in_file = fopen(file, "r");

    srand(time(NULL)); // Initialize the random number generator with the current time

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


        // Read the size of the file
        struct stat sb;
        stat(file, &sb);

        // Allocate the file
        char *file_allocation = malloc(sb.st_size);

        // Display menu
        while (1)
        {
            display_introduction(lang);
            scanf("%i", &game_choice);

            switch (game_choice)
            {
            case 1:
                create_player(&new_player, file);
                create_rooms(rooms);

                for (int i = 0; i < 25; i++) {
            snprintf(folder, sizeof(folder), "room%d", i);
            create_room_folder(folder, &rooms[i / MAX][i % MAX], get_language(lang));
        }

        // Affichage des salles si demandé
        /*
        printf("Do you want to show the rooms? (1 for yes, 0 for no): ");
        scanf("%i", &user_choice);

        if (user_choice == 1) {
            for (int i = 0; i < MAX; i++) {
                for (int j = 0; j < MAX; j++) {
                    print_room(rooms[i][j]);
                }
            }
        }
        */

        // Boucle pour changer de salle jusqu'à la fin du jeu
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
            }
            

        }

        while (!check_game_over(&new_player) && rooms[position_boss.row][position_boss.column].boss == 1) {
            change_room(&new_player, get_language(lang));
            sprintf(room_name, "room%d", (new_player.player_position.row * MAX) + new_player.player_position.column );
            protocol_room(&room_name, &rooms[new_player.player_position.row][new_player.player_position.column], &new_player, get_language(lang));
            show_player(new_player);
        }

        // Affichage du joueur et libération de la mémoire
        free(file_allocation);

        // Création des dossiers pour chaque salle
        
                break;

            case 2:
                break;

            case 3:
                return 0;

            default:
                break;
            }

            
        }

        

        scanf("%s", exit); // Pour sortir de la boucle do-while

    } while (strcmp(exit, "quit") != 0 && strcmp(exit, "quitter") != 0);

    fclose(in_file);

}

int main(int argc, char *argv[])
{
    char exit[10], lang[3];
    
    do {
        printf("Choose your lang (fr) or (en): ");
        scanf("%2s", lang);

        play_game(lang);    

        printf("%s\n", get_line(get_language(lang), 114));
        scanf("%9s", exit); // Pour éviter tout dépassement de tampon

    } while (strcmp(exit, "quit") != 0 && strcmp(exit, "quitter") != 0);

    return 0;
}
