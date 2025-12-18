# rfid_FINAL_PER_LCD.py  EL MÉS BRUTAL I NET QUE VEURÀS MAI
import ssl, json, paho.mqtt.client as mqtt
from datetime import datetime
import mysql.connector

# ===================== CONFIG =====================
ENDPOINT   = "a1ti8lv2xwpr17-ats.iot.us-east-1.amazonaws.com"
CERT_PATH  = "/home/isard/certs/device.pem.crt"
KEY_PATH   = "/home/isard/certs/private.pem.key"
CA_PATH    = "/home/isard/certs/AmazonRootCA1.pem"
CLIENT_ID  = "control-assistencia-LCD"

TOPIC_ENTRADA  = "iticbcn/espnode01/pub"
TOPIC_RESPOSTA = "iticbcn/espnode01/sub"
ID_DISPOSITIU = 1

DB_CONFIG = {
    "host": "localhost", "database": "control_assistencia",
    "user": "rfid_user", "password": "MySql_2025_Secret!"
}

# ===================== COLORS (perquè sigui més guapo) =====================
VERD = "\033[92m"
VERMELL = "\033[91m"
BLAU = "\033[94m"
GROC = "\033[93m"
RESET = "\033[0m"

# ===================== ENVIAR RESPOSTA AMB NOM A LA LCD =====================
def envia_resposta(accio, nom="", cognoms=""):
    if nom and cognoms:
        payload = json.dumps({"action": accio, "nom": nom, "cognoms": cognoms})
    else:
        payload = json.dumps({"action": accio})
    client.publish(TOPIC_RESPOSTA, payload)
    print(f"{BLAU}→ Enviat a ESP32: {payload}{RESET}")

# ===================== BD =====================
def get_user(tag):
    conn = mysql.connector.connect(**DB_CONFIG)
    cur = conn.cursor(dictionary=True)
    cur.execute("SELECT id_usuari, nom, cognoms, tipus_usuari FROM Usuari WHERE id_targeta = %s AND actiu = TRUE", (tag,))
    user = cur.fetchone()
    cur.close(); conn.close()
    return user

def te_permis(id_usuari):
    conn = mysql.connector.connect(**DB_CONFIG)
    cur = conn.cursor()
    cur.execute("""SELECT p.permès FROM PermisZona p
                   JOIN Usuari u ON p.tipus_usuari = u.tipus_usuari
                   JOIN Dispositiu d ON p.id_zona = d.id_zona
                   WHERE u.id_usuari = %s AND d.id_dispositiu = %s""", (id_usuari, ID_DISPOSITIU))
    r = cur.fetchone()
    cur.close(); conn.close()
    return r[0] if r else False

def ultima_accio(id_usuari):
    conn = mysql.connector.connect(**DB_CONFIG)
    cur = conn.cursor()
    cur.execute("""SELECT tipus_registre FROM Assistencia 
                   WHERE id_usuari = %s AND id_dispositiu = %s 
                   ORDER BY data_hora DESC LIMIT 1""", (id_usuari, ID_DISPOSITIU))
    r = cur.fetchone()
    cur.close(); conn.close()
    return r[0] if r else None

def registrar(id_usuari, tipus):
    conn = mysql.connector.connect(**DB_CONFIG)
    cur = conn.cursor()
    cur.execute("""INSERT INTO Assistencia (id_usuari, id_dispositiu, data_hora, tipus_registre)
                   VALUES (%s, %s, NOW(), %s)""", (id_usuari, ID_DISPOSITIU, tipus))
    conn.commit()
    cur.close(); conn.close()

# ===================== MQTT =====================
def on_connect(client, userdata, flags, rc, properties):
    print(f"{GROC}SISTEMA ITIC BARCELONA AMB LCD ONLINE I ARMADO{RESET}")
    client.subscribe(TOPIC_ENTRADA)

def on_message(client, userdata, msg):
    try:
        data = json.loads(msg.payload.decode())
        tag = data.get("tag")
        if not tag: return

        print(f"\n{'='*60}")
        print(f"{BLAU}TARGETA → {tag.upper()} | {datetime.now().strftime('%H:%M:%S')}{RESET}")

        user = get_user(tag)
        if not user:
            envia_resposta("denegat")
            print(f"{VERMELL}TARGETA NO REGISTRADA → ACCÉS DENEGAT{RESET}")
            return

        print(f"{VERD}USUARI: {user['nom']} {user['cognoms']} ({user['tipus_usuari']}){RESET}")

        if not te_permis(user['id_usuari']):
            envia_resposta("denegat")
            print(f"{VERMELL}ACCÉS DENEGAT A AQUESTA ZONA{RESET}")
            return

        tipus = "sortida" if ultima_accio(user['id_usuari']) == "entrada" else "entrada"
        registrar(user['id_usuari'], tipus)
        envia_resposta("ok", user['nom'], user['cognoms'])
        print(f"{VERD}{tipus.upper()} REGISTRADA → NOM ENVIAT A LA LCD{RESET}")
        print(f"{'='*60}")

    except Exception as e:
        print(f"{VERMELL}ERROR: {e}{RESET}")

# ===================== INICI =====================
client = mqtt.Client(client_id=CLIENT_ID, callback_api_version=mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.tls_set(ca_certs=CA_PATH, certfile=CERT_PATH, keyfile=KEY_PATH, tls_version=ssl.PROTOCOL_TLSv1_2)
client.connect(ENDPOINT, 8883, keepalive=60)
print(f"{GROC}SERVIDOR LCD ACTIU – ESPERANT TARGETES...{RESET}")
client.loop_forever()