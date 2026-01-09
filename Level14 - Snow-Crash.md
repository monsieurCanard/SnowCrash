# Level 14

## 📋 Informations

- **Domaine** : Reverse Engineering / Anti-Debugging
- **Objectif** : Analyser le binaire `getflag` lui-même pour trouver le token final.

---

## 🔍 Exploration

Pour ce dernier niveau, il n'y a pas de fichier ou de binaire spécifique au level14 dans le home.
L'objectif est de s'attaquer au binaire `/bin/getflag` lui-même (ou `/usr/bin/getflag`).

Comme nous n'avons pas de droits spéciaux sur ce binaire pour le moment, nous allons le copier chez nous pour l'analyser.

```bash
scp -P 7777 level14@localhost:/bin/getflag .
```

---

## 🛠️ Analyse

Décompilons ce binaire pour comprendre comment il délivre les flags.

```c title:getflag.c (Extrait)
int main() {
    // 1. Protection Anti-Debug (Ptrace)
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) < 0) {
        puts("You should not reverse this");
        return 1;
    }

    // 2. Protection Anti-Injections (LD_PRELOAD)
    if (getenv("LD_PRELOAD") != NULL) {
        // ... exit
    }

    // 3. Délivrance du flag
    printf("Check flag.Here is your token : ");
    uid_t uid = getuid();
    
    // Une série de if/else if pour chaque UID (flag00, flag01...)
    if (uid == 3000) { /* ... */ }
    // ...
    else if (uid == 3014) { // UID de flag14 (0xbc6)
        char *token = ft_des("g <t61:|4_|!@IF.-62FH&G~DCK/Ekrvvdwz?v|");
        fputs(token, stdout);
    }
    // ...
}
```

> [!NOTE]
> **Anti-Debugging `ptrace`**
> La fonction `ptrace(PTRACE_TRACEME, ...)` est utilisée par les processus pour dire "je veux être tracé par mon père (debugger)".
> Si le processus est déjà tracé (par gdb par exemple), cet appel échoue et renvoie -1.
> C'est une technique classique pour détecter si on est lancé dans `gdb`.

> [!IMPORTANT]
> **La découverte**
> Le binaire contient tous les tokens chiffrés pour tous les niveaux.
> Il utilise la même fonction `ft_des` que le **Level 13** pour les déchiffrer.
> L'UID pour le user `flag14` est `3014` (ou `0xbc6` en hexadécimal).
> La chaîne chiffrée pour ce niveau est `"g <t61:|4_|!@IF.-62FH&G~DCK/Ekrvvdwz?v|"`.

---

## 🔓 Résolution

### Méthode 1 : Réutilisation de l'algorithme (Statique)
Puisque nous avons le code de déchiffrement (provenant de notre analyse du Level 13) et la chaîne chiffrée, nous pouvons simplement calculer le flag.

```c title:decrypt_level14.c
#include <string.h>
#include <stdio.h>

int main() {
    // Chaîne extraite du binaire getflag pour l'UID 3014
    char str[] = "g <t61:|4_|!@IF.-62FH&G~DCK/Ekrvvdwz?v|";
    int i = 0;
    int rot = 0;
    int len = strlen(str);

    while (i < len) {
        if (rot == 6) rot = 0;

        // Même algo que Level 13
        if (i % 2 == 0) {
            for (int j = 0; j < "0123456"[rot]; j++) {
                str[i] = str[i] - 1;
                if (str[i] == '\x1f') str[i] = '~'; // Gestion du wrap-around
            }
        } else {
            for (int j = 0; j < "0123456"[rot]; j++) {
                str[i] = str[i] + 1;
                if (str[i] == '\x7f') str[i] = ' '; // Gestion du wrap-around
            }
        }
        rot++;
        i++;
    }

    printf("Flag: %s\n", str);
    return 0;
}
```

**Compilation et exécution :**
```bash
gcc decrypt_level14.c -o decrypt
./decrypt
```

**Flag Final :**
```text
7QiHafiNa3HVozsaXkawuYrTstxbpABHD8CPnHJ
```

---
