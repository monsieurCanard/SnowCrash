# Level 02

## 📋 Informations

- **Domaine** : Analyse Réseau (PCAP & Telnet)
- **Objectif** : Analyser une capture réseau pour retrouver des identifiants.

---

## 🔍 Exploration

Nous disposons d'un fichier `level02.pcap`. C'est un fichier de capture de paquets réseau.
Bien que **Wireshark** soit l'outil graphique de référence, nous pouvons utiliser `tcpdump` en ligne de commande pour une analyse rapide.

```bash
tcpdump -qns 0 -X -r level02.pcap
```

Dans le flux de données, nous repérons des paquets TCP contenant du texte clair.

> [!NOTE]
> **Pourquoi du texte clair ?**
> La capture montre une communication sur le port standard de **Telnet**. Contrairement à SSH, Telnet ne chiffre pas les communications. Tout ce qui est tapé (login, mot de passe) transite en clair sur le réseau.

---

## 🛠️ Analyse

En lisant les payloads des paquets TCP, nous reconstruisons la conversation :

1.  Demande de Login/Password.
2.  L'utilisateur tape un mot de passe.
3.  Le serveur renvoie des acquittements.

**Extrait hexadécimal intéressant :**
```text
66 74 5f 77 61 6e 64 72 7f 7f 7f 4e 44 52 65 6c 4c 30 4c 0a
```

Si on convertit cela en ASCII :
`f t _ w a n d r DEL DEL DEL N D R e l L 0 L \n`

> [!WARNING]
> **Le piège des caractères de contrôle**
> Le caractère hexadécimal `7f` correspond à la touche **DELETE** (Backspace).
> Cela signifie que l'utilisateur a fait une erreur de frappe et a effacé les 3 caractères précédents (`a`, `n`, `d`) pour les corriger.

---

## 🔓 Résolution

Reconstruisons le mot de passe en appliquant les suppressions :

*   Saisie brute : `ft_wandr`
*   3x `DELETE` : on supprime les 3 derniers caractères (`n`, `d`, `r`) -> reste `ft_wa`
*   Suite de la saisie : `NDRelL0L`

**Mot de passe reconstitué :**
```text
ft_waNDRelL0L
```

### Validation
Connexion à l'utilisateur `flag02` :
```bash
su flag02
# Password: ft_waNDRelL0L
getflag
```

**Flag Final :**
```text
kooda2puivaav1idi4f57q8iq
```

---

## 📚 Notions utiles

- **Wireshark / tcpdump** : Outils d'analyse de paquets.
- **Telnet vs SSH** : Importance du chiffrement des flux d'administration.
- **ASCII Table** : Comprendre les codes hexadécimaux et les caractères de contrôle non imprimables.
