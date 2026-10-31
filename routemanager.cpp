#include "routemanager.h"
#include <limits>
#include <sstream>
#include <QFile>

// Lit un fichier entier et renvoie son contenu dans "contenu".
// On passe par QFile (et non std::ifstream) pour pouvoir lire les fichiers
// embarqués dans l'exécutable via Ressource.qrc, par exemple ":/villes.csv".
static bool lireFichier(const std::string& chemin, std::string& contenu)
{
    QFile fichier(QString::fromStdString(chemin));
    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    contenu = fichier.readAll().toStdString();
    return true;
}

// Constructeur : on initialise les structures de données
RouteManager::RouteManager() {
    // On remplit la matrice avec "l'infini" pour dire qu'au début, aucune ville n'est connectée
    matriceTemps.assign(200, std::vector<double>(200, std::numeric_limits<double>::infinity()));

    // La distance entre une ville et elle-même est forcément de 0
    for (int i = 0; i < 200; i++) {
        matriceTemps[i][i] = 0;
    }

    // Le tableau stockage sert à retenir le chemin. On met -1 (aucun successeur) partout
    stockage.assign(200, std::vector<int>(200, -1));
}

// Chargement des villes à partir du fichier CSV
bool RouteManager::loadCities(std::string fichier)
{
    std::string contenu;

    // Si le fichier a un problème ou n'existe pas, on arrête
    if (!lireFichier(fichier, contenu)) {
        return false;
    }
    std::istringstream monFichier(contenu);

    std::string ligne;
    // On saute la première ligne du CSV (les entêtes de colonnes)
    std::getline(monFichier, ligne);

    // On lit le fichier ligne par ligne
    while (std::getline(monFichier, ligne)) {
        std::stringstream ss(ligne);
        std::string nom, idStr, poubelle, latStr, lonStr;

        // On découpe la ligne selon les virgules du CSV
        std::getline(ss, nom, ',');
        std::getline(ss, idStr, ',');
        std::getline(ss, poubelle, ','); // Colonne inutile qu'on ignore
        std::getline(ss, latStr, ',');
        std::getline(ss, lonStr, ',');

        // On remplit notre structure de données City
        City c;
        c.nom = nom;
        c.id = std::stoi(idStr); // Conversion texte -> entier
        c.latitude = std::stod(latStr); // Conversion texte -> double
        c.longitude = std::stod(lonStr);

        // On ajoute la ville dans notre vecteur global
        listeVilles.push_back(c);
    }

    return true;
}

// Chargement des routes (les liaisons directes)
bool RouteManager::loadRoads(std::string fichier)
{
    std::string contenu;

    if (!lireFichier(fichier, contenu)) {
        return false;
    }
    std::istringstream monFichier(contenu);

    std::string ligne;

    // Lecture ligne par ligne
    while (std::getline(monFichier, ligne)) {
        std::stringstream ss(ligne);
        std::string idA_str, idB_str, temps_str;

        // Découpage des données
        std::getline(ss, idA_str, ',');
        std::getline(ss, idB_str, ',');
        std::getline(ss, temps_str, ',');

        int idA = std::stoi(idA_str);
        int idB = std::stoi(idB_str);
        double temps = std::stod(temps_str);

        // On remplit la matrice de façon symétrique (graphe non orienté : A vers B = B vers A)
        matriceTemps[idA][idB] = temps;
        matriceTemps[idB][idA] = temps;

        // Initialisation du chemin direct dans le tableau de stockage
        stockage[idA][idB] = idB;
        stockage[idB][idA] = idA;
    }

    return true;
}

// Algorithme de Floyd-Warshall pour calculer tous les plus courts chemins
void RouteManager::runAlgo()
{
    // Boucle k : la ville étape par laquelle on essaie de passer
    for (int k = 0; k < 200; k++) {
        // Boucle i : la ville de départ
        for (int i = 0; i < 200; i++) {
            // Boucle j : la ville d'arrivée
            for (int j = 0; j < 200; j++) {

                // On vérifie si passer par la ville k est plus court que le trajet direct actuel entre i et j
                if (matriceTemps[i][k] + matriceTemps[k][j] < matriceTemps[i][j]) {

                    // Si c'est plus court, on met à jour la matrice avec ce nouveau temps
                    matriceTemps[i][j] = matriceTemps[i][k] + matriceTemps[k][j];

                    // On retient dans le tableau de stockage qu'il faut passer par k
                    stockage[i][j] = stockage[i][k];
                }
            }
        }
    }
}

// Getter pour récupérer le temps total entre deux villes
double RouteManager::getTempo(int idA, int idB)
{
    return matriceTemps[idA][idB];
}

// Getter pour récupérer la liste complète des villes
std::vector<City> RouteManager::getVilles()
{
    return listeVilles;
}

// Fonction qui reconstruit la liste des IDs du chemin le plus court
std::vector<int> RouteManager::getChemin(int depart, int arrivee) {
    std::vector<int> trajet;

    // Si le temps est resté à l'infini, ça veut dire qu'il n'y a aucune route possible
    if (matriceTemps[depart][arrivee] == std::numeric_limits<double>::infinity()) {
        return trajet; // On renvoie un vecteur vide
    }

    int actuel = depart;

    // On suit les étapes enregistrées dans le tableau stockage jusqu'à arriver à destination
    while (actuel != arrivee) {
        trajet.push_back(actuel);

        // On passe à la ville suivante dictée par la matrice de stockage
        actuel = stockage[actuel][arrivee];
    }

    // On n'oublie pas d'ajouter la ville d'arrivée tout à la fin du trajet
    trajet.push_back(arrivee);

    return trajet;
}