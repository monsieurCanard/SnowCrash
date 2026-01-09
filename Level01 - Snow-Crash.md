# Level 01

## 📋 Informations

- **Domaine** : Password Cracking (Legacy Hash)
- **Objectif** : Retrouver le mot de passe de l'utilisateur `flag01` stocké dans un fichier système.

---

## 🔍 Exploration

Une pratique courante consiste à vérifier le fichier `/etc/passwd`. Historiquement, il contenait les mots de passe, mais aujourd'hui, ils sont généralement dans `/etc/shadow`. Cependant, des erreurs de configuration arrivent.

```bash
cat /etc/passwd
```

**Extrait intéressant :**
```text
flag01:42hDRfypTqqnw:3001:3001::/home/flag/flag01:/bin/bash
```

> [!IMPORTANT]
> **Structure de /etc/passwd**
> `user:password:uid:gid:comment:home:shell`
> Ici, le champ mot de passe n'est pas `x` (qui renvoie au shadow), mais `42hDRfypTqqnw`. C'est un hash **DES** (identifiable à sa longueur et son format).

> [!NOTE]
> **Le Hash DES (Data Encryption Standard)**
> Le chiffrement DES via `crypt(3)` est l'algorithme historique de stockage des mots de passe Unix.
> Il produit une chaîne de **13 caractères** ASCII :
> - Les **2 premiers caractères** constituent le **sel** (salt).
> - Les **11 suivants** sont le résultat du chiffrement.
> Cet algorithme est obsolète car il limite le mot de passe à 8 caractères et est vulnérable aux attaques par force brute modernes.

---

## 🛠️ Analyse

Nous avons un hash `42hDRfypTqqnw`.
Nous devons le casser (brute-force) pour retrouver le mot de passe en clair.

L'outil **John the Ripper** est parfait pour cela.

---

## 🔓 Résolution

### Attaque par dictionnaire

Créons un fichier contenant ce hash :
```bash
echo "42hDRfypTqqnw" > pass_flag01.txt
```

Lançons John the Ripper :
```bash
john pass_flag01.txt --show
```

**Sortie de John :**
```text
abcdefg:42hDRfypTqqnw:3001:3001::/home/flag/flag01:/bin/bash
```

Le mot de passe trouvé est : `abcdefg`

### Validation

Connexion à l'utilisateur `flag01` :
```bash
su flag01
# Password: abcdefg
getflag
```

**Flag Final :**
```text
f2av5il02puano7naaf6adaaf
```

---

## 📚 Notions utiles

- **John the Ripper** : Outil de cassage de mots de passe hors-ligne.
- **Crypt(3) DES** : Ancien algorithme de hachage Unix, très faible et vulnérable aujourd'hui.

