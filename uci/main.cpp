#include <activemq/core/ActiveMQConnectionFactory.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/MessageProducer.h>
#include <cms/MessageConsumer.h>
#include <cms/TextMessage.h>
#include <iostream>

using namespace std;
using namespace activemq::core;
using namespace cms;

int main() {
    // URI del broker
    string brokerURI = "tcp://localhost:61616";
    string queueName = "test";

    // Crear una instancia de ActiveMQConnectionFactory con la URI del broker
    ActiveMQConnectionFactory* connectionFactory = new ActiveMQConnectionFactory(brokerURI);

    // Crear una conexión
    Connection* connection = connectionFactory->createConnection();
    connection->start();

    // Crear una sesión
    Session* session = connection->createSession(Session::AUTO_ACKNOWLEDGE);

    // Crear una cola
    Destination* destination = session->createQueue(queueName);

    // Crear un productor de mensajes
    MessageProducer* producer = session->createProducer(destination);
    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    // Enviar un mensaje
    std::string text = "La vida es hermosa";
    TextMessage* message = session->createTextMessage(text);
    producer->send(message);

    // Crear un consumidor de mensajes
    MessageConsumer* consumer = session->createConsumer(destination);

    // Recibir un mensaje
    Message* receivedMessage = consumer->receive();

    try {
        if (receivedMessage != nullptr) {
            TextMessage* textMessage = dynamic_cast<TextMessage*>(receivedMessage);
            if (textMessage != nullptr) {
                cout << "Received: " << textMessage->getText() << endl;
            }
        }
    } catch (CMSException& e) {
        cerr << "Error occurred: " << e.getMessage() << endl;
    }

    // Limpieza
    delete producer;
    delete consumer;
    delete destination;
    session->close();
    delete session;
    connection->close();
    delete connection;
    delete connectionFactory;

    return 0;
}
