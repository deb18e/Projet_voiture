#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_VEHICULES 100

// Structure pour représenter un véhicule
typedef struct {
    int x, y;           // Position du véhicule
    char direction;     // Direction du véhicule ('H' pour horizontal, 'V' pour vertical)
    int en_cours;       // 1 si le véhicule est encore en mouvement, 0 sinon
    char** carte;       // Carte du réseau routier
    int hauteur, largeur; // Dimensions de la carte
    int id;             // Identifiant du véhicule
} Vehicule;

// Mutex global pour synchronisation
pthread_mutex_t mutex;

// Fonction pour lire les données du fichier
void lire_fichier(const char* fichier, int* hauteur, int* largeur, int* nb_vehicules) {
    FILE* fp = fopen(fichier, "r");
    if (!fp) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    fscanf(fp, "%d %d", hauteur, largeur);
    fscanf(fp, "%d", nb_vehicules);
    fclose(fp);
}

// Fonction pour initialiser la carte
void initialiser_carte(char** carte, int hauteur, int largeur) {
    for (int i = 0; i < hauteur; i++) {
        for (int j = 0; j < largeur; j++) {
            carte[i][j] = ' '; // Cases vides par défaut
        }
    }

    // Ajouter une route horizontale au milieu
    for (int j = 0; j < largeur; j++) {
        carte[hauteur / 2][j] = '-';
    }

    // Ajouter une route verticale au milieu
    for (int i = 0; i < hauteur; i++) {
        carte[i][largeur / 2] = '|';
    }
}

// Fonction pour afficher la carte
void afficher_carte(char** carte, int hauteur, int largeur) {
    for (int i = 0; i < hauteur; i++) {
        for (int j = 0; j < largeur; j++) {
            printf("%c", carte[i][j]);
        }
        printf("\n");
    }
}

// Fonction pour placer les véhicules
void placer_vehicules(char** carte, int hauteur, int largeur, int nb_vehicules, Vehicule* vehicules) {
    for (int i = 0; i < nb_vehicules; i++) {
        int x, y;
        char direction;

        do {
            // Placement aléatoire sur une des routes
            if (rand() % 2 == 0) {
                // Route horizontale
                x = hauteur / 2;
                y = rand() % largeur;
                direction = 'H';
            } else {
                // Route verticale
                x = rand() % hauteur;
                y = largeur / 2;
                direction = 'V';
            }
        } while (carte[x][y] != '-'); // Vérifier que la position est libre

        // Placer le véhicule
        vehicules[i].x = x;
        vehicules[i].y = y;
        vehicules[i].direction = direction;
        vehicules[i].en_cours = 1;
        vehicules[i].carte = carte;
        vehicules[i].hauteur = hauteur;
        vehicules[i].largeur = largeur;
        vehicules[i].id = i;

        carte[x][y] = '*'; // Marquer le véhicule sur la carte
    }
}

// Fonction pour déplacer un véhicule
void* deplacer_vehicule(void* arg) {
    Vehicule* vehicule = (Vehicule*)arg;

    while (vehicule->en_cours) {
        pthread_mutex_lock(&mutex);

        // Effacer l'ancienne position
        vehicule->carte[vehicule->x][vehicule->y] = '-';

        // Déplacer le véhicule
        if (vehicule->direction == 'H') {
            if (vehicule->y + 1 < vehicule->largeur && vehicule->carte[vehicule->x][vehicule->y + 1] == '-') {
                vehicule->y++;
            } else {
                vehicule->en_cours = 0; // Sortie de la carte
            }
        } else if (vehicule->direction == 'V') {
            if (vehicule->x + 1 < vehicule->hauteur && vehicule->carte[vehicule->x + 1][vehicule->y] == '|') {
                vehicule->x++;
            } else {
                vehicule->en_cours = 0; // Sortie de la carte
            }
        }

        // Mettre à jour la nouvelle position
        if (vehicule->en_cours) {
            vehicule->carte[vehicule->x][vehicule->y] = '*';
        }

        pthread_mutex_unlock(&mutex);
        usleep(500000); // Pause de 500 ms
    }

    return NULL;
}

// Programme principal
int main() {
    int hauteur, largeur, nb_vehicules;
    char* fichier = "reseau.txt";

    // Lire les données du fichier
    lire_fichier(fichier, &hauteur, &largeur, &nb_vehicules);

    // Initialiser la carte
    char** carte = malloc(hauteur * sizeof(char*));
    for (int i = 0; i < hauteur; i++) {
        carte[i] = malloc(largeur * sizeof(char));
    }
    initialiser_carte(carte, hauteur, largeur);

    // Placer les véhicules
    Vehicule* vehicules = malloc(nb_vehicules * sizeof(Vehicule));
    placer_vehicules(carte, hauteur, largeur, nb_vehicules, vehicules);

    // Afficher la carte initiale
    printf("Carte initiale :\n");
    afficher_carte(carte, hauteur, largeur);

    // Initialiser le mutex
    pthread_mutex_init(&mutex, NULL);

    // Créer les threads pour chaque véhicule
    pthread_t* threads = malloc(nb_vehicules * sizeof(pthread_t));
    for (int i = 0; i < nb_vehicules; i++) {
        if (pthread_create(&threads[i], NULL, deplacer_vehicule, &vehicules[i]) != 0) {
            perror("Erreur lors de la création du thread");
            exit(EXIT_FAILURE);
        }
    }

    // Attendre que tous les threads terminent
    for (int i = 0; i < nb_vehicules; i++) {
        pthread_join(threads[i], NULL);
    }

    // Afficher la carte finale
    printf("Carte finale :\n");
    afficher_carte(carte, hauteur, largeur);

    // Libérer les ressources
    for (int i = 0; i < hauteur; i++) {
        free(carte[i]);
    }
    free(carte);
    free(vehicules);
    free(threads);

    // Détruire le mutex
    pthread_mutex_destroy(&mutex);

    return 0;
}
