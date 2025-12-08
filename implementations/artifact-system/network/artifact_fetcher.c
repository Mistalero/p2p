#include "../include/artifact_fetcher.h"
#include "../include/artifact.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#endif

// Fetch artifact from URL
artifact_t* artifact_fetcher_fetch_from_url(const char* url) {
    // Parse URL to extract host, port, and path
    // This is a simplified parser that handles http://host:port/path style URLs
    if (!url || strlen(url) < 8 || strncmp(url, "http://", 7) != 0) {
        return NULL;
    }
    
    // Find start of host
    const char* host_start = url + 7;
    
    // Find end of host (look for : or /)
    const char* host_end = host_start;
    while (*host_end && *host_end != ':' && *host_end != '/') {
        host_end++;
    }
    
    if (host_start == host_end) {
        return NULL;
    }
    
    // Extract host
    char host[256];
    size_t host_len = host_end - host_start;
    if (host_len >= sizeof(host)) {
        return NULL;
    }
    strncpy(host, host_start, host_len);
    host[host_len] = '\0';
    
    // Extract port (default to 80)
    int port = 80;
    if (*host_end == ':') {
        const char* port_start = host_end + 1;
        const char* port_end = port_start;
        while (*port_end >= '0' && *port_end <= '9') {
            port_end++;
        }
        char port_str[6];
        size_t port_len = port_end - port_start;
        if (port_len > 0 && port_len < sizeof(port_str)) {
            strncpy(port_str, port_start, port_len);
            port_str[port_len] = '\0';
            port = atoi(port_str);
        }
        host_end = port_end;
    }
    
    // Extract path (default to /)
    const char* path = "/";
    if (*host_end == '/') {
        path = host_end;
    }
    
    // Initialize Winsock on Windows
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        return NULL;
    }
#endif
    
    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
#ifdef _WIN32
        WSACleanup();
#endif
        return NULL;
    }
    
    // Resolve hostname
    struct hostent* host_entry = gethostbyname(host);
    if (!host_entry) {
#ifdef _WIN32
        closesocket(sock);
        WSACleanup();
#else
        close(sock);
#endif
        return NULL;
    }
    
    // Fill in server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    memcpy(&server_addr.sin_addr, host_entry->h_addr_list[0], host_entry->h_length);
    
    // Connect to server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
#ifdef _WIN32
        closesocket(sock);
        WSACleanup();
#else
        close(sock);
#endif
        return NULL;
    }
    
    // Send HTTP GET request
    char request[1024];
    snprintf(request, sizeof(request), 
             "GET %s HTTP/1.1\r
"
             "Host: %s:%d\r
"
             "Connection: close\r
"
             "\r
", 
             path, host, port);
    
    if (send(sock, request, strlen(request), 0) < 0) {
#ifdef _WIN32
        closesocket(sock);
        WSACleanup();
#else
        close(sock);
#endif
        return NULL;
    }
    
    // Receive response
    char response[8192];
    int total_bytes = 0;
    int bytes_received;
    
    while ((bytes_received = recv(sock, response + total_bytes, sizeof(response) - total_bytes - 1, 0)) > 0) {
        total_bytes += bytes_received;
        if (total_bytes >= sizeof(response) - 1) {
            break;
        }
    }
    
    // Close socket
#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
    
    // Null terminate response
    response[total_bytes] = '\0';
    
    // Check HTTP status code
    if (strncmp(response, "HTTP/1.1 200", 13) != 0 && strncmp(response, "HTTP/1.0 200", 12) != 0) {
        // Not a 200 OK response, likely 404 Not Found or other error
        return NULL;
    }
    
    // Find start of body (skip headers)
    char* body_start = strstr(response, "\r
\r
");
    if (!body_start) {
        body_start = strstr(response, "

");
        if (!body_start) {
            return NULL;
        }
        body_start += 2;
    } else {
        body_start += 4;
    }
    
    // Calculate body size
    size_t body_size = strlen(body_start);
    
    // Check if body is empty
    if (body_size == 0) {
        return NULL;
    }
    
    // Parse artifact from body
    // For now, we assume the body contains the raw binary artifact data
    // In a real implementation, we might need to decode the body (e.g., if it's base64 encoded)
    return artifact_parse_from_buffer((uint8_t*)body_start, body_size);
}

// Fetch artifact from network (placeholder that uses a predefined URL)
artifact_t* artifact_fetcher_fetch(uint8_t* id) {
    // In a real implementation, this would:
    // 1. Connect to the P2P network
    // 2. Send a request for the artifact with the given ID
    // 3. Receive the artifact data
    // 4. Parse the data into an artifact_t structure
    // 5. Return the artifact
    
    // For now, we construct a URL based on the artifact ID and try to fetch it
    // This is a very simplified approach for demonstration purposes
    
    if (!id) {
        return NULL;
    }
    
    // Convert ID to hex string
    char id_str[65];
    for (int i = 0; i < 32; i++) {
        sprintf(&id_str[i * 2], "%02x", id[i]);
    }
    id_str[64] = '\0';
    
    // Construct URL
    char url[512];
    snprintf(url, sizeof(url), "http://localhost:8080/artifacts/%s", id_str);
    
    // Try to fetch from URL
    return artifact_fetcher_fetch_from_url(url);
}