// #include <activemq/core/ActiveMQConnectionFactory.h>
// #include <activemq/library/ActiveMQCPP.h>
// #include <activemq/core/ActiveMQConnection.h>
// #include <activemq/core/ActiveMQSession.h>
// #include <activemq/core/ActiveMQConsumer.h>
// #include <activemq/core/ActiveMQProducer.h>
// #include <cms/Connection.h>
// #include <cms/Session.h>
// #include <cms/MessageProducer.h>
// #include <cms/MessageConsumer.h>
// #include <cms/TextMessage.h>
// #include <iostream>
// #include <vector>
// #include <string.h>
// #include <log4cpp/Category.hh>//Libreria para los Logs 
// #include <log4cpp/PropertyConfigurator.hh> //libreria para los Logs

// using namespace std;
// using namespace activemq::core;
// using namespace cms;

// log4cpp::Category& root = log4cpp::Category::getRoot();
// string brokerURI = "tcp://localhost:61616";

// void sendMessage(Session* session);
// void readMessage(Session* session);



// int main(int argc, char* argv[]) {

//     char option;

//     activemq::library::ActiveMQCPP::initializeLibrary();

//     try
//     {
//         // Crea una conexion
//        ActiveMQConnectionFactory* connectionFactory = new ActiveMQConnectionFactory(brokerURI);
// auto connection = connectionFactory->createConnection("user1","password1");
//        connection->start();

//     //    Crea una sesion
//         auto session = connection->createSession(Session::AUTO_ACKNOWLEDGE);

//         do {
//             cout << "r. Read the message \ns. Send a Message" << endl;
//             cin >> option;

//             switch (option) {
//                 case 'r':
//                     cout << "Reading Messages..." << endl;
//                     readMessage(session);
//                     break;
                
//                 case 's':
//                     sendMessage(session);
//                     break;
                
//                 case 'e':
//                     cout << "leaving the room..." << endl;
//                     break;
//                 default:
//                     cout << "Invalid option, please try again." << endl;
//                     break;
//             }
//             // system("clear");
//         } while (option != 'e');

//         // Limpia
//         session->close();
//         connection->close();
        
//         delete session;
//         delete connection;
//         delete connectionFactory;

//     }
//     catch(const std::exception& e)
//     {
//          root.errorStream() << "Error: " << e.what();
//     }
//     return 0;
// }

// void sendMessage(Session* session){

//         string userMessage;
//         cout << "Please enter the message you want to send: ";
//         cin.ignore(); // Ignorar cualquier carácter residual en el buffer
//         getline(cin, userMessage); // Leer la línea completa
//         // Crea un destino (un tema)
//         auto destination = session->createTopic("TEST");
        
//         // Crea un productor de mensajes
//         auto producer = session->createProducer(destination);
        
//         // Crea un mensaje
//         auto message = session->createTextMessage(userMessage);
        
//         // Envía el mensaje
//         producer->send(message);

//         producer->close();
//         delete producer;

//         cout << "mensaje enviado" << endl;
// }

// void readMessage(Session* session){
//     auto destination = session->createTopic("TEST");
//     auto consumer = session->createConsumer(destination);

//     auto message = consumer->receive();
        
//     // Comprueba si el mensaje recibido es un TextMessage
//     if (message != nullptr) {
//         // Realiza el cast a TextMessage para obtener el texto
//         cms::TextMessage* textMessage = dynamic_cast<cms::TextMessage*>(message);
//         if (textMessage != nullptr) {
//             cout << "Received message: " << textMessage->getText() << endl;
//         } else {
//             cout << "Received non-text message." << endl;
//         }
//     } else {
//         cout << "No message received." << endl;
//     }

//     consumer->close();  
//     delete consumer;
// }

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/library/ActiveMQCPP.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/MessageProducer.h>
#include <cms/MessageConsumer.h>
#include <cms/TextMessage.h>
#include <iostream>

using namespace activemq::core;
using namespace cms;
using namespace std;

int main() {
    activemq::library::ActiveMQCPP::initializeLibrary();

    string brokerURI = "tcp://localhost:61616";
    string topicName = "test-topic";
    string clientID = "DurableClient1";
    string subscriptionName = "MyDurableSubscription";
    string username = "user1";
    string password = "password1";

    try {
        ActiveMQConnectionFactory* connectionFactory = new ActiveMQConnectionFactory(brokerURI);

        // Establece el usuario y la contraseña
        Connection* connection = connectionFactory->createConnection(username, password);
        connection->setClientID(clientID);
        connection->start();

        Session* session = connection->createSession(Session::AUTO_ACKNOWLEDGE);

        // Crear el Topic y hacer el cast a const cms::Topic*
        Topic* topic = session->createTopic(topicName);
        const Topic* constTopic = static_cast<const Topic*>(topic);

        // Crear el consumidor duradero
        MessageConsumer* consumer = session->createDurableConsumer(constTopic, subscriptionName, "", false);

        while (true) {
            Message* message = consumer->receive();
            if (message != nullptr) {
                TextMessage* textMessage = dynamic_cast<TextMessage*>(message);
                if (textMessage != nullptr) {
                    cout << "Received message: " << textMessage->getText() << endl;
                } else {
                    cout << "Received non-text message." << endl;
                }
                delete message;
            }
        }

        consumer->close();
        session->close();
        connection->close();

        delete consumer;
        delete session;
        delete connection;
        delete connectionFactory;
    } catch (CMSException& e) {
        cerr << "Exception occurred: " << e.getMessage() << endl;
    }

    activemq::library::ActiveMQCPP::shutdownLibrary();

    return 0;
}
