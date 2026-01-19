# Level 13

## 📋 Informations

- **Domaine** : Reverse Engineering / Check de UID
- **Objectif** : Récupérer le token en contournant une vérification d'UID (User ID) hardcodée dans le binaire.

---

## 🔍 Exploration

Nous avons un binaire `level13` SUID `flag13`.
Si nous l'exécutons :

```bash
./level13
```

```text
UID 2013 started us but we we expect 4242
```

Le programme vérifie notre UID et refuse de s'exécuter car nous ne sommes pas l'UID 4242.
Or, nous ne pouvons pas changer notre UID sur cette machine (nous ne sommes pas root).

---

## 🛠️ Analyse

Décompilons le binaire pour voir ce qui se cache derrière cette vérification.

```c title:level13.c (Check UID)
 // ...
 uid_t uid = getuid();
 if (uid != 4242) {
    printf("UID %d started us but we we expect %d\n", uid, 4242);
    exit(1);
 }
 char *token = ft_des("boe]!ai0FB@.:|L6l@A?>qJ}I");
 printf("your token is %s\n", token);
 // ...
```

Le programme calcule le token (via une fonction `ft_des` personnalisée) **seulement** si l'UID est 4242.
Le token est calculé à partir de la chaîne chiffrée `"boe]!ai0FB@.:|L6l@A?>qJ}I"`.

> [!IMPORTANT]
> **Le problème**
> Nous ne pouvons pas tromper `getuid()` sans utiliser `LD_PRELOAD` (qui est bloqué sur les binaires SUID) ou `guppy`/`ptrace` (bloqué aussi).
>
> **Les solutions**
> Nous avons deux approches possibles :
> 1.  **Reverse Engineering (Statique)** : Comprendre l'algo de déchiffrement (`ft_des`) et le réimplémenter nous-mêmes.
> 2.  **Reverse Engineering (Dynamique / Contournement)** : Utiliser un débogueur (`gdb`) pour sauter la vérification de l'UID pendant l'exécution.

---

## 🔓 Résolution

### Méthode 1 : Sauter le check avec GDB
C'est la méthode la plus rapide. Même si le binaire est SUID, nous pouvons l'exécuter dans GDB (il perdra ses droits SUID, mais peu importe, l'algo de chiffrement n'a pas besoin de droits spéciaux, il a juste besoin de s'exécuter).

1.  Lancer gdb : `gdb level13`
2.  Mettre un breakpoint au `main` : `break main`
3.  Lancer : `run`
4.  Désassembler pour trouver le `cmp` de l'UID : `disas`
    *   On cherche une instruction type `cmp ... 0x1092` (4242 en hex).
    *   Suivie d'un `jne` (Jump if Not Equal) qui mène au `exit`.
5.  **Altérer le registre** : Juste après le `getuid()`, nous modifions la valeur du registre `eax` (qui contient le retour de `getuid`) pour qu'il valle 4242.
    *   `set $eax=4242`
6.  Continuer l'exécution : `continue`

Le programme croira que nous sommes l'UID 4242 et affichera le token.


### Résultat dans GDB :

```bash
(gdb) break *0x08048595   <-- Adresse juste après le getuid()
(gdb) run
Breakpoint 1, 0x08048595 in main ()
(gdb) set $eax=4242
(gdb) continue
your token is 2A31L79asukciNyi8uppkEuSx
```

### Méthode 2 : Réimplémentation (Analyse de ft_des)
Si on ne pouvait pas utiliser GDB, on aurait recodé l'algorithme.
L'algorithme `ft_des` n'est pas du vrai DES, c'est un simple chiffrement par rotation alternée.

```c title:decrypt.c
#include <string.h>
#include <stdio.h>

int main() {
  char str[] = "boe]!ai0FB@.:|L6l@A?>qJ}I";
  int i = 0;
  int rot = 0;
  int len = strlen(str);

  while (i < len) {
    if (rot == 6) rot = 0;
    
    // La table de rotation est basée sur les caractères "0123456" 
    // qui sont en fait utilisés comme int (valeur ascii '0'=48, ...) 
    // C'est un peu bizarre mais c'est la logique du binaire.
    // Simplification : ça revient à modifier le char selon sa parité et sa position.
    
    int count = (rot % 2 == 0) ? rot : rot; // Logique simplifiée

    // Note : L'implémentation exacte est plus verbeuse (voir trace décompilée)
    // Mais le plus simple est de copier-coller le code décompilé tel quel.
    // ...
  }
}
```
*(Note : La méthode GDB est beaucoup plus fiable car elle évite les erreurs de réimplémentation d'un algo obfusqué).*

### Méthode 3 : Environnement Contrôlé (VM)
Comme le token est généré algorithmiquement et ne dépend pas d'un fichier externe (le flag n'est pas lu sur le disque, il est calculé), nous pouvons exfiltrer le binaire pour l'exécuter dans un environnement que nous contrôlons.

1.  **Récupérer le binaire** : Copiez `level13` sur votre machine locale.
2.  **Créer l'utilisateur requis** : Sur votre Linux local (ou VM), créez un utilisateur avec l'UID spécifique (4242).
    ```bash
    sudo useradd -u 4242 -m challenge_user
    ```
3.  **Exécuter** :
    ```bash
    # Copier le binaire dans un endroit accessible
    cp level13 /tmp/level13
    chmod +x /tmp/level13
    
    # Lancer en tant que l'utilisateur 4242
    sudo -u challenge_user /tmp/level13
    ```


**Flag Final :**
```text
2A31L79asukciNyi8uppkEuSx
```

---

## 📚 Notions utiles

- **GDB & SUID** : Quand on lance un binaire SUID dans GDB, il s'exécute avec **nos** droits (pas ceux du propriétaire). Ici, c'est parfait car le calcul du token ne dépend pas d'un secret lu dans un fichier protégé, mais d'une constante hardcodée.