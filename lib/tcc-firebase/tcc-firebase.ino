
/**
 * Created by Yago Oliveira
 *
 * Email: yagoliveira92@gmail.com
 *
 * Github: https://github.com/mobizt/Firebase-ESP-Client
 *
 *
 */

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <Firebase_ESP_Client.h>

// Fornece as informações do processo de geração de token.
#include <addons/TokenHelper.h>

// Fornece as informações de impressão da carga útil do RTDB e outras funções auxiliares.
#include <addons/RTDBHelper.h>

/* 1. Define as credenciais de acesso do WiFi */
#define WIFI_SSID "CLARO_2GCB44AC"
#define WIFI_PASSWORD "13@Ca0059740$@"

/* 2. Define a API Key do projeto no Firebase */
#define API_KEY "AIzaSyAh2INViHJh0_YgXS2StTXpz9oo2nHyZcM"

/* 3. Define a URL para o RTDB (Real Time Database) */
#define DATABASE_URL "openponic-mvp-default-rtdb.firebaseio.com"

/* 4. Define o usuário e senha para acesso ao projeto do Firebase */
#define USER_EMAIL "contato@yago.dev.br"
#define USER_PASSWORD "1qaz@WSX"

// Define o objeto (estruct) do Firebase
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

void setup()
{

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Atribuir a chave de API (obrigatório) */
  config.api_key = API_KEY;

  /* Atribui as credenciais de acesso do usuário*/
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Atribui a URL do RTDB (obrigatório) */
  config.database_url = DATABASE_URL;

  /* Atribua a função de retorno de chamada para a tarefa de geração de token de longa duração */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  //////////////////////////////////////////////////////////////////////////////////////////////
  // Certifique-se de que o heap livre do dispositivo não seja inferior a 80 k para ESP32 e 10 k para ESP8266,
  // caso contrário, a conexão SSL falhará.
  //////////////////////////////////////////////////////////////////////////////////////////////

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);
}

void loop()
{

  // Firebase.ready() deve ser chamado repetidamente para lidar com tarefas de autenticação.

  if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();
    FirebaseJson json;
    FirebaseJson jsonTeste;
    FirebaseJson jsonPh;
    FirebaseJson jsonTemperatura;
    FirebaseJson jsonCondutividade;
    FirebaseJson jsonUmidade;
    FirebaseJsonArray arr;
    FirebaseJsonArray dataArr;

    json.setJsonData("{\"nome\":\"Canteiro 001\"}");
    jsonPh.setJsonData("{\"nome\": \"ph\",\"status\":\"bom\",\"value\":"+String(randomNumber(0.0,7.0))+"}");
    jsonTemperatura.setJsonData("{\"nome\": \"temperatura\",\"status\":\"bom\",\"value\":"+String(randomNumber(0.0,45.0))+"}");
    jsonUmidade.setJsonData("{\"nome\": \"umidade\",\"status\":\"bom\",\"value\":"+String(randomNumber(0.0,100.0))+"}");
    jsonCondutividade.setJsonData("{\"nome\": \"condutividade\",\"status\":\"bom\",\"value\":"+String(randomNumber(5.0,1000.0))+"}");

    dataArr.add(jsonPh).add(jsonTemperatura).add(jsonUmidade).add(jsonCondutividade);
    json.set("data", dataArr);
    arr.add(json);
    Serial.printf("Set Json... %s\n", Firebase.RTDB.set(&fbdo, F("/"), &arr) ? "ok" : fbdo.errorReason().c_str());
  }
}

float randomNumber(float lower, float upper)
{
  srand((unsigned int)time(NULL));

  float scale = rand() / (float)RAND_MAX;
  return lower + scale * (upper - lower);
}
