#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <unistd.h>


#define MAX 5


typedef struct {
    short id_salle; // valeur fixe
    unsigned short nb_porte; // valeur fixe 
    unsigned short piege; // valeur aléatoire 
    unsigned short coffre; // booleen + valeur aléatoire
    unsigned short reserve; // booleen + valeur fixe peut apparaître que dans une seule salle 
    unsigned short boss; // booleen + vrai si la salle est la reserve
} salle;

typedef salle tableau[MAX][MAX];

typedef int liste[2];

void valeur_aleatoire_salle(salle *s) {
    int result;


    result = rand() % 2 ;
    (*s).piege = (rand() % 2); // 0 ou 1 pour représenter faux ou vrai

    result = rand() % 5;
    if (result == 0) (*s).coffre = 1; // 1/5 que la valeur soit vrai
    else if (result != 0) (*s).coffre = 0; // 4/5 que la valeur soit fausse

    (*s).reserve = 0;
    (*s).boss = 0; 
}

int nb_portes(int ligne, int colonne) {
    int nb_portes_output = 0;

    if ((ligne == 0 && colonne == 0) || (ligne == 0 && colonne == 4) || (ligne == 4 && colonne == 0) || (ligne == 4 && colonne == 4))
        nb_portes_output = 2;
    else if ((ligne == 1 && colonne == 0) || (ligne == 2 && colonne == 0) || (ligne == 3 && colonne == 0) ||
             (ligne == 1 && colonne == 4) || (ligne == 2 && colonne == 4) || (ligne == 3 && colonne == 4))
        nb_portes_output = 3;
    else if ((ligne == 0 && colonne == 1) || (ligne == 0 && colonne == 2) || (ligne == 0 && colonne == 3) ||
             (ligne == 4 && colonne == 1) || (ligne == 4 && colonne == 2) || (ligne == 4 && colonne == 3))
        nb_portes_output = 3;
    else
        nb_portes_output = 4;

    return nb_portes_output;
}

salle creation_d_une_salle(int ligne, int colonne) {
    salle nouvelle_salle;
    nouvelle_salle.id_salle = (ligne*MAX)+colonne;
    nouvelle_salle.nb_porte = nb_portes(ligne,colonne);

    if ((ligne == 0 && colonne == 0)) {
        nouvelle_salle.piege = 0;
        nouvelle_salle.coffre = 0;
        nouvelle_salle.boss = 0;
        nouvelle_salle.reserve = 0;
    }

    else {
    valeur_aleatoire_salle(&nouvelle_salle);
    }

    return nouvelle_salle;
}

void creation_reserve(tableau *tab_salles) {
    int nb_random = rand() % 25;
    salle (*psalles)[MAX] = tab_salles;

    while((nb_random == 0/MAX != 0) && (nb_random % MAX != 0)) {
        nb_random = rand() % 25;
    }


    psalles [nb_random/MAX][nb_random/MAX % MAX].reserve = 1;
    psalles [nb_random/MAX][nb_random/MAX % MAX].boss = 1;


}

void creation_des_salles(tableau *tab_salles) {
    salle (*psalles)[MAX] = tab_salles;

    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            psalles[i][j] = creation_d_une_salle(i, j);
        }
    }
    creation_reserve(*tab_salles);
}

void display_room(int nb_rooms) {
    int check = 0;
    
    for (int ligne = 0; ligne < nb_rooms ; ligne++) {
        char room[5][5] = {
        {'+', '-', '-', '-', '+'},
        {'|', ' ', ' ', ' ', '|'},
        {'|', ' ', ' ', ' ', '|'},
        {'|', ' ', ' ', ' ', '|'},
        {'+', '-', '-', '-', '+'}
    };

   for (int i = 0; i < 5; ++i) {
        for (int k = 0; k < 5; ++k) {  // Repeat 5 times for each square
            if (k == 0 && ligne == 0) room[2][2] = 'X'; //
            else room[2][2] = ' '; //
            for (int j = 0; j < 5; ++j) {
                printf("%c ", room[i][j]);
            }
            printf("");  // Add space between squares
        }
        printf("\n");
    }
    }


}



void afficher_salle(salle room) {
    printf("Salle %d-%d - Piege: %hu, Coffre: %hu, Nombre de portes : %hu, Boss : %hu, Reserve: %hu\n",
           (room.id_salle/MAX), (room.id_salle % MAX), room.piege,
           room.coffre, room.nb_porte, room.boss, room.reserve);
}

void play_sound() {
    printf("\a");  // Émet un caractère de contrôle ASCII pour générer un bip
}

