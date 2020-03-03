"""
A simple Python script to receive messages from a client over
Bluetooth using Python sockets (with Python 3.3 or above).
"""
import bluetooth
server_socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

port = 1
server_socket.bind(("", port))
server_socket.listen(1)

client_socket, address = server_socket.accept()
print("Accepted connection from {}".format(address))
while 1:
    data = client_socket.recv(1024)
    print(data)


'''
import socket

hostMACAddress = 'B4:BF:F6:2D:53:3C' # The MAC address of a Bluetooth adapter on the server. The server might have multiple Bluetooth adapters.
port = 1 # 3 is an arbitrary choice. However, it must match the port used by the client.
backlog = 1
size = 1024
s = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)
s.bind((hostMACAddress,port))
s.listen(backlog)
try:
    client, address = s.accept()
    print("Entro")
    while 1:
        data = client.recv(size)
        if data:
            print(data)
            client.send(data)
except:
    print("Closing socket")
    client.close()
    s.close()
    
    
    
    def _accept_and_send(self):
        server_sock = bt.BluetoothSocket(bt.RFCOMM)
        server_sock.bind(("", bt.PORT_ANY))
        server_sock.listen(1)

        self.fields = self.fields

        self.port = server_sock.getsockname()[1]
        self.queue_lock = threading.Lock()
        try:
            bt.advertise_service(server_sock, "RPi-Logger",
                                 service_id=self.uuid,
                                 service_classes=[self.uuid, bt.SERIAL_PORT_CLASS],
                                 profiles=[bt.SERIAL_PORT_PROFILE],
                                 )
        except bt.BluetoothError as e:
            logging.warning("Failed to start bluetooth: {}".format(e))
            import _thread
            _thread.interrupt_main()

        self.server_sock = server_sock
        print("Waiting for connection on RFCOMM channel {}".format(self.port))
        self.client_sock, client_info = self.server_sock.accept()
        logging.warning("Accepted connection from: {}".format(client_info))
        self.client_sock.settimeout(2.0)
        self.client_sock.send("$RPI-CAN-LOGGER!\n")
        # self.send("$ip={}".format(get_ip()))
        while 1:
            connected = self._is_connected()
            if self._finished:
                return
            if connected:
                # Try and receive for a little bit
                received = None
                try:
                    received = self.client_sock.recv(512)
                except bt.BluetoothError as e:
                    pass
                if received:
                    received = received.decode('ascii', 'ignore')
                    logging.warning("BTR> {}".format(received))
                    self.recv_queue.append(received)
                while len(self.queue) > 0:
                    msg = self.queue.popleft()
                    if msg:
                        try:
                            if not self.exporting:
                                self.client_sock.send("{}!\n".format(msg.strip()))
                        except bt.BluetoothError as e:
                            pass
            else:
                print("Disconnected from {}".format(client_info))
                break 
'''