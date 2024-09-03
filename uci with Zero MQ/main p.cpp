#include <zmq.hpp>  // ZeroMQ
#include <uuid/uuid.h>  // UUID
#include "tinyxml2.h"  // TinyXML-2
#include <openssl/evp.h>  // OpenSSL
#include <openssl/rand.h>
#include <openssl/err.h>
#include <libgpsmm.h>  // libgps
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>
#include <set>

#define RED     "\033[31m" 
#define BLUE    "\033[34m" 
#define YELLOW  "\033[33m" 
#define WHITE   "\033[37m"  

using namespace std;

char option = 'r';
string brokerURI = "";
string topicName = "";
string clientID = "";
string subscriptionName = "";
string username = "";
string password = "";
bool stateReading = true;
string filenameMessage = "";
string key = ""; // Clave de 256 bits
string iv = ""; // IV de 128 bits

mutex optionMutex;

set<string> uuidToFilter = {};

void sendMessage(zmq::socket_t& socket);
void readMessage(zmq::socket_t& socket);
void userInput(zmq::socket_t& socket);
bool fileXmlExists(const std::string& filename);
void createXMLFile(const std::string& filename);
string generateUUID();
bool loadXMLFile(const std::string& filename);
void saveMessageToXML(const std::string& filename, const std::string& uuid, const std::string& dateNow, const std::string& latitudeLongitude, const std::string& messageText);
string getCurrentDateTime();
bool getLatitudeLongitude(double &latitude, double &longitude);
string encryptMessage(const string &plaintext, const string &key, const string &iv);
string decryptMessage(const string &ciphertext, const string &key, const string &iv);

int main() {
    if (loadXMLFile("Conf.xml")) {
        cout << WHITE << "\nConfiguration file loaded successfully.\nUser: " << username << endl;
    } else {
        cerr << RED << "Error loading XML conf file." << endl;
    }

    if (!fileXmlExists(filenameMessage)) {
        createXMLFile(filenameMessage);
    }

    // se crea para manejar la comunicacion
    zmq::context_t context(1);
    zmq::socket_t pub_socket(context, ZMQ_PUB);
    zmq::socket_t sub_socket(context, ZMQ_SUB);


    try {
        // Configurar el socket de publicación
        pub_socket.bind(brokerURI);

        // Configurar el socket de suscripción
        sub_socket.set(zmq::sockopt::subscribe, topicName); // Suscribirse al tema específico
        sub_socket.connect(brokerURI);

        // Configurar hilos para manejar entrada del usuario y lectura de mensajes
        thread inputThread(userInput, std::ref(pub_socket));
        thread readThread([&]() {
            while (option != 'e') {
                readMessage(sub_socket);
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Pausa para evitar uso intensivo de CPU
            }
        });

        inputThread.join();
        readThread.join();
    } catch (const zmq::error_t& e) {
        cerr << RED << "ZeroMQ Exception occurred: " << e.what() << endl;
    }

    return 0;
}
void sendMessage(zmq::socket_t& socket) {
    string uuid = generateUUID();
    string messageText;
    string originalMessageText;
    string latitudeLongitude;
    cout << BLUE << "\nEnter the message to send: ";
    cin.ignore();
    getline(cin, messageText);

    originalMessageText = messageText;
    string dateNow = getCurrentDateTime();
    double latitude = 0.0, longitude = 0.0;

    getLatitudeLongitude(latitude, longitude);
    latitudeLongitude = to_string(latitude) + "," + to_string(longitude);
    messageText = uuid + ": " + messageText + " date: " + dateNow + " latitude and longitude: " + latitudeLongitude;

    string eMessage = encryptMessage(messageText, key, iv);
    zmq::message_t zmqMessage(eMessage.c_str(), eMessage.size());

    try {
        // ENVIAR EL MENSAJE
        socket.send(zmqMessage, zmq::send_flags::none);
        uuidToFilter.insert(uuid);
        saveMessageToXML(filenameMessage, uuid, dateNow, latitudeLongitude, originalMessageText);
    } catch (const zmq::error_t& e) {
        cerr << "Exception occurred while sending message: " << e.what() << endl;
    }

    stateReading = true;
}

void readMessage(zmq::socket_t& socket) {
    zmq::message_t zmqMessage;
    
    // Intentar recibir un mensaje en modo no bloqueante
    zmq::recv_result_t result = socket.recv(zmqMessage, zmq::recv_flags::dontwait);

    // Verificar si el mensaje se recibió correctamente
    if (result.has_value()) {
        // Convertir el mensaje a una cadena
        std::string messageText(static_cast<char*>(zmqMessage.data()), zmqMessage.size());
        // Mostrar el mensaje en la consola
        std::cout << "Received message: " << messageText << std::endl;
    }
}




void userInput(zmq::socket_t& socket) {
    while (true) {
        cout << BLUE << "--------------------Options-------------------- \ns. Send a Message \ne. Exit \n\nReading Messages..." << endl;
        cin >> option;
        lock_guard<mutex> lock(optionMutex);

        if (option == 'e') {
            break;
        }

        if (option == 's') {
            cout << WHITE << "\nSending a Message..." << endl;
            sendMessage(socket);
        } else {
            cerr << RED << "\nInvalid option." << endl;
        }
    }
}

// Resto de las funciones (generateUUID, createXMLFile, etc.) se mantienen igual
string generateUUID() {
    uuid_t id;
    uuid_generate(id);  // Generar un UUID

    char strUUID[37];       // Array para almacenar el UUID como cadena (36 + 1 para '\0')
    uuid_unparse(id, strUUID);  // Convertir el UUID a una cadena

    return std::string(strUUID); // Devolver como std::string
}

bool fileXmlExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

