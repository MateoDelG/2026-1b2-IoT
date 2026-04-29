import paho.mqtt.client as mqtt

# ==============================
# CONFIGURACIÓN DEL BROKER
# ==============================

BROKER = "localhost"  # Cambia esta IP por la IP del computador donde está Mosquitto
PUERTO = 1883
TOPIC = "test-topic/sensor"  # Topic al que se suscribirá el cliente

# ==============================
# FUNCIÓN CUANDO EL CLIENTE SE CONECTA
# ==============================

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Conectado correctamente al broker MQTT.")
        print(f"Suscrito al topic: {TOPIC}")

        # Suscribirse al topic
        client.subscribe(TOPIC)
    else:
        print(f"Error de conexión. Código: {rc}")

# ==============================
# FUNCIÓN CUANDO LLEGA UN MENSAJE
# ==============================

def on_message(client, userdata, msg):
    print("\nMensaje recibido:")
    print(f"Topic: {msg.topic}")
    print(f"Payload: {msg.payload.decode('utf-8')}")

# ==============================
# CREACIÓN DEL CLIENTE MQTT
# ==============================

client = mqtt.Client(client_id="suscriptor_python")

# Asociar funciones callback
client.on_connect = on_connect
client.on_message = on_message

# ==============================
# CONEXIÓN AL BROKER
# ==============================

print("Conectando al broker MQTT...")
client.connect(BROKER, PUERTO, keepalive=60)

# Mantener el cliente escuchando mensajes
print("Esperando mensajes...")
client.loop_forever()