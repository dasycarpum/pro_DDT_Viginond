# Introduction

La gestion des inondations lors d'un cellule de crise en Préfecture demande à l'agent d'astreinte une connaissance approfondie dans plusieurs domaines techniques, pour fournir des éléments pertinents en un temps restreint.

L'objectif de cette application est d'agréger toutes les informations connues, et dégager les axes prioritaires d'intervention des services. Ainsi sur base des données suivantes :

    ▪ les hauteurs d'eau en temps réel aux stations de mesure (site https://www.vigicrues.gouv.fr/)
    ▪ la modélisation des crues par le CEREMA
    ▪ les plans de prévention des risques et les atlas des zones inondables de la DDT de la Moselle
    ▪ les listes des enjeux ponctuels, linéaires et surfaciques produites par la DDT de la Moselle

l'application, de type IHM, assiste l'utilisateur en lui fournissant de manière semi-automatisée :

    ▪ un visuel instantanné sur le profil de la crue du cours d'eau ainsi que de ses affluents,
    ▪ une projection des hauteurs d'eau à t +4 heures, 
    ▪ une cartograhie dynamique de l'emprise modélisée de la crue, et des enjeux matériels & humains.

# Environnement de développement

    ▪ Langage : C++11
    ▪ Framework : Qt (version 5.12.6)
    ▪ IDE : Qt Creator (version 4.10.2)
    ▪ Bibliothèque complémentaire : qwt (version 6.1.4)
    ▪ Gestionnaire de versions du code : Git (version 2.28.0)
    ▪ Plateforme applicative : Azure Devops sous https://dev.azure.com/dasycarpum-pro-DDT/Viginond%2057
