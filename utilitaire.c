#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> // Pour usleep
#include "utilitaire.h"

#define VIDE ' '      // Symbole pour une case vide
#define VEHICULE '*'  // Symbole pour un véhicule
#define ROUTE_H '-'   // Route horizontale
#define ROUTE_V '|'   // Route verticale

pthread_mutex_t mutex;  // Mutex pour protéger la carte

// Fonction pour lire les dimensions du réseau routier et le nombre de véhicules
void lire_fichier(char* nom_fichier, int* hauteur, int* largeur, int* nb_vehicules) {
    FILE* fichier = fopen(nom_fichier, "r");
    if (!fichier) {
        perror("Erreur d'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    fscanf(fichier, "%d %d", hauteur, largeur);
    fscanf(fichier, "%d", nb_vehicules);
    fclose(fichier);
}

// Fonction pour initialiser la carte
void initialiser_carte(char** carte, int hauteur, int largeur) {
    for (int i = 0; i < hauteur; i++) {
        for (int j = 0; j < largeur; j++) {
            carte[i][j] = VIDE;
        }
    }

    // Placer les routes au milieu
    int route_horizontale = hauteur / 2;
    int route_verticale = largeur / 2;

    for (int j = 0; j < largeur; j++) {
        carte[route_horizontale][j] = ROUTE_H;
    }
    for (int i = 0; i < hauteur; i++) {
        carte[i][route_verticale] = ROUTE_V;
    }
}

// Fonction pour placer les véhicules aléatoirement
void placer_vehicules(char** carte, int hauteur, int largeur, int nb_vehicules, Vehicule* vehicules) {
    srand(time(NULL));
    int route_horizontale = hauteur / 2;
    int route_verticale = largeur / 2;

    for (int i = 0; i < nb_vehicules; i++) {
        int position;
        do {
            if (rand() % 2 == 0) { // Route horizontale
                position = rand() % largeur;
                if (carte[route_horizontale][position] == VIDE) {
                    carte[route_horizontale][position] = VEHICULE;
                    vehicules[i].x = route_horizontale;
                    vehicules[i].y = position;
                    vehicules[i].direction = 'H'; // Horizontal
                    break;
                }
            } else { // Route verticale
                position = rand() % hauteur;
                if (carte[position][route_verticale] == VIDE) {
                    carte[position][route_verticale] = VEHICULE;
                    vehicules[i].x = position;
                    vehicules[i].y = route_verticale;
                    vehicules[i].direction = 'V'; // Vertical
                    break;
                }
            }
        } while (1);
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
    printf("\n");
}

// Fonction exécutée par chaque thread pour déplacer un véhicule
void* deplacer_vehicule(void* arg) {
    Vehicule* vehicule = (Vehicule*)arg;

    while (1) {
        pthread_mutex_lock(&mutex); // Protéger l'accès à la carte
        int x = vehicule->x;
        int y = vehicule->y;

        // Déplacement horizontal
        if (vehicule->direction == 'H') {
            if (y + 1 < vehicule->largeur && vehicule->carte[x][y + 1] == VIDE) {
                vehicule->carte[x][y] = ROUTE_H;
                vehicule->carte[x][y + 1] = VEHICULE;
                vehicule->y++;
            } else {
                vehicule->carte[x][y] = ROUTE_H; // Libérer la case finale
                pthread_mutex_unlock(&mutex);
                break; // Sortir de la carte
            }
        }
        // Déplacement vertical
        else if (vehicule->direction == 'V') {
            if (x + 1 < vehicule->hauteur && vehicule->carte[x + 1][y] == VIDE) {
                vehicule->carte[x][y] = ROUTE_V;
                vehicule->carte[x + 1][y] = VEHICULE;
                vehicule->x++;
            } else {
                vehicule->carte[x][y] = ROUTE_V; // Libérer la case finale
                pthread_mutex_unlock(&mutex);
                break; // Sortir de la carte
            }
        }
        pthread_mutex_unlock(&mutex);

        // Pause pour simuler un déplacement progressif
        usleep(500000);
    }

    return NULL;
}
