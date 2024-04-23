#include "CPackets.h"

CPackets::CPackets()
{
}

CPackets::~CPackets()
{
}

bool CPackets::createPacket(const char* buffer, int bufferLength, int flags, const sockaddr* to, int toLength)
{

    return false;
}

bool CPackets::sendPacket(const char* buffer, int bufferLength, int flags, const sockaddr* to, int toLength)
{
    return false;
}

int CPackets::receivePacket(char* buffer, int bufferLength, int flags, sockaddr* from, int* fromSize)
{
    return 0;
}

char* CPackets::getBuffer()
{
    // Get Buffer
    return m_buffer;
}

const int CPackets::getBufferLength()
{
    return m_bufferLength;
}

const int CPackets::getFlags()
{
    return m_flags;
}

const int CPackets::getToLength()
{
    return m_toLength;
}

int CPackets::getSocketBufferSize()
{
    return SOCKET_BUFFER_SIZE;
}

int CPackets::getFromSize()
{
    return m_fromSize;
}

void CPackets::closePacket()
{
}
