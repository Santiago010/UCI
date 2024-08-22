// TODO:activeMQ
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/library/ActiveMQCPP.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/MessageProducer.h>
#include <cms/MessageConsumer.h>
#include <cms/TextMessage.h>
// TODO:log4cpp
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <thread>
#include <mutex>
// TODO:UUID
#include <uuid/uuid.h>
// TODO:tinyxml2
#include "tinyxml2.h"
#include <fstream>
// TODO:librerias nativas
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

// TODO: GPS
#include <libgpsmm.h>




using namespace activemq::core;
using namespace cms;
using namespace std;

char option = 'r';
log4cpp::Category& root = log4cpp::Category::getRoot();
string brokerURI = "";
string topicName = "";
string clientID = "";
string subscriptionName = "";
string username = "";
string password = "";
bool stateReading = true;
string filenameMessage = "";

mutex optionMutex;
Session* globalSession = nullptr;
Topic* globalTopic = nullptr;

set<string> uuidToFilter = {};


void sendMessage(Session* session, Topic* topic);
void readMessage(MessageConsumer* consumer);
void userInput(Session* session, Topic* topic);
bool fileXmlExists(const std::string& filename);
void createXMLFile(const std::string& filename);
string generateUUID();
bool loadXMLFile(const std::string& filename);
void saveMessageToXML(const std::string& filename, const std::string& uuid,const std::string& dateNow,const std::string& latitudeLongitude, const std::string& messageText);
string getCurrentDateTime();
bool getLatitudeLongitude(double &latitude, double &longitude);

// TODO:recordar qu debemos pasar tambien por parametro la ruta del archivo xml y el password y usuario de la session
int main() {

    if(loadXMLFile("Conf.xml")){
        std::cout << "configuration file loaded with successfully" << endl;
    }else{
        std::cerr << "Error loading XML conf file: "  << std::endl;
    }


    if(!fileXmlExists(filenameMessage)){
        createXMLFile(filenameMessage);
    }

    // Configuración de log4cpp
    // log4cpp::PropertyConfigurator::configure("log4cpp.properties");

    activemq::library::ActiveMQCPP::initializeLibrary();

    try {
        ActiveMQConnectionFactory* connectionFactory = new ActiveMQConnectionFactory(brokerURI);

        auto connection = connectionFactory->createConnection();
        connection->setClientID(clientID);
        connection->start();

        Session* session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
        Topic* topic = session->createTopic(topicName);
        const Topic* constTopic = static_cast<const Topic*>(topic);

        // Establecer variables globales
        globalSession = session;
        globalTopic = topic;

        // Crear el consumidor duradero
        MessageConsumer* consumer = session->createDurableConsumer(constTopic, subscriptionName, "", false);

        // Iniciar el hilo para manejar la entrada del usuario
        thread inputThread(userInput, session, topic);

        do {
            // Esperar mensajes en un bucle
            readMessage(consumer);

            // Bloquear el acceso a la variable de opción para evitar condiciones de carrera
            lock_guard<mutex> lock(optionMutex);

        } while (option != 'e');

        // Esperar a que el hilo de entrada del usuario termine
        inputThread.join();

        consumer->close();
        session->close();
        connection->close();

        delete consumer;
        delete session;
        delete connection;
        delete connectionFactory;
    }
    catch (CMSException& e) {
        cerr << "Exception occurred: " << e.getMessage() << endl;
    }

    return 0;
}

void sendMessage(Session* session, Topic* topic) {
    MessageProducer* producer = session->createProducer(topic);
    producer->setDeliveryMode(cms::DeliveryMode::NON_PERSISTENT);

    string uuid = generateUUID();
    string messageText;
    string originalMessageText;
    string latitudeLongitude;
    cout << "Enter the message to send: ";
    cin.ignore(); // Ignorar cualquier carácter de nueva línea restante en el buffer
    getline(cin, messageText);

    originalMessageText = messageText;
    string dateNow = getCurrentDateTime();
    double latitude = 0.0, longitude = 0.0;

    getLatitudeLongitude(latitude,longitude);
    latitudeLongitude = to_string(latitude) + "," + to_string(longitude);
    messageText = uuid + ": " + messageText + " date: " + dateNow + " latitude and longitude: " + latitudeLongitude;
    TextMessage* message = session->createTextMessage(messageText);

    try {
        producer->send(message);
        // TODO:imprimi el mensaje
        // cout << "Message sent: " << message->getText() << endl;

        // Añadir el UUID al set de UUIDs a filtrar
        uuidToFilter.insert(uuid);
        saveMessageToXML(filenameMessage, uuid,dateNow,latitudeLongitude, originalMessageText);
    } catch (CMSException& e) {
        cerr << "Exception occurred while sending message: " << e.getMessage() << endl;
    }

    stateReading = true;

    delete message;
    delete producer;
}

