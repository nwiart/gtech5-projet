---
description: Crée une Pull Request GitHub pour la branche courante
---

Tu es un assistant de gestion de projet. Ta mission est de créer une **Pull Request** sur GitHub pour la branche courante, avec un titre clair et une description structurée résumant les changements.

## Instructions

### Étape 1 : Collecter le contexte

Exécute ces commandes **en parallèle** pour comprendre l'état de la branche :

1. `git status` — fichiers non suivis et modifications
2. `git log --oneline $(git merge-base HEAD origin/dev)..HEAD` — tous les commits de la branche depuis sa divergence de `dev`
3. `git diff origin/dev...HEAD --stat` — résumé des fichiers modifiés par rapport à `dev`
4. `git diff origin/dev...HEAD` — le diff complet des changements
5. `git branch --show-current` — nom de la branche courante
6. `git remote get-url origin` — URL du repo pour extraire owner/repo

> **Note** : La branche de base par défaut est `dev`. Si l'utilisateur précise une autre branche cible (ex : `main`), utilise-la à la place de `dev` dans toutes les commandes ci-dessus.

### Étape 2 : Analyser les changements

À partir du diff et de l'historique des commits, identifie :

- **Le type de changement** : nouvelle fonctionnalité (feat), correction (fix), refactoring, documentation, etc.
- **Les systèmes impactés** : quels modules / subsystems / classes sont touchés
- **Les changements majeurs** : nouveaux fichiers, modifications d'API, changements d'architecture
- **Les risques potentiels** : breaking changes, dépendances modifiées, comportements altérés

### Étape 3 : Vérifier que la branche est poussée

```bash
git status -sb
```

Si la branche n'est pas à jour avec le remote (ou n'a pas de remote tracking), pousse-la :

```bash
git push -u origin HEAD
```

### Étape 4 : Créer la Pull Request

Utilise `gh pr create` avec le format ci-dessous. Adapte le titre et le contenu en fonction de l'analyse des changements.

#### Conventions de titre

Le titre suit le format : `<type>(<scope>): <description courte>`

- **type** : `feat`, `fix`, `refactor`, `docs`, `chore`, `perf`, `style`, `test`
- **scope** : le système ou module principal impacté (ex : `audio`, `dialogue`, `save`, `minigame`, `map`, `core`, `editor`)
- **description** : en anglais, concise, au présent impératif (ex : "add crossfade support to MusicSubsystem")

#### Format du body

```bash
gh pr create --base dev --title "<titre>" --body "$(cat <<'EOF'
## Résumé

<1-3 bullet points décrivant les changements principaux en français>

## Changements détaillés

<Liste des modifications groupées par système/module>

### <Nom du système 1>
- Description du changement 1
- Description du changement 2

### <Nom du système 2>
- Description du changement

## Impact

- **Fichiers modifiés** : <nombre>
- **Systèmes impactés** : <liste des systèmes>
- **Breaking changes** : <Aucun / Description>

## Test plan

- [ ] Vérifier <scénario de test 1 lié au contexte du projet>
- [ ] Vérifier <scénario de test 2>
- [ ] <Autres vérifications pertinentes>

EOF
)"
```

> **Important** : La branche de base (`--base`) est `dev` par défaut. Utilise une autre branche si l'utilisateur le précise.

### Étape 5 : Confirmer à l'utilisateur

Affiche un résumé dans le chat :

- Le **lien vers la PR** (URL retournée par `gh pr create`)
- Le **titre** de la PR
- La **branche de base** ciblée
- Un **résumé court** des changements inclus

## Règles

- **Langue** : Le titre de la PR est en anglais (convention commits). Le body est en français.
- **Exhaustivité** : Analyse TOUS les commits de la branche, pas seulement le dernier. Le résumé doit refléter l'ensemble des changements.
- **Contexte projet** : Mentionne les concepts spécifiques au projet quand pertinent (MetaSound, PaperZD, subsystems, DataTable, etc.)
- **Pas de secrets** : Ne jamais inclure de fichiers `.env`, credentials, ou tokens dans la PR.
- **Pas de push force** : Ne jamais utiliser `git push --force`.
- **Demander confirmation** : Si des changements non commités sont détectés (`git status`), signale-le à l'utilisateur avant de créer la PR.
