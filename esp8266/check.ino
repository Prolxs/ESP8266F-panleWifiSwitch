bool extractCredentials(const char* raw, WiFiCredentials* credentials) {
    // 参数检查
    if (raw == NULL || credentials == NULL) {
        Serial.println("错误：无效的输入参数");
        return false;
    }

    // 初始化结构体
    memset(credentials, 0, sizeof(WiFiCredentials));

    String input = raw;
    
    // 查找关键字段位置
    int ssidLabelPos = input.indexOf("ssid=");
    int pwdLabelPos = input.indexOf("pwd=");
    
    // 验证字段是否存在
    if (ssidLabelPos == -1 || pwdLabelPos == -1) {
        Serial.println("错误：未找到ssid或pwd字段");
        return false;
    }
    
    // 计算字段值起始位置
    int ssidStart = ssidLabelPos + 5;  // 跳过"ssid:"
    int pwdStart = pwdLabelPos + 4;    // 跳过"pwd:"
    
    // 提取SSID（从ssid:后到pwd:前）
    String ssid = input.substring(ssidStart, pwdLabelPos);
    
    // 提取密码（从pwd:后到字符串结束）
    String password = input.substring(pwdStart);
    
    // 去除可能的前后空白字符
    ssid.trim();
    password.trim();
    
    // 验证提取结果
    if (ssid.length() == 0 || password.length() == 0) {
        Serial.println("错误：ssid或密码为空");
        return false;
    }

    // 检查长度是否超过缓冲区
    if (ssid.length() >= MAX_SSID_LEN || password.length() >= MAX_PWD_LEN) {
        Serial.println("错误：ssid或密码过长");
        return false;
    }
    
    // 安全拷贝到结构体
    strncpy(credentials->ssid, ssid.c_str(), MAX_SSID_LEN - 1);
    strncpy(credentials->password, password.c_str(), MAX_PWD_LEN - 1);
    
    Serial.print("SSID: ");
    Serial.println(credentials->ssid);
    Serial.print("Password: ");
    Serial.println(credentials->password);
    
    return true;
}

bool parseData(const String &input, WiFiCredentials* credentials) {

  // 解析PWR值
  int pwrStart = input.indexOf("pwr=");
  if (pwrStart != -1) {
    credentials->PWR = input.substring(pwrStart + 4, input.indexOf("PRST=")).toInt();
  }

  // 解析PRST值
  int prstStart = input.indexOf("prst=");
  if (prstStart != -1) {
    credentials->PRST = input.substring(prstStart + 5, input.indexOf("RST=")).toInt();
  }

  // 解析RST值
  int rstStart = input.indexOf("RST=");
  if (rstStart != -1) {
    credentials->RST = input.substring(rstStart + 4).toInt();
  }

  // 验证并输出结果
  if (credentials->PWR != -1 && credentials->PRST != -1 && credentials->RST != -1) {
    Serial.printf("解析成功: PWR=%d, PRST=%d, RST=%d\n", credentials->PWR, credentials->PRST, credentials->RST);
    return true;
  } else {
    Serial.println("解析失败，格式错误");
    return false;
  }
}