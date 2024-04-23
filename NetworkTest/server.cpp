#include "odin.h" // Einbinden einer benutzerdefinierten Header-Datei namens "odin.h".

#include <math.h> // Einbinden der Mathematikbibliothek.
#include <stdio.h> // Einbinden der Standard-Ein-/Ausgabebibliothek.
#include <winsock2.h> // Einbinden der Winsock2-Bibliothek für Netzwerkfunktionen.
#include <windows.h> // Einbinden der Windows-Bibliothek für verschiedene Windows-Funktionen.

// Definieren von Spielkonstanten wie Drehgeschwindigkeit, Beschleunigung, maximale Geschwindigkeit usw.
const float32 TURN_SPEED = 1.0f;
const float32 ACCELERATION = 20.0f;
const float32 MAX_SPEED = 200.0f;
const uint32 TICKS_PER_SECOND = 60;
const float32 SECONDS_PER_TICK = 1.0f / (float32)TICKS_PER_SECOND;
const uint16 MAX_CLIENTS = 4;
const float32 CLIENT_TIMEOUT = 5.0f;

// Definieren von Nachrichtentypen, die vom Client und vom Server gesendet werden.
enum class Client_Message : uint8
{
	Join,
	Leave,
	Input,
};

enum class Server_Message : uint8
{
	Join_Result,
	State,
};

// Definieren einer Struktur für die IP-Endpunkte der Clients.
struct IP_Endpoint
{
	uint32 address;
	uint16 port;
};

// Überladen des Gleichheitsoperators für IP_Endpoint-Strukturen.
bool operator==(const IP_Endpoint& a, const IP_Endpoint& b)
{
	return a.address == b.address && a.port == b.port;
}

// Definieren einer Struktur für den Zustand eines Spielers.
struct Player_State
{
	float32 x, y, facing, speed;
};

// Definieren einer Struktur für die Eingabe eines Spielers.
struct Player_Input
{
	bool32 up, down, left, right;
};

// Funktion zur Berechnung der vergangenen Zeit seit einem gegebenen Zeitpunkt.
static float32 time_since(LARGE_INTEGER t, LARGE_INTEGER frequency)
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	return (float32)(now.QuadPart - t.QuadPart) / (float32)frequency.QuadPart;
}

