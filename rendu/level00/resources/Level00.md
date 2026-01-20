# Level 00

## 📋 Informations

- **Domaine** : Cryptographie (Caesar Cipher / Rotations)
- **Objectif** : Trouver le mot de passe pour l'utilisateur `flag00`.

---

## 🔍 Exploration

Nous recherchons des fichiers potentiellement oubliés ou liés au flag. Une méthode efficace consiste à lister tout le système et filtrer :

```bash
ls -lRa / | grep flag00
```

Dans le flux de sortie, une entrée intéressante apparaît : `john`.
Pour retrouver son chemin absolu proprement (ou via `find / -user flag00`) :

```bash
find / -name john 2>/dev/null
```

**Résultat :**
```text
/usr/sbin/john
```

Le fichier **`john`** a été localisé. Vérifions son contenu.

---

## 🛠️ Analyse

Lecture du fichier trouvé :
```bash
cat /usr/sbin/john
```

**Contenu :**
```text
cdiiddwpgswtgt
```

La chaîne de caractères ressemble à un mot de passe, mais elle est illisible. L'absence de chiffres ou de symboles spéciaux suggère un chiffrement simple par substitution alphabétique.

---

## 🔓 Résolution

### Cryptanalyse
Il s'agit sans doute d'un **chiffrement de César** (rotation de l'alphabet).
Avec une rotation de **15** caractères (ROT15), la chaîne devient lisible.

Via [dCode - Chiffre de César](https://www.dcode.fr/chiffre-cesar) on obtient :

**Résultat (Mot de passe) :**
```text
nottoohardhere
```

### Validation
Connexion à l'utilisateur `flag00` avec ce mot de passe :
```bash
su flag00
getflag
```

**Flag Final :**
```text
x24ti5gi3x0ol2eh4esiuxias
```
