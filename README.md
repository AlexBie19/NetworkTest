# NetworkTest
Konstruktor (Network::Network()): Der Konstruktor initialisiert die Anzahl der Netzwerkgeräte auf 0.
Network::Init(): Diese Methode initialisiert das Netzwerk. Sie führt mehrere Aufgaben aus:
Start von Winsock: Mit WSAStartup wird die Winsock-DLL initialisiert. Wenn dies fehlschlägt, wird eine Ausnahme ausgelöst.
Abrufen von Adapterinformationen: Mit GetAdaptersAddresses werden Informationen über die Netzwerkadapter des Systems abgerufen. Für jeden Adapter, der nicht die Loopback-Adresse (127.0.0.1) verwendet, werden die Unicast-Adresse, die Subnetzmaske und die Broadcast-Adresse berechnet und gespeichert. Die Adressen werden auch in Stringform für die Ausgabe konvertiert.
Erstellen eines Sockets: Ein UDP-Socket wird erstellt und so konfiguriert, dass er Broadcast-Nachrichten senden kann. Der Socket wird auch in den nicht blockierenden Modus versetzt.
Binden des Sockets: Der Socket wird an die Adresse 0.0.0.0 und den Port 21000 gebunden, sodass er Nachrichten von jeder Quelle auf diesem Port empfangen kann.
Network::Fini(): Diese Methode schließt den Socket, wenn er nicht mehr benötigt wird.
Network::Receive(DataPacket* packet, int packetSize): Diese Methode empfängt Daten vom Socket. Sie verwendet recvfrom, um Daten zu empfangen, und gibt die Anzahl der empfangenen Bytes zurück. Wenn ein Fehler auftritt, wird eine Ausnahme ausgelöst, es sei denn, der Fehler ist WSAEWOULDBLOCK, was bedeutet, dass keine Daten zum Empfangen vorhanden sind. In diesem Fall gibt die Methode 0 zurück.
