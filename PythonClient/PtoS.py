from PIL import Image, ImageDraw, ImageFont
import numpy as np

def generate_lcd_bitmap(text, font_size=16, font_name="simsun.ttc"):
    if not text:
        return

    # 加载字体
    try:
        font = ImageFont.truetype(font_name, font_size)
    except IOError:
        print(f"错误：找不到字体文件 '{font_name}'，请检查路径！")
        return

    # 获取文本边界框
    left, top, right, bottom = font.getbbox(text)
    width = right - left
    height = bottom - top

    # 创建图像并绘制文本
    image = Image.new("1", (width, height), 1)  # 1表示白色背景
    draw = ImageDraw.Draw(image)
    draw.text((-left, -top), text, font=font, fill=0)  # 0表示黑色文本

    # 转换为二进制矩阵
    bitmap = np.array(image)
    bitmap = np.where(bitmap == 0, 1, 0)  # 反色（0为点亮）

    # 根据 PCtoLCD 的要求生成字节流
    byte_data = []
    for y in range(0, height, 8):  # 每8行生成一个字节
        row = []
        for x in range(width):
            byte = 0
            for bit in range(8):
                if y + bit < height:  # 确保不超出高度
                    byte |= (bitmap[y + bit, x] << (7 - bit))  # 高位在前
            row.append(byte)
        byte_data.append(row)

    # 格式化输出为 font_data[][]
    output = "/* 字体: {} 字号: {} */\n".format(font_name, font_size)
    output += "const uint8_t font_data[][{}] = {{\n".format(len(byte_data[0]))  # 每行的字节数
    for i, row in enumerate(byte_data):
        output += "    {"
        for j, byte in enumerate(row):
            output += "0x{:02X}".format(byte)
            if j < len(row) - 1:
                output += ", "
        output += "}"
        if i < len(byte_data) - 1:
            output += ",\n"
    output += "\n};\n"
    print(output)

if __name__ == '__main__':
    generate_lcd_bitmap("地址", font_size=16)