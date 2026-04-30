import time
import random
import json
import paho.mqtt.client as mqtt

# ==============================
# CONFIGURACIÓN DEL BROKER
# ==============================

# Selecciona la dirección del broker MQTT
BROKER = "broker.emqx.io"
# BROKER = "mqtt.eclipseprojects.io"  
# BROKER = "broker.hivemq.com"
# BROKER = "mqtt.coreflux.org"

#ip del broker en la red local
# BROKER = "192.168.1.4" 

PUERTO = 1883
TOPIC = "test-topic/to-esp32"  # Topic donde se publicarán los datos

# ==============================
# CREACIÓN DEL CLIENTE MQTT
# ==============================

client = mqtt.Client(client_id="publicador_python")

# ==============================
# CONEXIÓN AL BROKER
# ==============================

print("Conectando al broker MQTT...")
client.connect(BROKER, PUERTO, keepalive=60)

print("Publicador conectado.")
print(f"Publicando datos en el topic: {TOPIC}")

# ==============================
# PUBLICACIÓN DE MENSAJES
# ==============================

try:
    while True:
        # Simulación de datos de sensores
        temperatura = round(random.uniform(20.0, 35.0), 2)
        humedad = round(random.uniform(40.0, 80.0), 2)

        # Mensaje en formato JSON
        mensaje = {
            "estado": "LED_OFF"          
        }

        # Convertir diccionario Python a texto JSON
        mensaje_json = json.dumps(mensaje)

        # Publicar mensaje
        client.publish(TOPIC, mensaje_json)

        print(f"Mensaje publicado: {mensaje_json}")

        # Esperar 5 segundos antes de enviar otro dato
        time.sleep(5)

except KeyboardInterrupt:
    print("\nPublicación detenida por el usuario.")
    client.disconnect()
    print("Cliente desconectado.")