# Monitoramento Inteligente de Estufa com IoT 

## 1. Descrição do Projeto
Este projeto consiste em uma solução de monitoramento inteligente para uma estufa, desenvolvida como trabalho prático para a FIAP. O sistema realiza a coleta contínua do clima interno (temperatura e umidade) e do nível do reservatório de água. Os dados capturados pelos sensores físicos são transmitidos de forma segura para um servidor, processados, cruzados com dados meteorológicos de uma API externa e exibidos em um dashboard interativo. Para análise de histórico, todos os dados são persistidos em um banco de dados hospedado na nuvem.

## 2. Arquitetura da Solução
A arquitetura do projeto foi desenhada para garantir um fluxo de dados contínuo, seguro e escalável:
* **Aquisição de Dados:** Um microcontrolador ESP32 atua como o cérebro físico, realizando as leituras periódicas através dos pinos D27 (DHT11), D32 (Ultrassônico Trig) e D33 (Ultrassônico Echo).
* **Comunicação Segura (IoT):** A placa se conecta a uma rede corporativa e publica os dados empacotados via protocolo MQTT com criptografia TLS/SSL (porta 8883) e autenticação por usuário e senha.
* **Processamento e Integração:** O Node-RED atua como o middleware da aplicação, recebendo as métricas físicas e consumindo a API pública do OpenWeather.
* **Visualização e Persistência na Nuvem:** O Node-RED orquestra a exibição no dashboard em tempo real e executa os comandos SQL para registrar os valores diretamente em um servidor MySQL hospedado no Clever Cloud.

## 3. Tecnologias Utilizadas
**Hardware:**
* Placa ESP32
* Sensor de Temperatura e Umidade (DHT11)
* Sensor Ultrassônico (HC-SR04)

**Software & Protocolos:**
* **C++ (Arduino IDE):** Desenvolvimento do firmware do ESP32 com `WiFiClientSecure`.
* **MQTT:** Protocolo de mensageria com suporte a TLS/SSL.
* **Node-RED:** Fluxo de integração, processamento de dados e criação do Dashboard.
* **MySQL (Clever Cloud):** Banco de dados relacional em nuvem (DBaaS) para armazenamento das métricas.
* **OpenWeather API:** Fornecimento de dados climáticos externos.

## 4. Instruções de Execução

Para replicar este projeto, siga os passos abaixo:

### Passo 1: Preparando o Banco de Dados (Nuvem)
1. Crie um banco de dados MySQL gratuito no [Clever Cloud](https://www.clever-cloud.com/).
2. Copie as credenciais geradas (Host, User, Password, Database Name).
3. Acesse o **phpMyAdmin** pelo painel do Clever Cloud e execute o script abaixo na aba SQL:

```sql
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
3. **API Clima:** Dê um duplo clique no nó da API OpenWeather e insira a sua chave válida no link.
4. **Banco de Dados:** Edite a configuração do nó do MySQL e insira as credenciais que você copiou do Clever Cloud.
5. **Broker MQTT:** Edite os nós de entrada MQTT, configurando a porta para `8883`, marcando a opção **Use TLS/SSL** e inserindo o usuário/senha na aba *Security*.
6. Clique em **Deploy**. O dashboard estará disponível em `http://localhost:1880/ui`.

### Passo 3: Configurando o Hardware e Montagem
1. Faça as ligações na protoboard:
   * **DHT11:** VCC no 3.3V, GND no GND e Dados no pino **D27** (necessário resistor de pull-up de 10k ohms entre VCC e Dados).
   * **HC-SR04:** VCC no VIN (5V), GND no GND, TRIG no **D32** e ECHO no **D33**.
2. Abra o arquivo do código na Arduino IDE e instale as bibliotecas `PubSubClient` e `DHT sensor library`.
3. Insira as credenciais da rede Wi-Fi e do seu Broker MQTT (usuário e senha) no cabeçalho do código.
4. Selecione a porta COM e faça o upload para o ESP32.
