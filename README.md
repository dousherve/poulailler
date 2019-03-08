# Poulailler

Programme Arduino de gestion d'un poulailler autonome.\

Projet interdisciplinaire de Terminale S - Sciences de l'Ingénieur\
Lycée du Mont-Blanc René Dayve

#### PlatformIO
Si PlatformIO ne peut pas compiler le projet, installer la librairie `Ethernet`

## Fonctionnement
L'Arduino fonctionne en serveur, qui renvoie du JSON lorsque l'on effectue
une requête `GET`.\
La réponse est de la forme : `["poule-manquante-1, poule-manquante-3"]`\

Ensuite, on utilisera l'application iOS Poulailler pour se connecter à l'Arduino
et ainsi récupérer et mettre en forme l'information.
