## 1. Descrição do Projeto
Este projeto consiste em uma solução de monitoramento inteligente para uma estufa, desenvolvida como trabalho prático para a faculdade. O sistema realiza a coleta contínua do clima interno (temperatura e umidade) e do nível do reservatório de água. Os dados capturados pelos sensores físicos são transmitidos em tempo real para um servidor, processados, cruzados com dados meteorológicos da rua e exibidos em um dashboard interativo, além de serem salvos para análise histórica.

## 2. Arquitetura da Solução
A arquitetura do projeto foi desenhada para garantir um fluxo de dados contínuo e escalável de ponta a ponta:
* **Aquisição de Dados:** Um microcontrolador ESP32 atua como o cérebro físico, realizando as leituras periódicas dos sensores de temperatura/umidade e de distância (nível de água).
* **Comunicação:** Os dados são empacotados e publicados através do protocolo MQTT em tópicos organizados no broker público HiveMQ.
* **Processamento e Integração:** O Node-RED atua como o middleware da aplicação, assinando os tópicos MQTT para receber as métricas físicas. Simultaneamente, ele realiza requisições HTTP para a API pública do OpenWeather para consumir a temperatura externa.
* **Visualização e Persistência:** O Node-RED orquestra a exibição de todas essas informações no dashboard em tempo real e executa os comandos SQL para registrar os valores no banco de dados MySQL.

## 3. Tecnologias Utilizadas
**Hardware:**
* Placa ESP32
* Sensor de Temperatura e Umidade (DHT11/DHT22)
* Sensor Ultrassônico (HC-SR04)

**Software & Protocolos:**
* **C++ (Arduino IDE):** Desenvolvimento do firmware do ESP32.
* **MQTT (HiveMQ):** Protocolo de comunicação IoT.
* **Node-RED:** Fluxo de integração, processamento de dados e criação da interface visual (Dashboard).
* **MySQL:** Banco de dados relacional para armazenamento persistente das métricas.
* **OpenWeather API:** Fornecimento de dados climáticos externos.

## 4. Instruções de Execução

Para replicar e rodar este projeto na sua máquina local, siga os passos abaixo:

### Passo 1: Preparando o Banco de Dados
1. Inicie o seu servidor MySQL (via XAMPP ou Docker).
2. Execute o script abaixo para criar a estrutura necessária:

```sql
CREATE DATABASE iot_monitoramento;
USE iot_monitoramento;
CREATE TABLE leituras (
    id INT AUTO_INCREMENT PRIMARY KEY,
    topico VARCHAR(50) NOT NULL,
    valor FLOAT NOT NULL,
    data_leitura TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

### Passo 2: Configurando o Node-RED
1. Inicie o Node-RED na sua máquina.
2. Importe o arquivo JSON do fluxo (`fluxo_nodered.json`) disponível neste repositório.
3. Dê um duplo clique no nó da **API OpenWeather** e insira a sua chave de API válida no link.
4. Dê um duplo clique no nó do **MySQL** e confirme se as credenciais (`host`, `usuário`, `senha`) correspondem ao seu banco local.
5. Clique em **Deploy**. O dashboard estará disponível em `http://localhost:1880/ui`.

### Passo 3: Configurando o Hardware (ESP32)
1. Abra o arquivo `codigo_esp32.ino` na Arduino IDE.
2. Instale as bibliotecas `PubSubClient`, `DHT sensor library` e `Adafruit Unified Sensor`.
3. Altere as variáveis `ssid` e `password` com os dados da sua rede Wi-Fi.
4. Conecte o ESP32 ao computador e faça o upload do código.
