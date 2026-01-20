# Level 04

## 📋 Informations

- **Domaine** : Injection de Commande (Perl / CGI)
- **Objectif** : Exploiter un script Perl vulnérable s'exécutant avec des privilèges élevés.

---

## 🔍 Exploration

Nous trouvons un script Perl `level04` avec le bit **SUID** activé (ou exécutable en tant que `flag04`).
Le script écoute potentiellement sur un port web (mention `localhost:4747` en commentaire), mais peut être exécuté directement en ligne de commande.

### Analyse du Code

```perl
#!/usr/bin/perl
# localhost:4747
use CGI qw{param}; 
print "Content-type: text/html\n\n";

sub x {
  $y = $_[0]; 
  print `echo $y 2>&1`;
}

x(param("x"));
```

---

## 🛠️ Analyse

Le script utilise le module `CGI` pour récupérer un paramètre nommé `x`.
Il passe ensuite ce paramètre à la fonction `x`.

La vulnérabilité se situe ici :
```perl
print `echo $y 2>&1`;
```

> [!IMPORTANT]
> **Dangers des Backticks (`) en Perl**
> En Perl, les chaînes entourées de backticks (`...`) sont exécutées comme des commandes Shell, et le résultat est retourné.
> C'est l'équivalent d'un `system()` ou `popen()` en C, mais plus insidieux car cela ressemble à une simple chaîne de caractères.

Le script construit la commande : `echo [DIRE_UTILISATEUR] 2>&1`.
Il n'y a aucune désinfection (sanitization) de l'entrée `$y`.

---

## 🔓 Résolution

Nous pouvons injecter des caractères spéciaux du Shell pour détourner la commande `echo`.
Nous voulons exécuter `getflag`.

### Méthode par défaut (CLI du module CGI)
Le module Perl CGI permet de passer des arguments en ligne de commande sous la forme `param=valeur`.

Si nous passons `x=|getflag`, la commande exécutée par le Perl sera :
`echo |getflag`

Cela signifie :
1.  `echo` s'exécute (avec un argument vide implicite ou juste coupé).
2.  Le résultat est pipé (`|`) vers...
3.  `getflag`.

### Exécution

```bash
./level04 "x=|getflag"
```
*(Le pipe `|` est ici interprété par le script perl, pas par notre shell actuel, mais pour être sûr, on peut quoter l'argument)*

**Résultat :**
`getflag` est exécuté avec les droits du script (flag04).

**Flag Final :**
```text
ne2searoevaevoem4ov4ar8ap
```

---

## 📚 Notions utiles

- **Perl CGI** : Module standard pour gérer les requêtes web en Perl.
- **Command Injection** : L'utilisation de caractères comme `;`, `|`, `&&`, `$` permet d'enchaîner des commandes.
