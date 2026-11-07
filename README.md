# ESGI-MiniShell

## Commandes MiniShell

- `pwd`
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
