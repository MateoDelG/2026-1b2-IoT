# Instructivo: configurar Mosquitto en Windows como servicio sin contraseña

## 1. Objetivo

Configurar **Mosquitto** en Windows como **servicio del sistema**, permitiendo conexiones MQTT desde otros dispositivos de la red local sin usar usuario ni contraseña.

Una vez finalizada la configuración, Mosquitto quedará ejecutándose en segundo plano. No será necesario abrir una terminal ni ejecutar manualmente `mosquitto.exe`. Bastará con conectarse al broker usando la dirección IP del computador y el puerto `1883`.

---

## 2. Resultado esperado

Al finalizar este instructivo, el broker MQTT quedará disponible de la siguiente manera:

```text
Broker MQTT: IP_DEL_COMPUTADOR
Puerto: 1883
Usuario: ninguno
Contraseña: ninguna
```

Ejemplo:

```text
Broker MQTT: 192.168.1.25
Puerto: 1883
```

---

## 3. Requisitos

Antes de iniciar, asegúrate de contar con:

* Un computador con Windows.
* Acceso de administrador.
* Conexión a una red local Wi-Fi o Ethernet.
* Instalador de Mosquitto para Windows.
* Permisos para modificar el firewall de Windows.

---

## 4. Instalar Mosquitto

Descarga el instalador de Mosquitto para Windows desde el sitio oficial:

```text
https://mosquitto.org/download/
```

El archivo suele tener un nombre similar a:

```text
mosquitto-2.x.x-install-windows-x64.exe
```

Ejecuta el instalador como administrador y conserva la ruta por defecto:

```text
C:\Program Files\mosquitto
```

Durante la instalación, permite que Mosquitto quede instalado como servicio si el instalador ofrece esa opción.

---

## 5. Abrir PowerShell como administrador

Busca **PowerShell** en el menú de inicio.

Haz clic derecho y selecciona:

```text
Ejecutar como administrador
```

Luego entra a la carpeta de instalación de Mosquitto:

```powershell
cd "C:\Program Files\mosquitto"
```

---

## 6. Verificar si el servicio Mosquitto existe

Ejecuta el siguiente comando:

```powershell
Get-Service mosquitto
```

Si aparece información del servicio, Mosquitto ya está instalado como servicio.

Si aparece un error indicando que el servicio no existe, instálalo con:

```powershell
.\mosquitto.exe install
```

Luego verifica nuevamente:

```powershell
Get-Service mosquitto
```

---

## 7. Detener el servicio antes de configurar

Antes de modificar la configuración, detén Mosquitto:

```powershell
Stop-Service mosquitto
```

También puedes detenerlo desde:

```text
Servicios de Windows → Mosquitto Broker → Detener
```

---

## 8. Editar el archivo de configuración del servicio

Cuando Mosquitto se ejecuta como servicio en Windows, normalmente usa el archivo:

```text
C:\Program Files\mosquitto\mosquitto.conf
```

Abre el archivo con Bloc de notas como administrador:

```powershell
notepad "C:\Program Files\mosquitto\mosquitto.conf"
```

Borra o comenta el contenido anterior y deja la siguiente configuración básica:

```conf
listener 1883 0.0.0.0
allow_anonymous true
connection_messages true
log_type all
```

Guarda el archivo y cierra el Bloc de notas.

---

## 9. Explicación de la configuración

| Línea                      | Función                                                                               |
| -------------------------- | ------------------------------------------------------------------------------------- |
| `listener 1883 0.0.0.0`    | Permite que Mosquitto escuche conexiones MQTT en el puerto `1883` desde la red local. |
| `allow_anonymous true`     | Permite conectarse sin usuario ni contraseña.                                         |
| `connection_messages true` | Registra eventos de conexión y desconexión.                                           |
| `log_type all`             | Activa el registro de eventos para revisar el funcionamiento del broker.              |

---

## 10. Abrir el puerto 1883 en el firewall de Windows

Para que otros dispositivos puedan conectarse al broker, permite el puerto MQTT `1883`.

En PowerShell como administrador, ejecuta:

```powershell
New-NetFirewallRule -DisplayName "Mosquitto MQTT 1883" -Direction Inbound -Action Allow -Protocol TCP -LocalPort 1883 -Profile Private
```

Esto habilita conexiones entrantes al broker MQTT en redes privadas.

---

## 11. Iniciar Mosquitto como servicio

Ejecuta:

```powershell
Start-Service mosquitto
```

Verifica que esté activo:

```powershell
Get-Service mosquitto
```

Debe aparecer algo similar a:

