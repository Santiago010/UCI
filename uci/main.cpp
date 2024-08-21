#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/library/ActiveMQCPP.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/MessageProducer.h>
#include <cms/MessageConsumer.h>
#include <cms/TextMessage.h>
#include <iostream>
#include <vector>
#include <string>
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <thread>
#include <mutex>
#include <uuid/uuid.h>
#include "tinyxml2.h"
#include <fstream>
#include <iostream>


using namespace activemq::core;
using namespace cms;
using namespace std;

char option = 'r';
log4cpp::Category& root = log4cpp::Category::getRoot();
string brokerURI = "tcp://localhost:61616";
string topicName = "TEST";
string clientID = "";
string subscriptionName = "MyDurableSubscription";
string username = "user1";
string password = "password1";
bool stateReading = true;
string filename = "Messages.xml";

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
void loadXMLFile(const std::string& filename);
void saveMessageToXML(const std::string& filename, const std::string& uuid, const std::string& messageText);

// TODO:recordar qu debemos pasar tambien por parametro la ruta del archivo xml y el password y usuario de la session
int main(int argc, char* argv[]) {

    if (argc > 1) {
        clientID = argv[1];  // Asignar el argumento pasado al clientID
    } else {
        cerr << "No clientID provided. Using default clientID: " << clientID << endl;
        return 1;
    }


    if(!fileXmlExists(filename)){
        createXMLFile(filename);
    }else{
        loadXMLFile(filename);
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
    cout << "Enter the message to send: ";
    cin.ignore(); // Ignorar cualquier carácter de nueva línea restante en el buffer
    getline(cin, messageText);

    messageText = uuid + ": " + messageText;
    TextMessage* message = session->createTextMessage(messageText);

    try {
        producer->send(message);
        cout << "Message sent: " << message->getText() << endl;

        // Añadir el UUID al set de UUIDs a filtrar
        uuidToFilter.insert(uuid);
        saveMessageToXML("Messages.xml", uuid, messageText);
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
            string uuid, actualMessage;

            // Dividir el mensaje en UUID y texto
            size_t pos = messageText.find(':');
            if (pos != string::npos) {
                uuid = messageText.substr(0, pos);
                actualMessage = messageText.substr(pos + 2); // Salto de dos caracteres (': ')

                

                // Verificar si el UUID está en la lista de UUIDs a filtrar
                if (uuidToFilter.find(uuid) == uuidToFilter.end()) {
                    cout << "Received message: " << actualMessage << endl;
                    saveMessageToXML(filename,uuid,actualMessage);
                } else {
                    cout << "Message with filtered UUID: " << uuid << endl;
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
        cout << "s. Send a Message \ne. Exit \nReading Message..." << endl;
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
}

void loadXMLFile(const std::string& filename) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError eResult = doc.LoadFile(filename.c_str());
    if (eResult != tinyxml2::XML_SUCCESS) {
        std::cerr << "Error loading XML file: " << eResult << std::endl;
        return;
    }else{
        cout << "Messages.xml Loaded with success" << endl;
    }
    
    // Procesar el archivo XML    TODO:esto nos va servir para ver los mensajes que estan en el xml
    // tinyxml2::XMLElement* root = doc.FirstChildElement("Messages");
    // if (root) {
    //     tinyxml2::XMLElement* message = root->FirstChildElement("Message");
        


    //     while (message) {
    //         const char* uuid = message->Attribute("uuid");
    //         const char* text = message->GetText();
    //         if (uuid && text) {
    //             std::cout << "UUID: " << uuid << ", Message: " << text << std::endl;
    //         }
    //         message = message->NextSiblingElement("Message");
    //     }
    // } else {
    //     std::cerr << "No root element 'Messages' found in XML file." << std::endl;
    // }
}


void saveMessageToXML(const std::string& filename, const std::string& uuid, const std::string& messageText) {
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
    messageElement->SetAttribute("uuid", uuid.c_str());
    messageElement->SetText(messageText.c_str());
    
    // Insertar el nuevo mensaje en el nodo raíz
    root->InsertEndChild(messageElement);
    
    // Guardar los cambios en el archivo
    if (doc.SaveFile(filename.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Error saving XML file: " << filename << std::endl;
    }
}
