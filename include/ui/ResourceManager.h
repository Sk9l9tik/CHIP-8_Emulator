#ifndef CHIP_8_EMULATOR_RESOURCEMANAGER_HPP
#define CHIP_8_EMULATOR_RESOURCEMANAGER_HPP

#include <SFML/Graphics.hpp>

// Мне лень тут синглтонить чет если честно, можете сами переписать
class ResourceManager{
public:
    static void set_font(sf::Font& new_font) { default_font = new_font; }
    static sf::Font& get_font() { return default_font; }

    static sf::Texture* load_texture(const std::string& name, sf::Texture texture){
        return &(textures[name] = std::move(texture)); // 0_0
    }

    static sf::Texture* load_texture(const std::string& name, const std::string& filename) {
        sf::Texture texture;
        if (texture.loadFromFile(filename)) {
            textures[name] = std::move(texture);
            return &textures[name];
        }
        return nullptr;
    }

    static const sf::Texture* get_texture(const std::string& name) {
        auto it = textures.find(name);
        if (it != textures.end()) {
            return &it->second;
        }
        return nullptr;
    }
private:
    static inline sf::Font default_font;

    static inline std::unordered_map<std::string, sf::Texture> textures;

    //
};

#endif //CHIP_8_EMULATOR_RESOURCEMANAGER_HPP
