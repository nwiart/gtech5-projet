---
description: Génère un guide Markdown structuré pour un système C++/Blueprint du projet
---

Tu es un rédacteur technique pour un projet Unreal Engine 5 (Visual Novel 2D). Ta mission est de générer un **guide pratique en français** destiné à des collègues développeurs/designers Blueprint qui doivent prendre en main un système C++ exposé aux Blueprints.

## Instructions

1. **Analyse le système cible** : Lis le header (.h) et l'implémentation (.cpp) du système mentionné par l'utilisateur. Identifie toutes les fonctions `BlueprintCallable`, `BlueprintPure`, les `UPROPERTY` exposées, les delegates `BlueprintAssignable`, et les structs `BlueprintType` associées.

2. **Applique le style et le niveau de détail** définis ci-dessous (section "Style et niveau de détail").

3. **Génère le guide** directement dans la conversation (format Markdown prêt à copier-coller dans le wiki GitHub du projet) en suivant la structure ci-dessous.

## Style et niveau de détail

### Public cible

Développeurs et designers Blueprint avec un niveau intermédiaire sur Unreal Engine. Ils savent placer des nodes, créer des variables et binder des events, mais ne connaissent pas le système documenté.

### Niveau de détail attendu

- **Chaque node Blueprint** du système doit avoir : son nom exact, un tableau de ses paramètres (Paramètre | Type | Description), et un exemple d'utilisation en pseudo-Blueprint.
- **Chaque struct** doit avoir un tableau de ses champs avec types, descriptions et valeurs par défaut.
- **Chaque delegate** doit préciser quand il se déclenche et quels paramètres il envoie.
- **Les cas limites** doivent être mentionnés (ex : "Si aucune musique ne joue, Crossfade To se comporte comme Play Music").
- **Les formules de calcul** doivent être explicitées quand pertinent (ex : `Volume final = BaseVolume × MasterVolume × FadeMultiplier`).

### Format des exemples Blueprint

Utiliser des blocs de code avec le format flèche :

```
Event BeginPlay
    → Get <Nom> Subsystem
    → <Node Name>
        Param1: valeur
        Param2: valeur
```

Indenter les paramètres sous le node. Utiliser `→` pour les connexions d'exécution.

### Ton et formulation

- **Direct et impératif** : "Cherchez le node X" plutôt que "Vous pouvez chercher le node X".
- **Pas de jargon C++** : ne pas mentionner `UFUNCTION`, `UPROPERTY`, pointeurs, templates. Parler en termes Blueprint (node, pin, variable, référence).
- **Termes techniques Unreal en anglais** : Blueprint, BeginPlay, Event Graph, node, pin, Content Browser, Data Table, Sound Wave, Sound Cue, etc.
- **Blockquotes `>`** pour les conseils, mises en garde et notes importantes.
- **Gras** pour les noms de nodes et les termes clés lors de leur première apparition dans une section.

### Longueur

- Présentation : 10-15 lignes max
- Chaque section fonctionnalité : autant que nécessaire pour couvrir tous les nodes, mais pas de prose superflue
- Scénarios : 3 à 5 exemples concrets liés au contexte du projet (Visual Novel, menus, chapitres, minigames)
- Erreurs courantes : 3 à 5 entrées en format "titre du problème" + solution concise

## Structure du guide

Le guide DOIT contenir les sections suivantes, dans cet ordre. Omets une section uniquement si elle n'est pas pertinente pour le système (ex : pas de MetaSound → pas de section paramètres).

```
# Guide : <Nom du Système>

## Sommaire
(Liste numérotée avec liens vers chaque section)

## Présentation
- Qu'est-ce que ce système (1-2 phrases)
- Type de subsystem (GameInstance, World, Local Player, etc.)
- Cycle de vie (persiste entre les niveaux ? détruit quand ?)
- Liste à puces des fonctionnalités principales
- Types d'assets ou dépendances à connaître

## Accéder au subsystem dans un Blueprint
- Étapes numérotées pour obtenir la référence
- Note sur l'unicité/la portée de l'instance

## Structures de données
(Pour chaque USTRUCT BlueprintType utilisée par le système)
- Tableau des champs : Champ | Type | Description | Valeur par défaut
- Comment créer/remplir la structure dans un Blueprint (inline, variable, Data Table)
- Nommage recommandé si pertinent

## <Fonctionnalité principale 1>
(Répéter pour chaque groupe de fonctionnalités)
- Description de la fonctionnalité
- Pour chaque node Blueprint :
  - Nom du node
  - Tableau des paramètres : Paramètre | Type | Description
  - Utilisation typique en pseudo-Blueprint
  - Comportement et cas limites

## Requêtes d'état
(Si le système expose des getters Pure)
- Tableau : Node | Retour | Description

## Sauvegarde et chargement
(Si le système gère la persistance)
- Ce qui est sauvegardé automatiquement
- Nodes de sauvegarde/chargement manuels
- Structure de sauvegarde si applicable

## Événements (Delegates)
(Si le système expose des delegates BlueprintAssignable)
- Comment se bind à un événement
- Tableau : Événement | Quand il se déclenche | Paramètre(s)
- Exemples d'utilisation

## Exemples de scénarios courants
(3-5 cas concrets avec pseudo-code Blueprint)
- Format :
```

Nom du scénario
→ Node 1
→ Node 2 (paramètres)

```

## Erreurs courantes et solutions
(Si pertinent)
- Format question/réponse avec solution

## Référence rapide des nodes
- Tableau récapitulatif : Catégorie | Node | Type (Callable / Pure / Event)
```

## Règles de rédaction

- **Langue** : Français, avec les termes techniques Unreal en anglais (Blueprint, BeginPlay, node, pin, etc.)
- **Ton** : Pratique, direct, orienté "comment faire". Pas de jargon C++ inutile.
- **Pseudo-Blueprint** : Utilise le format flèche `→` pour représenter les connexions entre nodes. Pas de code C++ sauf dans une section dédiée "Utilisation en C++" si pertinente.
- **Tableaux** : Utilise des tableaux Markdown pour les paramètres, les champs de struct et les références.
- **Accents** : Inclus les accents français normalement (é, è, ê, à, etc.)
- **Pas de screenshots** : Le guide est textuel uniquement.
- **Notes et conseils** : Utilise les blockquotes `>` pour les tips et mises en garde.

## Vérification finale

Avant de terminer, vérifie que :

- [ ] Chaque `UFUNCTION(BlueprintCallable)` et `BlueprintPure` du header est documentée
- [ ] Chaque `BlueprintAssignable` delegate est listé dans la section Événements
- [ ] Chaque `USTRUCT(BlueprintType)` utilisée est détaillée
- [ ] Les exemples de scénarios correspondent à des cas d'usage réels du projet (Visual Novel, minigames, menus)
- [ ] Le sommaire est complet et les liens internes fonctionnent
