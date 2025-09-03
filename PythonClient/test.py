# 假设乱码字符串是 UTF-8 被错误解码为 Latin-1
bad_text = "�ϵ��豸û�з������á�"
bytes_data = bad_text.encode('gbk')  # 逆向还原错误编码的字节
correct_text = bytes_data.decode('utf-8')  # 用正确的 UTF-8 解码
print(correct_text)  # 输出应为正常中文