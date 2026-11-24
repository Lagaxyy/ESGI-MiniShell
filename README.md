# ESGI-MiniShell

## Commandes MiniShell

- `pwd`
- `touch`
- `ls`
- `cd`
- `mission`, `submit`
- `help` pour afficher toutes les commandes
- `quit`, `exit`, `bye` pour quitter le MiniShell

## Ajouter une commande

Ajouter:
- le fichier .c dans `src/commands/`
- le prototype de la fonction dans `src/commands.h`
- les métadonnées de la commande dans `src/commands_handler.c`

```diff
+ src/commands/exemple.c
+   int cmd_exemple(Shell *shell, int argc, char **argv) { ... }

  src/commands.h
+   int cmd_exemple(Shell *sh, int argc, char **argv);

  src/commands_handler.c
      static const t_command g_builtins[] = {
          ...
+         { "exemple", "description" cmd_date, "comment utiliser"},
          { NULL, NULL, NULL}
      };
```

## Système de mission

Dossier de missions localisé dans `missions/`.

Chaque mission doit avoir (`[mission]` est le nom de la mission):
- `missions/[aventure]/[mission]/[mission].mission`
	- `start [cwd|none]`: préciser un dossier à mettre en dossier actuel avec `cwd`, ou laisser la racine avec `none`
	- `expect [réponse|none]`: a besoin de `submit [réponse]` pour valider la mission, si pas besoin alors `none`
	- `tree [nom|none]`: `nom` est le nom du fichier avec l'arborescence de comparaison pour valider la mission, `none` si pas besoin de vérification de l'arborescence
	- `next [prochaine mission]`: `prochaine mission` est le nom de la mission suivante
- `missions/[aventure]/[mission]/[mission].starter.tree` qui correspond à l'arborescence de départ, à charger à chaque début de mission
- `missions/[aventure]/[mission]/[mission].txt` qui correspond au message à afficher à chaque début de mission
- `missions/[aventure]/[mission]/[mission].expect.tree` est optionnel

Le fichier `missions/suivi.mission` permet de garder la dernière mission. Pour changer l'aventure à lancer, mettre à jour ce fichier avec `suivi [nom de l'aventure] [nom de la première mission]` à la deuxième ligne.

## Makefile

```sh
# Compile le projet dans build/
make

# Lancer les tests unitaires
make test-unit

# Supprimer les fichiers de compilation et tests
make fclean

# Nettoyer et recompiler
make re

# Voir toutes les commandes
make help
```

## Others

*Codes couleurs terminal*
  - Bleu gras : "\033[1;34m<texte>\033[0m\n"
