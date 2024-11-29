#ifndef UTILITAIRE_H
#define UTILITAIRE_H

typedef struct {
    int x, y;           // Coordonnées du véhicule
    char direction;     // Direction du déplacement ('H' pour horizontal, 'V' pour vertical)
    char** carte;       // Pointeur vers la carte partagée
    int hauteur;        // Hauteur de la carte
    int largeur;        // Largeur de la carte
} Vehicule;

void lire_fichier(char* nom_fichier, int* hauteur, int* largeur, int* nb_vehicules);
void initialiser_carte(char** carte, int hauteur, int largeur);
void placer_vehicules(char** carte, int hauteur, int largeur, int nb_vehicules, Vehicule* vehicules);
void afficher_carte(char** carte, int hauteur, int largeur);
void* deplacer_vehicule(void* arg);

#endif
