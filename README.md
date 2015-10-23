# Horloge "stargate" avec bandeau LED (WS2812)

- Schéma Kicad

Attention à l'alimentation du ESP2866 qui nécessite + de 200mA au démarrage.

- Programme

Encore en développement.
Récupère l'heure par NTP (OK)
Une couleur peut être envoyée pour affichage ! - en cours, avec utilisation de PubNub

- Le modèle

Réalisé en médium 3mm.
La face avant avec les chiffres + 3 épaisseurs collées réalisées en 4 morceaux (asymétriques alternées)
Le bandeau (largeur 1cm) est colé à l'arrière de la face avant, sur les 3 épaisseurs arrière.

Initialement, je voulais le créer à partir d'une découpe d'un STL (crée à partir de openSCAD) par 123Make.
Mais les morceaux n'étaient pas compactés (voir le PDF)
J'ai donc arrangé les morceaux (tranchage dans openSCAD) à la main (stargate-pack.svg) pour optimiser la découpe du médium 3mm.