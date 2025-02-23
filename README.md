
# Sistema de monitoramento do solo via microcontroladores Esp32, LoRa, NPK Soil Sensor 7x1 e Arduino Uno R3



Sistema capaz de monitorar o solo, com medidas como temperatura, umidade, condutividade elétrica, pH, fósforo, potássio e nitrôgenio via longas distâncias, 12 Km em áreas rurais e até 2 Km em áreas urbadas. Aprentação dos resultados é feita de maneira interativa via um display touch TFT. 

##Componentes essenciais utilizados

Duas placas de Desenvolvimento Wifi Bluetooth ESP32
Dois módulos Transceptor Longo Alcance Lora SX1278 433Mhz
NPKPHCTH-S 7 em 1 RS485
Módulo Conversor de Dados TTL para RS485 com Controle Automático
Placa Uno R3 Compatível Atmega328 SMD com Cabo Usb
Display LCD TFT 2.4" Touchscreen Shield
Módulo Conversor de Nível Lógico Bidirecional
Mini Painel Solar 5V 200mA
Mini Placa Solar Fotovoltaica 18V 277mA 5W 165x220mm
Dois diodos Retificadores Schottky 1A 40V 1N5819(um para cada painel)
Dois suportes para Bateria 18650 1 Slot Pino Rígido
Bateria Li-Íon 3.7V 18650 2500mAh (2 unidades)
Módulo Carregador de Bateria TP4056 - Mini USB
Módulo Carregador de Bateria de Lítio TP4056 com Proteção Usb Tipo C
Módulo XL6009 Regulador de Tensão Step Up Ajustável DC-DC


## Modulo transmissor


![circuito_basico](https://github.com/user-attachments/assets/be7079a1-0018-49fd-9312-d074f9905291)

O modulo transmissor consiste na captação das medidas do sensor NPKPHCTH-S 7 em 1, conversão via Módulo Conversor de Dados TTL, recepção do sinais via comunicação serial e transmissão via LoRa.
O modulo transmissor também tem uma alimentação solar autonôma com duas batérias de lítio sendo carregadas por dois paíneis fotovotaícos(um para cada batéria), assim como regulares de tensão apropriado para o caso do paínel de 18V.



## Modo receptor



![modulo receptor](https://github.com/user-attachments/assets/1ab9d6b4-26ea-485a-aa82-688e10768782)



O módulo receptor consiste na recepção dos sinais via um microcontrolador Esp32 com um módulo LoRa, assim como no pré-processamento implementado por parser. Logo em seguida as medidas são proporcionadas interativamente para o usuário através de um display TFT paralelo via uma placa Arduino Uno R3. O envio dos dados para o arduino foi feita através da comunicação serial entre o Arduino e Esp32. Não escolheu-se usar apenas uma placa Esp32 para fazer tudo(recepção e apresentação no display) devido as dificuldades do Esp32 lidar com as funcionalidades de toque do display TFT touch, dificuldade que o arduino não apresenta. 





