#include "mbed.h"
#include "mbed_trace.h"

#ifndef DEVICE_TRNG
#error "mbed-os-example-tls-socket requires a device which supports TRNG"
#else

const char cert[] = \
    "-----BEGIN CERTIFICATE-----\n"
    "MIIENjCCAx6gAwIBAgIBATANBgkqhkiG9w0BAQUFADBvMQswCQYDVQQGEwJTRTEU\n"
    "MBIGA1UEChMLQWRkVHJ1c3QgQUIxJjAkBgNVBAsTHUFkZFRydXN0IEV4dGVybmFs\n"
    "IFRUUCBOZXR3b3JrMSIwIAYDVQQDExlBZGRUcnVzdCBFeHRlcm5hbCBDQSBSb290\n"
    "MB4XDTAwMDUzMDEwNDgzOFoXDTIwMDUzMDEwNDgzOFowbzELMAkGA1UEBhMCU0Ux\n"
    "FDASBgNVBAoTC0FkZFRydXN0IEFCMSYwJAYDVQQLEx1BZGRUcnVzdCBFeHRlcm5h\n"
    "bCBUVFAgTmV0d29yazEiMCAGA1UEAxMZQWRkVHJ1c3QgRXh0ZXJuYWwgQ0EgUm9v\n"
    "dDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALf3GjPm8gAELTngTlvt\n"
    "H7xsD821+iO2zt6bETOXpClMfZOfvUq8k+0DGuOPz+VtUFrWlymUWoCwSXrbLpX9\n"
    "uMq/NzgtHj6RQa1wVsfwTz/oMp50ysiQVOnGXw94nZpAPA6sYapeFI+eh6FqUNzX\n"
    "mk6vBbOmcZSccbNQYArHE504B4YCqOmoaSYYkKtMsE8jqzpPhNjfzp/haW+710LX\n"
    "a0Tkx63ubUFfclpxCDezeWWkWaCUN/cALw3CknLa0Dhy2xSoRcRdKn23tNbE7qzN\n"
    "E0S3ySvdQwAl+mG5aWpYIxG3pzOPVnVZ9c0p10a3CitlttNCbxWyuHv77+ldU9U0\n"
    "WicCAwEAAaOB3DCB2TAdBgNVHQ4EFgQUrb2YejS0Jvf6xCZU7wO94CTLVBowCwYD\n"
    "VR0PBAQDAgEGMA8GA1UdEwEB/wQFMAMBAf8wgZkGA1UdIwSBkTCBjoAUrb2YejS0\n"
    "Jvf6xCZU7wO94CTLVBqhc6RxMG8xCzAJBgNVBAYTAlNFMRQwEgYDVQQKEwtBZGRU\n"
    "cnVzdCBBQjEmMCQGA1UECxMdQWRkVHJ1c3QgRXh0ZXJuYWwgVFRQIE5ldHdvcmsx\n"
    "IjAgBgNVBAMTGUFkZFRydXN0IEV4dGVybmFsIENBIFJvb3SCAQEwDQYJKoZIhvcN\n"
    "AQEFBQADggEBALCb4IUlwtYj4g+WBpKdQZic2YR5gdkeWxQHIzZlj7DYd7usQWxH\n"
    "YINRsPkyPef89iYTx4AWpb9a/IfPeHmJIZriTAcKhjW88t5RxNKWt9x+Tu5w/Rw5\n"
    "6wwCURQtjr0W4MHfRnXnJK3s9EK0hZNwEGe6nQY1ShjTK3rMUUKhemPR5ruhxSvC\n"
    "Nr4TDea9Y355e6cJDUCrat2PisP29owaQgVR1EX1n6diIWgVIEM8med8vSTYqZEX\n"
    "c4g/VhsxOBi0cQ+azcgOno4uG+GMmIPLHzHxREzGBHNJdmAPx/i9F4BrLunMTA5a\n"
    "mnkPIAou1Z5jJh5VkpTYghdae9C8x49OhgQ=\n"
    "-----END CERTIFICATE-----";

int main(void)
{
    char *buffer = new char[256];
    nsapi_size_or_error_t result;
    nsapi_size_t size;
    const char query[] = "GET / HTTP/1.1\r\nHost: ifconfig.io\r\nConnection: close\r\n\r\n";

    mbed_trace_init();

    printf("TLSSocket Example.\n");
    printf("Mbed OS version: %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);

    NetworkInterface *net = NetworkInterface::get_default_instance();

    if (!net) {
        printf("Error! No network inteface found.\n");
        return 0;
    }

    printf("Connecting to network\n");
    result = net->connect();
    if (result != 0) {
        printf("Error! net->connect() returned: %d\n", result);
        return result;
    }

    TLSSocket *socket = new TLSSocket;
    result = socket->set_root_ca_cert(cert);
    if (result != 0) {
        printf("Error: socket->set_root_ca_cert() returned %d\n", result);
        return result;
    }

    result = socket->open(net);
    if (result != 0) {
        printf("Error! socket->open() returned: %d\n", result);
        return result;
    }

    printf("Connecting to ifconfig.io\n");
    result = socket->connect("ifconfig.io", 443);
    if (result != 0) {
        printf("Error! socket->connect() returned: %d\n", result);
        goto DISCONNECT;
    }

    // Send a simple http request
    size = strlen(query);
    result = socket->send(query, size);
    if (result != size) {
        printf("Error! socket->send() returned: %d\n", result);
        goto DISCONNECT;
    }

    // Receieve an HTTP response and print out the response line
    while ((result = socket->recv(buffer, 255)) > 0) {
        buffer[result] = 0;
        printf("%s", buffer);
    }
    printf("\n");

    if (result < 0) {
        printf("Error! socket->recv() returned: %d\n", result);
        goto DISCONNECT;
    }


DISCONNECT:
    delete[] buffer;
    // Close the socket to return its memory
    socket->close();
    delete socket;

    // Bring down the network interface
    net->disconnect();
    printf("Done\n");
}
#endif
