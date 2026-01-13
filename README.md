# ❄️ SnowCrash

 **SnowCrash** est un projet de sécurité informatique de l'école 42. Il s'agit d'un **Capture The Flag (CTF)** introductif conçu pour initier aux concepts fondamentaux de la cyber-sécurité, de l'exploitation système et du reverse engineering.

Le projet se présente sous la forme d'une image ISO à faire tourner dans une machine virtuelle. Le but est de résoudre **15 niveaux** (de 00 à 14), chaque niveau donnant accès au mot de passe du niveau suivant.

---

## 🚀 Liste des Niveaux

Chaque niveau aborde une vulnérabilité ou un concept spécifique.

| Niveau | Domaine | Technique / Vulnérabilité |
| :---: | :--- | :--- |
| **00** | 🔐 Cryptographie | **Caesar Cipher / Rotation**. Identification d'une chaîne encodée et décodage simple. |
| **01** | 🔨 Cracking | **Legacy Password Hash**. Exploitation d'un hash DES (algorithm ID 0) exposé dans `/etc/passwd`. |
| **02** | 📡 Réseau | **Analyse PCAP**. Inspection de paquets réseau (Wireshark/Tcpdump) pour intercepter des identifiants Telnet. |
| **03** | 🐧 Système | **PATH Hijacking**. Exploitation d'un binaire SUID appelant une commande sans chemin absolu. |
| **04** | 🌐 Web / Scripting | **CGI Command Injection**. Exploitation d'un script Perl vulnérable accessible via HTTP. |
| **05** | ⏰ Système | **Privilege Escalation (Cron)**. Exploitation d'une tâche planifiée exécutant des scripts utilisateur aveuglément. |
| **06** | 🐘 Web / PHP | **Code Injection**. Exploitation du modificateur déprécié `/e` (eval) dans `preg_replace` en PHP. |
| **07** | 💉 Injection | **Environment Injection**. Détournement d'une commande `system()` via manipulation de variables d'environnement. |
| **08** | 🔗 Système | **Symlink Attack**. Contournement d'un filtre de nom de fichier via des liens symboliques. |
| **09** | 🔄 Reverse | **Custom Crypto**. Analyse statique/dynamique pour comprendre et inverser un algorithme de hachage artisanal. |
| **10** | ⚡ Race Condition | **TOCTOU (Time of Check Time of Use)**. Exploitation de la latence entre `access()` et `open()`. |
| **11** | 🐍 Scripting | **Blind Command Injection (Lua)**. Injection de commandes dans un `popen` mal protégé sans retour visuel. |
| **12** | 🧠 Bypassing | **Regex / Filter Bypass**. Contournement de filtres complexes (Majuscules/Espaces) via Shell Globbing. |
| **13** | 🕵️ Reverse | **Custom Crypto**. Analyse et réutilisation d'un algorithme de chiffrement personnalisé. |
| **14** | 🛡️ Advanced Reverse | **Anti-Debugging & LD_PRELOAD**. Contournement de protections (`ptrace`) et analyse de structures internes. |

---

## 🛠️ Outils Utilisés

Une variété d'outils ont été utilisés pour résoudre ces challenges :

- **Analyse Statique & Reverse** : `Ghidra`, `GDB`, `objdump`, `ltrace`, `strace`.
- **Réseau & Web** : `Wireshark`, `curl`, `netcat`.
- **Scripting** : `Python`, `Perl`, `Bash`, `Lua`.
- **Cracking** : `John the Ripper`.

## 📚 Documentation

Chaque niveau possède sa propre documentation détaillée dans le dossier `All_Levels/`, expliquant :
1.  **L'exploration** et la découverte de la faille.
2.  **L'analyse** technique du code vulnérable.
3.  **La résolution** pas à pas.
4.  **Les notions** théoriques associées.

---
*Projet réalisé dans un cadre pédagogique.*