void readMessage(MessageConsumer* consumer) {
    Message* message = consumer->receive(1000);  // Espera 1 segundo
    if (message != nullptr) {
        TextMessage* textMessage = dynamic_cast<TextMessage*>(message);
        if (textMessage != nullptr) {
            string messageText = textMessage->getText();
            string uuid, actualMessage, dateNow, latLong;

            // Dividir el mensaje en UUID, mensaje y fecha
            size_t uuidPos = messageText.find(':');
            size_t datePos = messageText.find(" date: ");
            size_t latLongPos = messageText.find(" latitude and longitude: ");

            if (uuidPos != string::npos && datePos != string::npos && latLongPos != string::npos) {
                uuid = messageText.substr(0, uuidPos);
                actualMessage = messageText.substr(uuidPos + 2, datePos - (uuidPos + 2)); // Salto de dos caracteres (': ')
                dateNow = messageText.substr(datePos + 7, latLongPos - (datePos + 7));   // 7 para " date: "
                latLong = messageText.substr(latLongPos + 24);  // 24 para " latitude and longitude: "

                // Verificar si el UUID está en la lista de UUIDs a filtrar
                if (uuidToFilter.find(uuid) == uuidToFilter.end()) {
                    cout << "Received message: " << actualMessage <<"\nwith uuid:" << uuid<< "\ndate Time: " <<dateNow << "\nLatitude and Longitude: " << latLong<< endl;
                    saveMessageToXML(filenameMessage, uuid,dateNow,latLong, actualMessage);
                } else {
                    cout << "Message with filtered UUID sent: " << uuid << endl;
                }
            } else {
                cout << "Received message in unknown format: " << messageText << endl;
            }
        } else {
            cout << "Received non-text message." << endl;
        }
        delete message;
    }
}

void userInput(Session* session, Topic* topic) {
    while (true) {
        cout << "\ns. Send a Message \ne. Exit \nReading Message..." << endl;
        cin >> option;

        // Bloquear el acceso a la variable de opción para evitar condiciones de carrera
        lock_guard<mutex> lock(optionMutex);

        if (option == 'e') {
            break;
        }

        if (option == 's') {
            cout << "Sending a Message..." << endl;
            sendMessage(session, topic); // Pasar Session y Topic
        } else {
            cout << "Invalid option." << endl;
        }
    }
}

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
    tinyxml2::XMLElement* root = doc.NewElement("Messages");
    doc.InsertFirstChild(root);
    
    // Agrega un elemento inicial si es necesario
    tinyxml2::XMLElement* message = doc.NewElement("Message");
    message->SetAttribute("uuid", "example-uuid");
    message->SetText("This is an example message.");
    root->InsertEndChild(message);
    
    // Guardar el archivo
    doc.SaveFile(filename.c_str());
    cout << "xml file of messages successfully created";
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
        std::cerr << "Error loading XML file: " << filename << std::endl;
        return;
    }

    // Obtener el nodo raíz
    tinyxml2::XMLElement* root = doc.FirstChildElement("Messages");
    if (!root) {
        std::cerr << "No root element 'Messages' found in XML file." << std::endl;
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
    root->InsertEndChild(messageElement);
    
    // Guardar los cambios en el archivo
    if (doc.SaveFile(filename.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Error saving XML file: " << filename << std::endl;
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
        cerr << "Could not connect to GPSD." << endl;
        return false;
    }

        while (gps_rec.waiting(1000000)) {
        struct gps_data_t* gpsData = gps_rec.read();
        if (gpsData == nullptr) {
            cerr << "Error reading GPS data." << endl;
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

    cerr << "No se pudo obtener una fijación válida del GPS." << endl;
    return false;
}