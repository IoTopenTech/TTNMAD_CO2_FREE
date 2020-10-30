# TTNMAD_CO2_FREE

Ante la creciente evidencia de contagio de la COVID-19 a través de aerosoles, las autoridades recomiendan ventilar los espacios para reducir la concentración de éstos.

Sin embargo, es difícil medir la concentración de aerosoles, por lo que muchos centros están optando por mantener ventanas y puerta permanentemente abiertas.

Con la bajada de temperaturas esto provoca incomodidades a los alumnos/trabajadores y también un aumento del gasto energético en climatización (y su consecuente impacto sobre el medio ambiente).

Estudios como "Risk of indoor airborne infection transmission estimated from carbon dioxide concentration" establecen una correlación entre la concentración de aerosoles y la de CO2.

Afortunadamente la concentración de CO2 puede medirse fácilmente y con precisión usando sensores de bajo coste de tipo Non-Dispersive infrared absorption (NDIR).

Desde la asociación sin ánimo de lucro IoT open Tech nos pusimos manos a la obra para desarrollar un medidor de CO2 DIY fiable, económico y sencillo de construir.

El resultado ha sido TTNMAD_CO2_FREE, que es un dispositivo basado en el microcontrolador ESP8266 y el sensor SCD30, que muestra la concentración de CO2 (además de la temperatura y la humedad) en una pantalla OLED, y utiliza un LED con 3 colores para indicar la necesidad de ventilar cuando la concentración de CO2 supera las 1000 ppm.

![TTNMAD_CO2_FREE](/media/photo_2020-10-29_13-05-56.jpg "TTNMAD_CO2_FREE")

Además, TTNMAD_CO2_FREE puede conectarse a través de WiFi a la plataforma myIoT, en la que se representan los datos y el usuario puede configurar alarmas por mail y Telegram, entre otras.

![TTNMAD_CO2_FREE representado en myIoT](/media/myIoT_TTNMAD_CO2_FREE.jpg "TTNMAD_CO2_FREE representado en myIoT")

En el siguiente vídeo se describe brevemente el proyecto.

[![TTNMAD_CO2_FREE](https://img.youtube.com/vi/RzwtDpLR0ds/0.jpg)](https://www.youtube.com/watch?v=RzwtDpLR0ds)

A continuación se detallan los elementos necesario para constuir el dispositivo (cuyo precio total puede variar entre los 55€ y los 70€ incluyendo gastos de envío e impuestos), y en este mismo repositorio está disponible el programa.

| Componente | Ejemplo de opción de compra | Precio aproximado |
|---|---|---|
|Wemos D1 mini|https://es.aliexpress.com/item/4001291931302.html|2€|
|Sensor SCD30|https://www.digikey.es/product-detail/es/sensirion-ag/SCD30/1649-1098-ND/8445334|30-50€|
|Doble zócalo para Wemos D1|https://es.aliexpress.com/item/1680869855.html|0.5€|
|Pantala OLED para Wemos D1|https://es.aliexpress.com/item/32865961001.html|2€|
|PCB prototipado para Wemos D1|https://es.aliexpress.com/item/32627711647.html|0.5€|
|LED 5mm cátodo común bicolor rojo/verde difuso|https://es.aliexpress.com/item/32974981818.html|0.05€|
|Resistencia 100ohm 1/4W|https://es.aliexpress.com/item/32847096736.html|0.01€|
|Resistencia 10ohm 1/4W|https://es.aliexpress.com/item/32847096736.html|0.01€|

Para su construcción sólo se necesita un soldador de electrónica tipo lápiz y estaño (preferiblemente sin plomo para evitar su toxicidad).

El tiempo aproximado para construirlo es 1 hora.

Adicionalmente hemos diseñado una carcasa 3D que está disponible en https://www.thingiverse.com/thing:4637457

![Carcasa para TTNMAD_CO2_FREE](/media/carcasa3d_ttnmad_co2_free.jpg "Carcasa para TTNMAD_CO2_FREE")

