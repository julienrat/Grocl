# Fichiers SPIFFS pour PlatformIO

Ce dossier contient les fichiers destinés à être uploadés dans la SPIFFS de l'ESP (PlatformIO `data/` convention).

Fichier ajouté pour les données du vent : `wind.json`

Usage rapide :

- Copiez le contenu JSON fourni dans `wind.json` et collez-le dans ce fichier.
- Utilisez l'outil PlatformIO "Upload File System Image" pour écrire ces fichiers dans la SPIFFS.

Exemple JSON (à copier-coller dans `data/wind.json`) :

```
[
	{
		"timestamp": "2025-10-21T12:00:00Z",
		"speed_m_s": 5.4,
		"direction_deg": 135,
		"gust_m_s": 7.2,
		"source": "anemometer-1",
		"location": "roof",
		"notes": "Mesure horaire moyenne"
	},
	{
		"timestamp": "2025-10-21T12:10:00Z",
		"speed_m_s": 6.1,
		"direction_deg": 140,
		"gust_m_s": 8.0,
		"source": "anemometer-1",
		"location": "roof",
		"notes": "Rafale après rafale"
	}
]
```

Format recommandé :

- Un tableau JSON d'objets. Chaque objet peut contenir au moins :
	- `timestamp` (ISO 8601 UTC)
	- `speed_m_s` (vitesse moyenne en m/s)
	- `direction_deg` (direction en degrés, 0=Nord)
	- `gust_m_s` (rafale maximale en m/s, optionnel)
	- `source` (identifiant du capteur)
	- `location` (emplacement du capteur)
	- `notes` (texte libre, optionnel)

Si vous souhaitez un autre format (par exemple série temporelle compacte ou objets imbriqués), dites-moi lequel et j'adapterai l'exemple.
