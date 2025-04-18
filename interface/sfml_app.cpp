#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(
        sf::VideoMode::getDesktopMode(),
        "Bandeco Simulator",
        sf::Style::Fullscreen);

    // Carrega a textura da imagem
    sf::Texture texture;
    if (!texture.loadFromFile("../Spritesheets/ChefTools.png")) // Substitua pelo caminho correto
        return -1; // Retorna erro se a imagem não for carregada

    // Dimensões de cada subimagem no sprite sheet
    const int frameCount = 9; // Número total de subimagens
    const int frameWidth = texture.getSize().x / frameCount; // Largura de cada subimagem
    const int frameHeight = texture.getSize().y;            // Altura total da imagem

    // Define o recorte para a 6ª subimagem (índice 5, pois começa em 0)
    sf::IntRect frameRect(frameWidth * 5, 0, frameWidth, frameHeight);

    // Cria um sprite e aplica o recorte
    sf::Sprite sprite(texture, frameRect);

    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                window.close();

        window.clear(sf::Color(246, 241, 230)); // Fundo bege
        window.draw(sprite); // Desenha a subimagem
        window.display();
    }
    return 0;
}