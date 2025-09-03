import socket

def main():
    # 创建一个 UDP 套接字
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # 绑定套接字到地址和端口
    server_address = ('0.0.0.0', 6543)  # 替换为你想要的地址和端口
    server_socket.bind(server_address)

    print('等待连接...')

    while True:
        # 接收数据
        data, client_address = server_socket.recvfrom(1024)
        print('连接来自:', client_address)
        
        message = data.decode()
        print('接收到:', message)
        
        # 如果收到close命令，则关闭服务器
        if message.lower() == 'close':
            print('客户端请求关闭连接')
            break
            
        # 发送响应
        response = '服务器已收到消息: ' + message
        server_socket.sendto(response.encode(), client_address)
        
    # 关闭套接字
    server_socket.close()
    print('服务器已关闭')

if __name__ == '__main__':
    main()