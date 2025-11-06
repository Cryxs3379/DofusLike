#include "systems/Json.hpp"
#include <algorithm>
#include <cctype>

bool JsonParser::loadMapFromFile(const std::string& path, MapData& out) {
    // Intentar cargar JSON primero
    if (parseJsonFile(path, out)) {
        if (validateMapData(out)) {
            std::cout << "Mapa cargado desde JSON: " << path << " (" << out.width << "x" << out.height << ")" << std::endl;
            return true;
        } else {
            std::cout << "Error: Datos del mapa JSON inválidos" << std::endl;
        }
    }
    
    // Fallback a CSV
    std::string csvPath = path;
    size_t lastDot = csvPath.find_last_of('.');
    if (lastDot != std::string::npos) {
        csvPath = csvPath.substr(0, lastDot) + ".csv";
    } else {
        csvPath += ".csv";
    }
    
    if (loadMapFromCSV(csvPath, out)) {
        if (validateMapData(out)) {
            std::cout << "Mapa cargado desde CSV: " << csvPath << " (" << out.width << "x" << out.height << ")" << std::endl;
            return true;
        } else {
            std::cout << "Error: Datos del mapa CSV inválidos" << std::endl;
        }
    }
    
    std::cout << "Error: No se pudo cargar el mapa desde " << path << std::endl;
    return false;
}

bool JsonParser::loadMapFromCSV(const std::string& path, MapData& out) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    std::vector<std::vector<int>> grid;
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::vector<int> row;
        std::stringstream ss(line);
        std::string cell;
        
        while (std::getline(ss, cell, ',')) {
            cell = trim(cell);
            if (!cell.empty()) {
                row.push_back(parseInt(cell));
            }
        }
        
        if (!row.empty()) {
            grid.push_back(row);
        }
    }
    
    if (grid.empty()) {
        return false;
    }
    
    out.height = static_cast<int>(grid.size());
    out.width = static_cast<int>(grid[0].size());
    out.blocked.clear();
    
    for (const auto& row : grid) {
        for (int cell : row) {
            out.blocked.push_back(static_cast<uint8_t>(cell));
        }
    }
    
    out.valid = true;
    return true;
}

bool JsonParser::saveMapToFile(const std::string& path, const MapData& data) {
    if (!data.valid) {
        std::cout << "Error: Datos del mapa inválidos para guardar" << std::endl;
        return false;
    }
    
    std::string json = mapDataToJson(data);
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cout << "Error: No se pudo abrir archivo para escribir: " << path << std::endl;
        return false;
    }
    
    file << json;
    file.close();
    
    std::cout << "Mapa guardado en: " << path << " (" << data.width << "x" << data.height << ")" << std::endl;
    return true;
}

bool JsonParser::saveMapToCSV(const std::string& path, const MapData& data) {
    if (!data.valid) {
        std::cout << "Error: Datos del mapa inválidos para guardar" << std::endl;
        return false;
    }
    
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cout << "Error: No se pudo abrir archivo CSV para escribir: " << path << std::endl;
        return false;
    }
    
    for (int y = 0; y < data.height; ++y) {
        for (int x = 0; x < data.width; ++x) {
            if (x > 0) file << ",";
            file << static_cast<int>(data.blocked[y * data.width + x]);
        }
        file << "\n";
    }
    
    file.close();
    std::cout << "Mapa guardado en CSV: " << path << " (" << data.width << "x" << data.height << ")" << std::endl;
    return true;
}

bool JsonParser::parseJsonFile(const std::string& path, MapData& out) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    std::string json((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    return parseJsonString(json, out);
}

bool JsonParser::parseJsonString(const std::string& json, MapData& out) {
    // Parser JSON muy simple para nuestro formato específico
    std::string cleanJson = json;
    
    // Remover espacios y saltos de línea
    cleanJson.erase(std::remove_if(cleanJson.begin(), cleanJson.end(), 
        [](char c) { return std::isspace(c); }), cleanJson.end());
    
    // Buscar width
    size_t widthPos = cleanJson.find("\"width\":");
    if (widthPos == std::string::npos) return false;
    
    size_t widthStart = cleanJson.find(':', widthPos) + 1;
    size_t widthEnd = cleanJson.find_first_of(",}", widthStart);
    if (widthEnd == std::string::npos) return false;
    
    std::string widthStr = cleanJson.substr(widthStart, widthEnd - widthStart);
    out.width = parseInt(widthStr);
    
    // Buscar height
    size_t heightPos = cleanJson.find("\"height\":");
    if (heightPos == std::string::npos) return false;
    
    size_t heightStart = cleanJson.find(':', heightPos) + 1;
    size_t heightEnd = cleanJson.find_first_of(",}", heightStart);
    if (heightEnd == std::string::npos) return false;
    
    std::string heightStr = cleanJson.substr(heightStart, heightEnd - heightStart);
    out.height = parseInt(heightStr);
    
    // Buscar blocked array
    size_t blockedPos = cleanJson.find("\"blocked\":[");
    if (blockedPos == std::string::npos) return false;
    
    size_t arrayStart = cleanJson.find('[', blockedPos) + 1;
    size_t arrayEnd = cleanJson.find(']', arrayStart);
    if (arrayEnd == std::string::npos) return false;
    
    std::string arrayStr = cleanJson.substr(arrayStart, arrayEnd - arrayStart);
    std::vector<int> blockedInts = parseIntArray(arrayStr);
    
    out.blocked.clear();
    for (int val : blockedInts) {
        out.blocked.push_back(static_cast<uint8_t>(val));
    }
    
    out.valid = true;
    return true;
}

std::string JsonParser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> JsonParser::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    
    return tokens;
}

int JsonParser::parseInt(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (...) {
        return 0;
    }
}

std::vector<int> JsonParser::parseIntArray(const std::string& str) {
    std::vector<int> result;
    std::vector<std::string> tokens = split(str, ',');
    
    for (const std::string& token : tokens) {
        if (!token.empty()) {
            result.push_back(parseInt(token));
        }
    }
    
    return result;
}

bool JsonParser::validateMapData(const MapData& data) {
    if (data.width <= 0 || data.height <= 0) {
        std::cout << "Error: Dimensiones inválidas (" << data.width << "x" << data.height << ")" << std::endl;
        return false;
    }
    
    int expectedSize = data.width * data.height;
    if (static_cast<int>(data.blocked.size()) != expectedSize) {
        std::cout << "Error: Tamaño de array blocked incorrecto. Esperado: " << expectedSize 
                  << ", Obtenido: " << data.blocked.size() << std::endl;
        return false;
    }
    
    return true;
}

std::string JsonParser::mapDataToJson(const MapData& data) {
    std::stringstream json;
    json << "{\n";
    json << "  \"width\": " << data.width << ",\n";
    json << "  \"height\": " << data.height << ",\n";
    json << "  \"blocked\": [\n";
    
    for (size_t i = 0; i < data.blocked.size(); ++i) {
        if (i > 0) {
            if (i % data.width == 0) {
                json << ",\n";
            } else {
                json << ",";
            }
        }
        
        if (i % data.width == 0) {
            json << "    ";
        }
        
        json << static_cast<int>(data.blocked[i]);
    }
    
    json << "\n  ]\n";
    json << "}\n";
    
    return json.str();
}
