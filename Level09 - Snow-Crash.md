# Level 09

## 📋 Informations

- **Domaine** : Reverse Engineering / Cryptographie
- **Objectif** : Comprendre l'algorithme de chiffrement d'un binaire pour déchiffrer le fichier `token`.

---

## 🔍 Exploration

Nous trouvons un binaire `level09` (setuid) et un fichier `token`.

```bash
ls -l
```
```text
-rwsr-sr-x+ 1 flag09 level09 8617 Mar  5  2016 level09
-rw-------  1 flag09 flag09    25 Mar  5  2016 token
```

Regardons le contenu du `token` :
```bash
cat -e token
```
```text
f4kmm6p|=M-^B^?pM-^BnM-^CM-^BDBM-^CDu{^?M-^LM-^I$
```
Le fichier contient des caractères non imprimables, ce qui suggère qu'il s'agit de données binaires ou chiffrées.

Testons le binaire `level09` :
```bash
./level09
# You need to provied only one arg.
./level09 aaaaa
# abcde
./level09 11111
# 12345
```

> [!NOTE]
> **Observation**
> Le programme semble appliquer une transformation simple sur l'entrée :
> - `a` (index 0) -> `a` (a+0)
> - `a` (index 1) -> `b` (a+1)
> - `a` (index 2) -> `c` (a+2)
>
> Il s'agit d'un décalage incrémental : `char[i] = char[i] + i`.

---

## 🛠️ Analyse

Une analyse rapide du binaire (`gdb` ou décompilation) confirme cette hypothèse. Voici une version simplifiée de la boucle principale :

```c title:level09_simplified.c
// ...
int i = 0;
while (argv[1][i]) {
    putchar(argv[1][i] + i);
    i++;
}
putchar('\n');
// ...
```

Le fichier `token` a été généré par ce programme. Pour retrouver le flag original, nous devons inverser l'opération.

> [!IMPORTANT]
> **L'inverse**
> Si `Chiffré[i] = Original[i] + i`
> Alors `Original[i] = Chiffré[i] - i`

---

## 🔓 Résolution

### 1. Création du script de décodage

Nous allons écrire un petit programme en C pour effectuer l'opération inverse (soustraction).

```c title:decode.c
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int ac, char** av) {
    if (ac != 2) return 1;

    // Lecture du fichier token
    int fd = open(av[1], O_RDONLY);
    char buf[1024];
    int ret = read(fd, buf, 1024);
    
    // Déchiffrement
    for(int i = 0; i < ret - 1; i++) { // ret - 1 pour ignorer le \n final si présent
        printf("%c", buf[i] - i);
    }
    printf("\n");
    return 0;
}
```

### 2. Compilation et Exécution

Compilons notre outil.
```bash
gcc -std=c99 decode.c -o decode
```

Lançons-le sur le fichier `token`.
```bash
./decode token
```

### 3. Résultat

```text
f3iji1ju5yuevaus41q1afiuq
```

Il s'agit du mot de passe pour `flag09`.
```bash
su flag09
getflag
```
**Flag Final :**
```text
s5cAJpM8ev6XHw998pRWG728z
```

---

## 📚 Notions utiles

- **Reverse Engineering** : Analyse "Boîte noire" (test des entrées/sorties) vs Décompilation.
- **Cryptographie simple** : Les algorithmes basés sur des simples décalages (ROT, XOR simple) sont triviaux à casser.

