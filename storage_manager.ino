


bool initStorage() {
    if (!SPIFFS.begin(true)) {
        Serial.println("Can't connect SPIFFS");
        return false;
    }
    Serial.println("SPIFFS initialized");
    return true;
}

bool writeData(const char* path, const String& data) {
    File file = SPIFFS.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Error opening file");
        return false;
    }
    if (file.print(data)) {
        Serial.println("Data writte");
    } else {
        Serial.println("Data writing error");
    }
    file.close();
    return true;
}

String readData(const char* path) {
    if (!SPIFFS.exists(path)) {
        Serial.println("File is not exist");
        return "";
    }
    File file = SPIFFS.open(path, FILE_READ);
    if (!file) {
        Serial.println("Error opening file");
        return "";
    }
    String data = file.readString();
    file.close();
    return data;
}


void deleteFile(const char* path) {
    if (SPIFFS.exists(path)) {
        if (SPIFFS.remove(path)) {
            Serial.println("File removed");
        } else {
            Serial.println("Error removing file");
        }
    } else {
        Serial.println("File is not exist");
    }
}

