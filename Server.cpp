#include <QtCore/QCoreApplication>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QDebug>
#include <QList>

class GameServer : public QTcpServer {
    Q_OBJECT

public:
    GameServer(QObject* parent = nullptr) : QTcpServer(parent) {
        connect(this, &QTcpServer::newConnection, this, &GameServer::handleNewConnection);
    }

    void startServer(quint16 port) {
        if (listen(QHostAddress::Any, port)) {
            qDebug() << "Serwer uruchomiony na porcie:" << port;
        }
        else {
            qDebug() << "Nie mo¿na uruchomiæ serwera:" << errorString();
        }
    }

private slots:
    void handleNewConnection() {
        QTcpSocket* clientSocket = nextPendingConnection();
        clients.append(clientSocket);
        connect(clientSocket, &QTcpSocket::readyRead, this, &GameServer::readClientData);
        connect(clientSocket, &QTcpSocket::disconnected, this, &GameServer::handleClientDisconnected);

        qDebug() << "Nowe po³¹czenie od:" << clientSocket->peerAddress().toString();
    }

    void readClientData() {
        QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
        if (!clientSocket) return;

        QByteArray data = clientSocket->readAll();
        qDebug() << "Otrzymano dane:" << data;

        // Logika gry: na przyk³ad przekazanie wiadomoœci do wszystkich klientów
        for (QTcpSocket* client : clients) {
            if (client != clientSocket) {
                client->write(data);
            }
        }
    }

    void handleClientDisconnected() {
        QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
        if (!clientSocket) return;

        clients.removeAll(clientSocket);
        clientSocket->deleteLater();
        qDebug() << "Klient roz³¹czy³ siê";
    }

private:
    QList<QTcpSocket*> clients;
};
