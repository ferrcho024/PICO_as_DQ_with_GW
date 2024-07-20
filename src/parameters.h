
const char *df = "/spiffs/149_df.txt"; // Archivo con los datos del sensor DF
const char *nova = "/spiffs/149_nova.txt"; // Archivo con los datos del sensor NOVA
const char *siata = "/spiffs/80_siata.txt";  // Archivo con los datos de la estación SIATA de referencia

const char *data = "/spiffs/data.txt"; // Archivo de almacenamiento permanente de los datos (fecha, hora y dato)
const char *temp_df = "/spiffs/temp_df.txt"; // Archivo de almacenamiento temporal de los datos DF (solo datos)
const char *temp_nova = "/spiffs/temp_nova.txt"; // Archivo de almacenamiento temporal de los datos DF (solo datos)
const char *dimensions = "/spiffs/dimensions.txt"; // Archivo que almacena las métricas de DQ del sensor cada hora

const char datos_df[60][12] = {"44.46522", "44.46522", "42.39872", "44.46522", "nan", "42.39872", "45.49846",
                                "42.39872", "40.33223", "36.19923", "nan", "nan", "35.16599", "35.16599", "35.16599",
                                "34.13274", "37.23248", "39.29898", "40.33223", "40.33223", "39.29898", "nan", "40.33223",
                                "40.33223", "40.33223", "36.19923", "36.19923", "40.33223", "41.36547", "44.46522", "46.53171",
                                "46.53171", "47.56496", "42.39872", "41.36547", "45.49846", "51.69795", "50.6647", "48.59821",
                                "45.49846", "43.43197", "41.36547", "41.36547", "43.43197", "47.56496", "43.43197", "40.33223",
                                "44.46522", "45.49846", "43.43197", "41.36547", "41.36547", "44.46522", "47.56496", "45.49846",
                                "41.36547", "45.49846", "47.56496", "45.49846", "44.46522"}; 

const char datos_nova[60][12] = {"61.4259", "61.4259", "65.47722", "76.28072", "nan", "64.12678", "93.83641", "96.53729",
                                "76.28072", "65.47722", "nan", "nan", "57.37459", "37.11802", "47.92152", "56.02415",
                                "54.67371", "80.33203", "41.16933", "47.92152", "46.57109", "nan", "50.6224", "51.97284",
                                "57.37459", "135.7000", "45.22065", "35.76758", "39.8189", "65.47722", "68.17809", "64.12678",
                                "72.22941", "61.4259", "58.72503", "61.4259", "83.03291", "108.6912", "60.07546", "49.27196",
                                "62.77634", "64.12678", "60.07546", "57.37459", "56.02415", "64.12678", "51.97284", "53.32328",
                                "57.37459", "57.37459", "62.77634", "50.6224", "41.16933", "43.87021", "46.57109", "42.51977",
                                "45.22065", "62.77634", "114.0930", "107.3408"};

const char dato_siata[12] = "14.2742";