// Hauptfunktion des Programms.
int main()
{
    // Initialisierung der Winsock-Bibliothek.
    WORD winsock_version = 0x202;
    WSADATA winsock_data;
    if (WSAStartup(winsock_version, &winsock_data))
    {
        printf("WSAStartup failed: %d", WSAGetLastError());
        return 1;
    }

    // Erstellen eines UDP-Sockets.
    int address_family = AF_INET;
    int type = SOCK_DGRAM;
    int protocol = IPPROTO_UDP;
    SOCKET sock = socket(address_family, type, protocol);

    if (sock == INVALID_SOCKET)
    {
        printf("socket failed: %d", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Binden des Sockets an eine bestimmte Adresse und einen Port.
    SOCKADDR_IN local_address;
    local_address.sin_family = AF_INET;
    local_address.sin_port = htons(PORT);
    local_address.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock, (SOCKADDR*)&local_address, sizeof(local_address)) == SOCKET_ERROR)
    {
        printf("bind failed: %d", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Einrichten des Sockets für nicht blockierende Operationen.
    u_long enabled = 1;
    ioctlsocket(sock, FIONBIO, &enabled);

    // Verbesserung: Überprüfen Sie den Rückgabewert von ioctlsocket auf Fehler.
    if (ioctlsocket(sock, FIONBIO, &enabled) == SOCKET_ERROR)
    {
        printf("ioctlsocket failed: %d", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    UINT sleep_granularity_ms = 1;
    bool32 sleep_granularity_was_set = timeBeginPeriod(sleep_granularity_ms) == TIMERR_NOERROR;

    LARGE_INTEGER clock_frequency;
    QueryPerformanceFrequency(&clock_frequency);

    // Initialisierung der Client-Endpunkte, Spielobjekte und Eingaben.
    IP_Endpoint client_endpoints[MAX_CLIENTS];
    float32 time_since_heard_from_client[MAX_CLIENTS];
    Player_State client_objects[MAX_CLIENTS];
    Player_Input client_inputs[MAX_CLIENTS];

    // Initialisierung des Puffers für die Kommunikation.
    int8 buffer[SOCKET_BUFFER_SIZE];

    for (uint16 i = 0; i < MAX_CLIENTS; i++)
    {
        client_endpoints[i].address = {};
    }

    bool32 is_running = 1;

    while (is_running)
    {
        LARGE_INTEGER tick_start_time;
        QueryPerformanceCounter(&tick_start_time);

        while (true)
        {
            int flags = 0;
            SOCKADDR_IN from;
            int from_size = sizeof(from);
            int bytes_received = recvfrom(sock, buffer, SOCKET_BUFFER_SIZE, flags, (SOCKADDR*)&from, &from_size);

            if (bytes_received == SOCKET_ERROR)
            {
                int error = WSAGetLastError();
                if (error != WSAEWOULDBLOCK)
                {
                    printf("recvfrom returned SOCKET_ERROR, WSAGetLastError() %d", error);
                }
                break;
            }

            IP_Endpoint from_endpoint;
            from_endpoint.address = from.sin_addr.S_un.S_addr;
            from_endpoint.port = from.sin_port;
            // Der Switch-Block behandelt verschiedene Arten von Client-Nachrichten.
            switch (buffer[0])
            {
                // Fall: Der Client möchte dem Spiel beitreten.
            case (uint8)Client_Message::Join:
            {
                // Ausgabe der IP-Adresse und des Ports des Clients, der beitreten möchte.
                printf("Client_Message::Join from %u:%hu\n", from_endpoint.address, from_endpoint.port);

                // Initialisierung des Slots für den Client auf einen ungültigen Wert.
                uint16 slot = uint16(-1);

                // Durchsuchen der Client-Endpunkte, um einen freien Slot zu finden.
                for (uint16 i = 0; i < MAX_CLIENTS; i++)
                {
                    // Wenn die Adresse des aktuellen Slots 0 ist, ist der Slot frei.
                    if (client_endpoints[i].address == 0)
                    {
                        // Der freie Slot wird dem beitretenden Client zugewiesen.
                        slot = i;
                        break;
                    }
                }

                // Die erste Position des Puffers wird auf den Join_Result-Nachrichtentyp gesetzt.
                buffer[0] = (int8)Server_Message::Join_Result;

                // Wenn ein freier Slot gefunden wurde...
                if (slot != uint16(-1))
                {
                    // Ausgabe des zugewiesenen Slots.
                    printf("Client will be assigned to slot %hu\n", slot);

                    // Die zweite Position des Puffers wird auf 1 gesetzt, um anzuzeigen, dass der Beitritt erfolgreich war.
                    buffer[1] = 1;

                    // Kopieren des Slots in den Puffer.
                    memcpy(&buffer[2], &slot, sizeof(slot));

                    // Initialisierung der Flags für die sendto-Funktion.
                    flags = 0;

                    // Senden der Antwort an den Client.
                    if (sendto(sock, buffer, 4, flags, (SOCKADDR*)&from, from_size) != SOCKET_ERROR)
                    {
                        // Aktualisieren der Client-Endpunkte und Spielobjekte.
                        client_endpoints[slot] = from_endpoint;
                        time_since_heard_from_client[slot] = 0.0f;
                        client_objects[slot].x = 0.0f;
                        client_objects[slot].y = 0.0f;
                        client_objects[slot].facing = 0.0f;
                        client_objects[slot].speed = 0.0f;
                    }
                    else
                    {
                        // Ausgabe der Fehlermeldung, wenn das Senden fehlschlägt.
                        printf("sendto failed: %d", WSAGetLastError());
                    }
                }
                else
                {
                    // Ausgabe einer Nachricht, wenn kein freier Slot verfügbar ist.
                    printf("No slots available for client\n");

                    // Die zweite Position des Puffers wird auf 0 gesetzt, um anzuzeigen, dass der Beitritt fehlgeschlagen ist.
                    buffer[1] = 0;

                    // Initialisierung der Flags für die sendto-Funktion.
                    flags = 0;

                    // Senden der Antwort an den Client.
                    if (sendto(sock, buffer, 2, flags, (SOCKADDR*)&from, from_size) == SOCKET_ERROR)
                    {
                        // Ausgabe der Fehlermeldung, wenn das Senden fehlschlägt.
                        printf("sendto failed: %d", WSAGetLastError());
                    }
                }
            }
            break;

            // Fall: Der Client möchte das Spiel verlassen.
            case (uint8)Client_Message::Leave:
            {
                // Extrahieren des Slots aus der Nachricht des Clients.
                uint16 slot;
                memcpy(&slot, &buffer[1], sizeof(slot));

                // Überprüfen, ob der Endpunkt des Clients mit dem gespeicherten Endpunkt übereinstimmt.
                if (client_endpoints[slot] == from_endpoint)
                {
                    // Ausgabe der IP-Adresse und des Ports des Clients, der das Spiel verlassen hat.
                    printf("Client_Message::Leave from %u:%hu\n", from_endpoint.address, from_endpoint.port);

                    // Löschen des Endpunkts des Clients.
                    client_endpoints[slot].address = 0;
                }
                else
                {
                    // Ausgabe einer Nachricht, wenn der Endpunkt des Clients nicht mit dem gespeicherten Endpunkt übereinstimmt.
                    printf("Client_Message::Leave from %u:%hu, but endpoint doesn't match\n", from_endpoint.address, from_endpoint.port);
                }
            }
            break;

            // Fall: Der Client sendet Eingaben.
            case (uint8)Client_Message::Input:
            {
                // Extrahieren des Slots aus der Nachricht des Clients.
                uint16 slot;
                memcpy(&slot, &buffer[1], sizeof(slot));

                // Ausgabe der Anzahl der empfangenen Bytes und des Slots.
                printf("%d %hu\n", bytes_received, slot);

                // Überprüfen, ob der Endpunkt des Clients mit dem gespeicherten Endpunkt übereinstimmt.
                if (client_endpoints[slot] == from_endpoint)
                {
                    // Extrahieren der Eingaben aus der Nachricht des Clients.
                    uint8 input = buffer[3];

                    // Aktualisieren der Eingaben des Clients.
                    client_inputs[slot].up = input & 0x01;
                    client_inputs[slot].down = input & 0x02;
                    client_inputs[slot].left = input & 0x04;
                    client_inputs[slot].right = input & 0x08;

                    // Zurücksetzen des Timers, seitdem der Client zuletzt gehört wurde.
                    time_since_heard_from_client[slot] = 0.0f;

                    // Ausgabe der Eingaben des Clients.
                    printf("Client_Message::Input from %u:%hu\n", slot, int32(input));
                }
                else
                {
                    // Ausgabe einer Nachricht, wenn der Endpunkt des Clients nicht mit dem gespeicherten Endpunkt übereinstimmt.
                    printf("Client_Message::Input from %u:%hu, but endpoint doesn't match but expected %u:%hu\n", from_endpoint.address, from_endpoint.port, client_endpoints[slot].address, client_endpoints[slot].port);
                }
            }
            break;
            }
        }

        // Durchlaufen aller Clients.
        for (uint16 i = 0; i < MAX_CLIENTS; i++)
        {
            // Überprüfen, ob der Client verbunden ist.
            if (client_endpoints[i].address)
            {
                // Behandeln der Eingaben des Clients.
                if (client_inputs[i].up)
                {
                    // Aktualisieren der Geschwindigkeit des Clients, wenn die "nach oben"-Taste gedrückt wird.
                    client_objects[i].speed += ACCELERATION * SECONDS_PER_TICK;
                    if (client_objects[i].speed > MAX_SPEED)
                    {
                        client_objects[i].speed = MAX_SPEED;
                    }
                }

                // Aktualisieren der Geschwindigkeit des Clients, wenn die "nach unten"-Taste gedrückt wird.
                if (client_inputs[i].down)
                {
                    client_objects[i].speed -= ACCELERATION * SECONDS_PER_TICK;
                    if (client_objects[i].speed < 0.0f)
                    {
                        client_objects[i].speed = 0.0f;
                    }
                }

                // Aktualisieren der Ausrichtung des Clients, wenn die "nach links"-Taste gedrückt wird.
                if (client_inputs[i].left)
                {
                    client_objects[i].facing -= TURN_SPEED * SECONDS_PER_TICK;
                }

                // Aktualisieren der Ausrichtung des Clients, wenn die "nach rechts"-Taste gedrückt wird.
                if (client_inputs[i].right)
                {
                    client_objects[i].facing += TURN_SPEED * SECONDS_PER_TICK;
                }

                // Aktualisieren der Position des Clients basierend auf seiner Geschwindigkeit und Ausrichtung.
                client_objects[i].x += client_objects[i].speed * SECONDS_PER_TICK * sinf(client_objects[i].facing);
                client_objects[i].y += client_objects[i].speed * SECONDS_PER_TICK * cosf(client_objects[i].facing);

                // Aktualisieren des Timers, seitdem der Client zuletzt gehört wurde.
                time_since_heard_from_client[i] += SECONDS_PER_TICK;

                // Trennen des Clients, wenn er für eine bestimmte Zeit nicht gehört wurde.
                if (time_since_heard_from_client[i] > CLIENT_TIMEOUT)
                {
                    client_endpoints[i].address = {};
                }
            }
        }

        // Erstellen eines Zustandspakets.
        buffer[0] = (int8)Server_Message::State;
        int32 bytes_written = 1;

        // Durchlaufen aller Clients.
        for (uint16 i = 0; i < MAX_CLIENTS; i++)
        {
            // Überprüfen, ob der Client verbunden ist.
            if (client_endpoints[i].address)
            {
                // Kopieren der Client-Informationen in den Puffer.
                memcpy(&buffer[bytes_written], &i, sizeof(i));
                bytes_written += sizeof(i);

                memcpy(&buffer[bytes_written], &client_objects[i].x, sizeof(client_objects[i].x));
                bytes_written += sizeof(client_objects[i].x);

                memcpy(&buffer[bytes_written], &client_objects[i].y, sizeof(client_objects[i].y));
                bytes_written += sizeof(client_objects[i].y);

                memcpy(&buffer[bytes_written], &client_objects[i].facing, sizeof(client_objects[i].facing));
                bytes_written += sizeof(client_objects[i].facing);
            }
        }

        // Senden des Zustandspakets an alle Clients.
        int flags = 0;
        SOCKADDR_IN to;
        to.sin_family = AF_INET;
        to.sin_port = htons(PORT);
        int to_length = sizeof(to);

        for (uint16 i = 0; i < MAX_CLIENTS; ++i)
        {
            if (client_endpoints[i].address)
            {
                to.sin_addr.S_un.S_addr = client_endpoints[i].address;
                to.sin_port = htons(client_endpoints[i].port);

                if (sendto(sock, buffer, bytes_written, flags, (SOCKADDR*)&to, to_length) == SOCKET_ERROR)
                {
                    printf("sendto failed: %d", WSAGetLastError());
                }
            }
        }

        // Berechnung der seit dem Start des Ticks verstrichenen Zeit.
        float32 time_taken_s = time_since(tick_start_time, clock_frequency);

        // Warten, bis die erforderliche Zeit für einen Tick verstrichen ist.
        while (time_taken_s < SECONDS_PER_TICK)
         {
            if (sleep_granularity_was_set)
            {
                DWORD time_to_wait_ms = DWORD((SECONDS_PER_TICK - time_taken_s) * 1000);
                if (time_to_wait_ms > 0)
                {
                    Sleep(time_to_wait_ms);
                }
            }
            time_taken_s = time_since(tick_start_time, clock_frequency);
         }
    }
}
