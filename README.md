### Disclaimer:
Se usó [LittleFS](https://github.com/littlefs-project/littlefs) para manejar el sistema de archivos (Y subir directamente el csv de datos al ESP32...), esto fue beneficioso por dos razones, una, para garantizar que el servidor gaste la menor cantidad de recursos, asegurando además que los relés no perdieran voltaje VIN en cada cambio, y dos, para manejar consistentemente los datos desde dentro del ESP32 al momento de desplegar el servidor.
El unico contra que tuvo el usar este método de deploy fue usar un servidor hosteado desde un WiFi conocido, el cual se debió ingresar previamente en el código para sincronizar la IP del servidor.

En éste proyecto se implementó una web HTML básica, se espera que la mayor atención recaiga en el artefacto en cuestión, que en la visualización de la plataforma de "selección de país".
