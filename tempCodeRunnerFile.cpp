void renderBackground(const Texture2D *texture)
{
    Rectangle textureArea = {
        0.0f, 0.0f,
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    Rectangle destinationArea = {
        ORIGIN.x,
        ORIGIN.y,
        static_cast<float>(SCREEN_WIDTH),
        static_cast<float>(SCREEN_HEIGHT)
    };

    Vector2 originOffset = {
        static_cast<float>(SCREEN_WIDTH) / 2.0f,
        static_cast<float>(SCREEN_HEIGHT) / 2.0f
    };

    DrawTexturePro(
        *texture,
        textureArea, destinationArea, originOffset,
        0.0f, WHITE
    );
}