void createXMLFile(const std::string& filename) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* rootdoc = doc.NewElement("Messages");
    doc.InsertFirstChild(rootdoc);
    
    // Agrega un elemento inicial si es necesario
    tinyxml2::XMLElement* message = doc.NewElement("Message");
    message->SetAttribute("uuid", "example-uuid");
    message->SetText("This is a test message.");
    rootdoc->InsertEndChild(message);
    
    // Guardar el archivo
    doc.SaveFile(filename.c_str());
    cout<<WHITE << "\nxml file of messages successfully created"<<endl;
}



bool loadXMLFile(const std::string& filename) {
    // Crear un objeto XMLDocument
    tinyxml2::XMLDocument doc;

    // Cargar el archivo XML
    tinyxml2::XMLError eResult = doc.LoadFile(filename.c_str());
    if (eResult != tinyxml2::XML_SUCCESS) {
        return false;
    }

    // Obtener el elemento raíz
    tinyxml2::XMLElement* root = doc.FirstChildElement("Conf");
    if (root) {
        tinyxml2::XMLElement* property = root->FirstChildElement("Properties");
        while (property) {
            const char* type = property->Attribute("type");
            const char* value = property->GetText();

            if (type && value) {
                if (std::strcmp(type, "brokerURI") == 0) {
                    brokerURI = value;
                } else if (std::strcmp(type, "topicName") == 0) {
                    topicName = value;
                } else if (std::strcmp(type, "clientID") == 0) {
                    clientID = value;
                } else if (std::strcmp(type, "subscriptionName") == 0) {
                    subscriptionName = value;
                } else if (std::strcmp(type, "username") == 0) {
                    username = value;
                } else if (std::strcmp(type, "password") == 0) {
                    password = value;
                } else if (std::strcmp(type, "filenameMessage") == 0) {
                    filenameMessage = value;
                }else if(std::strcmp(type,"key-openssl") == 0){
                    key = value;
                }else if(std::strcmp(type,"iv-openssl") == 0){
                    iv = value;
                }
            }

            // Ir al siguiente elemento "Properties"
            property = property->NextSiblingElement("Properties");
        }

        return true;

    } else {
        return false;
    }
}



void saveMessageToXML(const std::string& filename, const std::string& uuid,const std::string& dateNow,const std::string& latitudeLongitude, const std::string& messageText) {
    tinyxml2::XMLDocument doc;
    
    // Cargar el archivo XML existente
    if (doc.LoadFile(filename.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr <<RED<< "Error loading XML file: " << filename << std::endl;
        return;
    }

    // Obtener el nodo raíz
    tinyxml2::XMLElement* rootdoc = doc.FirstChildElement("Messages");
    if (!rootdoc) {
        cerr << RED <<"\n No root element 'Messages' found in XML file."<<endl;
        return;
    }

    // Crear un nuevo elemento de mensaje
    tinyxml2::XMLElement* messageElement = doc.NewElement("Message");
    // TODO:ACA ES DONDE VAMOS A AGREGAR EL ATRIBUTO
    messageElement->SetAttribute("uuid", uuid.c_str());
    messageElement->SetAttribute("date-and-time",dateNow.c_str());
    messageElement->SetAttribute("latitude-longitude",latitudeLongitude.c_str());
    messageElement->SetText(messageText.c_str());
    
    // Insertar el nuevo mensaje en el nodo raíz
    rootdoc->InsertEndChild(messageElement);
    
    // Guardar los cambios en el archivo
    if (doc.SaveFile(filename.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr <<RED<< "Error saving XML file: " << filename << std::endl;
    }
}

string getCurrentDateTime(){
    auto now = chrono::system_clock::now();

    time_t time = chrono::system_clock::to_time_t(now);

    tm* localTime = localtime(&time);

    ostringstream oss;

    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    
    // Retornar la cadena de tiempo formateada
    return oss.str();


}

bool getLatitudeLongitude(double &latitude, double &longitude){
    gpsmm gps_rec("localhost", DEFAULT_GPSD_PORT);

    if (gps_rec.stream(WATCH_ENABLE | WATCH_JSON) == NULL) {
        cerr <<RED<<  "Could not connect to GPSD."<<endl;
        return false;
    }

        while (gps_rec.waiting(1000000)) {
        struct gps_data_t* gpsData = gps_rec.read();
        if (gpsData == nullptr) {
            cerr<<RED << "\nError reading GPS data."<< endl;
            continue;
        }

        if ((gpsData->fix.mode >= MODE_2D) &&
            !isnan(gpsData->fix.latitude) && 
            !isnan(gpsData->fix.longitude)) {
            latitude = gpsData->fix.latitude;
            longitude = gpsData->fix.longitude;
            return true;
        }
    }

    cerr <<RED<< "\nCould not obtain a valid GPS fix."<< endl;
    return false;
}

string encryptMessage(const string &plaintext, const string &key, const string &iv) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    int len;
    int ciphertext_len;
    unsigned char ciphertext[1024];

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (unsigned char *)key.c_str(), (unsigned char *)iv.c_str());
    EVP_EncryptUpdate(ctx, ciphertext, &len, (unsigned char *)plaintext.c_str(), plaintext.size());
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return string((char *)ciphertext, ciphertext_len);
}

string decryptMessage(const string &ciphertext, const string &key, const string &iv) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    int len;
    int plaintext_len;
    unsigned char plaintext[1024];

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (unsigned char *)key.c_str(), (unsigned char *)iv.c_str());
    EVP_DecryptUpdate(ctx, plaintext, &len, (unsigned char *)ciphertext.c_str(), ciphertext.size());
    plaintext_len = len;
    EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return std::string((char *)plaintext, plaintext_len);
}