char *recuperer_ligne(const char *nom_fichier, int numero_souhaitee)
{
    FILE *fichier = fopen(nom_fichier, "r");

    char ligne[1024];
    int numero_actuel = 0;

    struct stat sb;
    if(stat(nom_fichier, &sb) != 0)
    {
        perror("Erreur de récupération des informations sur le fichier");
        fclose(fichier);
        return NULL;
    }

    char *allocation = malloc(sb.st_size);
    
    while(fgets (ligne,sizeof ligne, fichier) != NULL)
    {
        numero_actuel += 1;

        if (numero_actuel == numero_souhaitee){
            fclose(fichier);

            // Supprimer le caractère de fin de ligne s'il est présent
            size_t longueur_ligne = strlen(ligne);
            if (longueur_ligne > 0 && ligne[longueur_ligne - 1] == '\n') {
                ligne[longueur_ligne - 1] = '\0';
            }

            char *copie_ligne = malloc(strlen(ligne) + 1);
            if (copie_ligne == NULL) {
                perror("Erreur d'allocation mémoire");
                return NULL;
            }

            strcpy(copie_ligne, ligne);
            return copie_ligne;
        }
    }

    fclose(fichier);
}

void between_lines(int start, int end, char nom_ficher[30]) {
    // Read the content of the file "nom_ficher" between the start and end
    
    for (int i = start; i <= end; i++) {
        printf("> %s\n", recuperer_ligne(nom_ficher, i));
        printf("\n");
        // sleep(3); 
    }
}

void displayIntroduction() {
    printf("| |  | ||  _  | ___ \\ |   |  _  \\ | |  | |/ _ \\ | ___ \\    /   |\n");
    printf("| |  | || | | | |_/ / |   | | | | | |  | / /_\\ \\| |_/ /   / /| |\n");
    printf("| |\\| || | | |    /| |   | | | | | |\\| |  _  ||    /   / /_| |\n");
    printf("\\  /\\  /\\ \\_/ / |\\ \\| |___| |/ /  \\  /\\  / | | || |\\ \\   \\___  |\n");
    printf(" \\/  \\/  \\___/\\_| \\_\\_____/___/    \\/  \\/\\_| |_/\\_| \\_|     |_/\n\n");
    printf("1. New Game\n");
    printf("2. Continue\n\n");
    printf("Choose an option by typing the corresponding number and press Enter:\n");
}



int main(int argc, char *argv[]) {
    int choose = 0, choice=0;
    char sentence[100];
    tableau salles;
    char exit[10], langue[3]; // Augmente la taille de langue pour stocker "fr" ou "en"
    
    const char *fichier = "text-en.txt"; // Par défaut, utilise le fichier en anglais
    FILE *dans_le_fichier = fopen(fichier, "r");

    if (dans_le_fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier %s\n", fichier);
        return 1; // Quitte le programme en cas d'erreur
    }

    do {
        printf("Choose your language (fr) or (en): ");
        scanf("%s", langue);

        if (strcmp(langue, "fr") == 0 || strcmp(langue, "FR") == 0) {
            fclose(dans_le_fichier);
            fichier = "text-fr.txt";
            dans_le_fichier = fopen(fichier, "r");
        } else if (strcmp(langue, "en") == 0 || strcmp(langue, "EN") == 0) {
            fclose(dans_le_fichier);
            fichier = "text-en.txt";
            dans_le_fichier = fopen(fichier, "r");
        }

        srand(time(NULL)); // Initialiser le générateur de nombres aléatoires avec le temps actuel

        // Lecture de la taille du fichier
        struct stat sb;
        stat(fichier, &sb);

        // Allocation du fichier
        char *allocation_fichier = malloc(sb.st_size);

       

        //display menu
        while(choice == 0) {
            displayIntroduction();
            scanf("%i", &choice);

            switch (choice)
            {
            case 1:
                break;
            
            case 2:
                break;

            default:
                break;
            }
        }

        // Read introduction
        between_lines(1, 11, fichier);
        
        // Informations of the first room
        between_lines(31,32,fichier);
        display_room(5);
        between_lines(34,37, fichier);

        free(allocation_fichier);

        // Exemple d'accès à une salle et affichage de ses attributs
        printf("Voulez-vous créer les salles ? (1 pour oui, 0 pour non): ");
        scanf("%i", &choose);

        if (choose == 1) {
            creation_des_salles(salles);

            for (int i = 0; i < MAX; i++) {
                for (int j = 0; j < MAX; j++) {
                    afficher_salle(salles[i][j]);
                }
            }
        }

        scanf("%s", exit);

    } while (strcmp(exit, "quit") != 0 && strcmp(exit, "quitter") != 0);

    fclose(dans_le_fichier);

    return 0;
}