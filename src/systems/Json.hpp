#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

struct MapData {
    int width;
    int height;
    std::vector<uint8_t> blocked;
    bool valid;
    
    MapData() : width(0), height(0), valid(false) {}
};

class JsonParser {
public:
    // Cargar mapa desde archivo JSON
    static bool loadMapFromFile(const std::string& path, MapData& out);
    
    // Cargar mapa desde archivo CSV (fallback)
    static bool loadMapFromCSV(const std::string& path, MapData& out);
    
    // Guardar mapa a archivo JSON
    static bool saveMapToFile(const std::string& path, const MapData& data);
    
    // Guardar mapa a archivo CSV
    static bool saveMapToCSV(const std::string& path, const MapData& data);
    
private:
    // Parsing JSON simple (sin dependencias externas)
    static bool parseJsonFile(const std::string& path, MapData& out);
    static bool parseJsonString(const std::string& json, MapData& out);
    static std::string trim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static int parseInt(const std::string& str);
    static std::vector<int> parseIntArray(const std::string& str);
    
    // Validación
    static bool validateMapData(const MapData& data);
    
    // Escritura JSON simple
    static std::string mapDataToJson(const MapData& data);
};
