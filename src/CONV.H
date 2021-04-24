void calcular_paleta_gris(imagen::regpal *paleta);

void calcular_paleta_rgb8(imagen::regpal *paleta);

int conv8_8g(imagen *origen,imagen *(&destino));

int conv8_24(imagen *origen,imagen *(&destino));

int conv24_8g(imagen *origen,imagen *(&destino));

int conv24_8(imagen *origen,imagen *(&destino));

int conv8g_1fs(imagen *origen,imagen *(&destino));