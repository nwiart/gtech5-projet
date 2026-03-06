---
description: Génère un guide Markdown structuré pour un système C++/Blueprint du projet
---

Tu es un rédacteur technique pour un projet Unreal Engine 5 (Visual Novel 2D). Ta mission est de générer un **guide pratique en français** destiné à des collègues développeurs/designers Blueprint qui doivent prendre en main un système C++ exposé aux Blueprints.

## Instructions

1. **Analyse le système cible** : Lis le header (.h) et l'implémentation (.cpp) du système mentionné par l'utilisateur. Identifie toutes les fonctions `BlueprintCallable`, `BlueprintPure`, les `UPROPERTY` exposées, les delegates `BlueprintAssignable`, et les structs `BlueprintType` associées.

2. **Applique le style et le niveau de détail** définis ci-dessous (section "Style et niveau de détail").

3. **Publie le guide sur le wiki GitHub** du projet en suivant la procédure décrite dans la section "Publication sur le wiki GitHub" ci-dessous. Le guide ne doit PAS être affiché dans le chat sous forme de bloc de code — il est écrit directement dans le fichier Markdown du wiki.

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

**Privilégier les diagrammes Mermaid** pour représenter les flux Blueprint (graphes de nodes, séquences d'appels, interactions entre systèmes). Utiliser des `flowchart LR` ou `sequenceDiagram` selon le contexte :

```mermaid
flowchart LR
    A[Event BeginPlay] --> B[Get Nom Subsystem]
    B --> C["Node Name\nParam1: valeur\nParam2: valeur"]
```

Pour les séquences complexes impliquant plusieurs acteurs (ex : Widget ↔ Subsystem ↔ AudioComponent) :

```mermaid
sequenceDiagram
    participant W as Widget
    participant S as Subsystem
    participant A as AudioComponent
    W->>S: Node Name(Param)
    S->>A: Action interne
    A-->>S: Résultat
    S-->>W: Delegate / Retour
```

En complément, le format texte flèche peut être utilisé pour les exemples simples inline :

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

## Règles pour le sommaire et les liens internes

Le sommaire utilise des liens internes Markdown vers les ancres des titres. Les ancres doivent suivre les règles de **GitHub Wiki** :

1. Tout en **minuscules**
2. Les espaces deviennent des **tirets** `-`
3. Les caractères accentués sont **conservés tels quels** (`é`, `è`, `ê`, `à` restent)
4. Les caractères spéciaux (parenthèses, virgules, deux-points, points, apostrophes, slashes) sont **supprimés**
5. Les tirets consécutifs sont fusionnés en un seul

**Exemples** :

| Titre | Ancre correcte |
|---|---|
| `## Présentation` | `#présentation` |
| `## Structures de données` | `#structures-de-données` |
| `## Événements (Delegates)` | `#événements-delegates` |
| `## Lecture par nom via DataTable` | `#lecture-par-nom-via-datatable` |
| `## Play SFX By Name` | `#play-sfx-by-name` |
| `## Requêtes d'état` | `#requêtes-détat` |
| `## Sauvegarde et chargement` | `#sauvegarde-et-chargement` |
| `## Exemples de scénarios courants` | `#exemples-de-scénarios-courants` |
| `## Référence rapide des nodes` | `#référence-rapide-des-nodes` |
| `## Formules de calcul` | `#formules-de-calcul` |

> **Attention** : Les sous-sections (`###`) sont aussi liées dans le sommaire quand pertinent. L'ancre suit les mêmes règles. Par exemple `### FSFXData` → `#fsfxdata`.

## Structure du guide

Le guide DOIT contenir les sections suivantes, dans cet ordre. Omets une section uniquement si elle n'est pas pertinente pour le système (ex : pas de MetaSound → pas de section paramètres).

```
# Guide : <Nom du Système>

## Sommaire
(Liste numérotée avec liens internes vers chaque section, en respectant les règles d'ancres GitHub Wiki ci-dessus)

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
(3-5 cas concrets avec diagrammes Mermaid)
- Chaque scénario inclut un diagramme Mermaid (`flowchart` ou `sequenceDiagram`) montrant le flux complet
- Format :

```mermaid
flowchart LR
    A[Événement déclencheur] --> B[Node 1]
    B --> C["Node 2\nParam: valeur"]
    C --> D[Résultat]
```

## Référence rapide des nodes
- Tableau récapitulatif : Catégorie | Node | Type (Callable / Pure / Event)
```

## Règles de rédaction

- **Langue** : Français, avec les termes techniques Unreal en anglais (Blueprint, BeginPlay, node, pin, etc.)
- **Ton** : Pratique, direct, orienté "comment faire". Pas de jargon C++ inutile.
- **Diagrammes Mermaid** : Privilégie les diagrammes Mermaid (`flowchart`, `sequenceDiagram`) pour visualiser les flux Blueprint. Le format texte flèche `→` reste utilisable en complément pour les exemples simples inline. Pas de code C++ sauf dans une section dédiée "Utilisation en C++" si pertinente.
- **Tableaux** : Utilise des tableaux Markdown pour les paramètres, les champs de struct et les références.
- **Accents** : Inclus les accents français normalement (é, è, ê, à, etc.)
- **Expressions mathématiques** : Utilise la syntaxe LaTeX pour les formules (ex : `$V_{final} = V_{base} \times V_{master} \times F_{fade}$`). Blocs `$$...$$` pour les formules centrées, inline `$...$` pour les formules dans le texte.
- **Pas de screenshots** : Le guide est textuel uniquement.
- **Notes et conseils** : Utilise les blockquotes `>` pour les tips et mises en garde.

## Publication sur le wiki GitHub

Le guide doit être publié directement sur le wiki GitHub du projet. Suis cette procédure **dans l'ordre** :

### Étape 1 : Déterminer le repo wiki

Exécute cette commande shell pour obtenir l'URL du wiki à partir du remote `origin` du projet courant :

```bash
WIKI_URL=$(git remote get-url origin | sed 's/\.git$//' | sed 's/$/.wiki.git/')
REPO_NAME=$(basename "$(git remote get-url origin)" .git)
WIKI_DIR="/tmp/${REPO_NAME}.wiki"
echo "Wiki URL: $WIKI_URL"
echo "Wiki dir: $WIKI_DIR"
```

### Étape 2 : Cloner le wiki (si nécessaire)

```bash
if [ ! -d "$WIKI_DIR/.git" ]; then
    git clone "$WIKI_URL" "$WIKI_DIR"
else
    cd "$WIKI_DIR" && git pull
fi
```

### Étape 3 : Déterminer le nom du fichier

Le nom du fichier Markdown suit la convention GitHub Wiki : les espaces sont remplacés par des tirets.

- Le nom de fichier est dérivé du titre `# Guide : <Nom du Système>` → `<Nom-du-Système>.md`
- Exemples : `Sound-Subsystem.md`, `Music-Subsystem.md`, `Scene-Transition-Subsystem.md`

### Étape 4 : Écrire le fichier

Utilise l'outil **Write** pour écrire le contenu Markdown complet dans `$WIKI_DIR/<Nom-du-Fichier>.md`.

> **Important** : Pour obtenir le chemin Windows correct (nécessaire pour l'outil Write), exécute `cd $WIKI_DIR && pwd -W` et utilise le résultat comme préfixe du chemin.

### Étape 5 : Vérifier, commit et push

```bash
cd "$WIKI_DIR"
git diff           # Vérifier les changements
git add <fichier>.md
git commit -m "docs: update/create <Nom du Système> guide"
git push
```

### Étape 6 : Confirmer à l'utilisateur

Affiche un résumé dans le chat :
- Le lien vers la page wiki (format : `https://github.com/<owner>/<repo>/wiki/<Nom-du-Fichier-sans-.md>`)
- Un bref résumé des sections générées/modifiées

## Vérification finale

Avant de commit/push, vérifie que :

- [ ] Chaque `UFUNCTION(BlueprintCallable)` et `BlueprintPure` du header est documentée
- [ ] Chaque `BlueprintAssignable` delegate est listé dans la section Événements
- [ ] Chaque `USTRUCT(BlueprintType)` utilisée est détaillée
- [ ] Les exemples de scénarios correspondent à des cas d'usage réels du projet (Visual Novel, minigames, menus)
- [ ] Le sommaire est complet et les liens internes fonctionnent (selon les règles d'ancres GitHub Wiki)
- [ ] Le fichier a bien été écrit dans le wiki cloné (vérifier avec `git diff`)