```text
Status   Name        DisplayName
------   ----        -----------
Running  mosquitto   Mosquitto Broker
```

---

## 12. Configurar inicio automático

Para que Mosquitto se inicie automáticamente cada vez que Windows arranque, ejecuta:

```powershell
Set-Service mosquitto -StartupType Automatic
```

Con esto, el broker queda funcionando en segundo plano sin necesidad de abrir terminal ni ejecutar comandos manuales.

---

## 13. Identificar la IP del computador broker

Ejecuta:

```powershell
ipconfig
```

Busca la sección de tu conexión Wi-Fi o Ethernet y ubica:

```text
Dirección IPv4
```

Ejemplo:

```text
192.168.1.25
```

Esa será la dirección del broker MQTT.

Los dispositivos deberán conectarse usando:

```text
Host/Broker: 192.168.1.25
Puerto: 1883
Usuario: vacío
Contraseña: vacía
```

---

## 14. Verificar que Mosquitto escucha conexiones externas

Ejecuta:

```powershell
netstat -ano | findstr :1883
```

Una salida correcta puede verse así:

```text
TCP    0.0.0.0:1883    0.0.0.0:0    LISTENING
```

Esto indica que Mosquitto está escuchando conexiones desde la red local, no solamente desde `localhost`.

---

## 15. Probar conexión desde otro dispositivo

Desde otro computador, aplicación MQTT o ESP32 conectado a la misma red, usa estos datos:

```text
Broker: 192.168.1.25
Puerto: 1883
Usuario: ninguno
Contraseña: ninguna
```

Ejemplo de topic:

```text
sensor/temperatura
```

Ejemplo de mensaje:

```text
Temperatura: 27.5 C
```

---

## 16. Ejemplo de conexión desde consola MQTT

Desde otro computador con Mosquitto instalado, puedes suscribirte así:

```powershell
mosquitto_sub -h 192.168.1.25 -p 1883 -t sensor/temperatura
```

Y publicar así:

```powershell
mosquitto_pub -h 192.168.1.25 -p 1883 -t sensor/temperatura -m "Temperatura: 27.5 C"
```

No se usan parámetros `-u` ni `-P` porque el broker quedó configurado sin usuario ni contraseña.

---

## 17. Reiniciar el servicio después de cambios

Cada vez que modifiques el archivo:

```text
C:\Program Files\mosquitto\mosquitto.conf
```

reinicia el servicio:

```powershell
Restart-Service mosquitto
```

---

## 18. Verificación final

El sistema queda correctamente configurado si se cumplen estas condiciones:

| Verificación       | Resultado esperado                                            |
| ------------------ | ------------------------------------------------------------- |
| Servicio Mosquitto | `Running`                                                     |
| Inicio automático  | `Automatic`                                                   |
| Puerto abierto     | `1883/TCP`                                                    |
| Escucha de red     | `0.0.0.0:1883 LISTENING`                                      |
| Autenticación      | No solicita usuario ni contraseña                             |
| Conexión externa   | Otro dispositivo puede conectarse usando la IP del computador |

---

## 19. Problemas frecuentes

| Problema                     | Posible causa                  | Solución                                        |
| ---------------------------- | ------------------------------ | ----------------------------------------------- |
| Solo conecta con `localhost` | Falta `listener 1883 0.0.0.0`. | Revisar `mosquitto.conf`.                       |
| Otro equipo no conecta       | Firewall bloquea el puerto.    | Crear la regla para `1883/TCP`.                 |
| El servicio no inicia        | Error en `mosquitto.conf`.     | Revisar sintaxis del archivo.                   |
| No aparece el servicio       | No se instaló como servicio.   | Ejecutar `.\mosquitto.exe install`.             |
| No llegan mensajes           | Los topics no coinciden.       | Usar el mismo topic en publicador y suscriptor. |

---

## 20. Recomendación de seguridad

Esta configuración es adecuada para una **red local controlada**, como un laboratorio o una clase.

Al usar:

```conf
allow_anonymous true
```

cualquier dispositivo conectado a la misma red puede publicar o suscribirse al broker.

No se recomienda exponer este broker a Internet. Para proyectos reales se recomienda usar:

* Usuario y contraseña.
* Control de acceso por topics.
* Cifrado TLS.
* Red privada o VPN.

---

## 21. Cierre

Después de completar este instructivo, Mosquitto quedará funcionando como un broker MQTT permanente en Windows. Los dispositivos IoT, como un ESP32, podrán conectarse directamente usando la IP del computador y el puerto `1883`, sin necesidad de ejecutar comandos manuales cada vez que se quiera usar el broker.
