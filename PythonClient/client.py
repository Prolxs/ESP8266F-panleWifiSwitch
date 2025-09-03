import socket

def main():
    # 创建一个 UDP 套接字
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # 服务器地址和端口
    server_address = ('192.168.1.182', 8888)  # 替换为你的服务器地址和端口

 
    # while True:
    # 发送数据
    message = 'pwr=3prst=0RST=0'
    print(f'发送: {message}')
    client_socket.sendto(message.encode(), server_address)

    # 接收响应
    data, server = client_socket.recvfrom(1024)
    print('接收到:', data.decode())


if __name__ == '__main__':
    main()
# import socket

# # 创建 UDP 套接字
# sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# # # 绑定本地端口（可选IP：'' 表示所有接口）
# local_port = 12345
# sock.bind(('', local_port))  # 绑定到 12345 端口

# # 目标地址和端口
# target_ip = "192.168.1.182"
# target_port = 8888

# # 发送数据到目标端口
# sock.sendto(b"pwr=3prst=0RST=0", (target_ip, target_port))

# # 关闭套接字
# sock.close()
