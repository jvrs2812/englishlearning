# 🇺🇸 English Buddy Bot

Bot de Telegram para praticar inglês conversando com um "amigo americano" (Jake). Usa uma LLM local via Ollama para gerar respostas com feedback sobre o inglês do usuário.

## Features

- 🤖 **Amigo virtual americano** — Conversa natural em inglês
- ✅ **Feedback em cada resposta** — Acertos, erros e sugestões
- 📅 **Mensagem proativa diária** — Jake puxa assunto dentro da janela configurada
- 💾 **Persistência** — Histórico salvo em SQLite para manter contexto
- 🐳 **Totalmente containerizado** — Docker Compose com Ollama embutido

## Arquitetura

C++17 com princípios **S.O.L.I.D**:

- `IMessenger` → `TelegramBot`
- `ILLMService` → `OllamaService`
- `IConversationStore` → `SQLiteConversationStore`
- `IScheduler` → `DailyScheduler`
- `IConfig` → `JsonConfig`

## Quick Start

### 1. Configure o bot

Edite `data/config.json`:

```json
{
    "telegram_token": "SEU_TOKEN_AQUI",
    "ollama_url": "http://ollama:11434",
    "ollama_model": "llama3",
    "schedule_start": "09:00",
    "schedule_end": "10:00",
    "allowed_chat_ids": [123456789],
    "db_path": "/app/data/conversations.db"
}
```

> **Nota**: Para obter o `allowed_chat_ids`, mande uma mensagem ao bot e veja o log. O scheduler só envia mensagens proativas para esses IDs.

### 2. Suba com Docker

```bash
docker compose up --build -d
```

O modelo LLM já vem embutido na imagem — sem necessidade de download adicional.

### 3. Use o bot

- Mande `/start` no Telegram para o bot
- Escreva qualquer coisa em inglês
- Jake vai responder com feedback + continuar a conversa

### Comandos

| Comando | Descrição |
|---------|-----------|
| `/start` | Inicia a conversa com boas-vindas |
| `/clear` | Limpa o histórico de conversa |

## GPU (Opcional)

Para usar GPU NVIDIA, descomente as linhas no `docker-compose.yml`:

```yaml
deploy:
  resources:
    reservations:
      devices:
        - driver: nvidia
          count: all
          capabilities: [gpu]
```

## Desenvolvimento Local (sem Docker)

```bash
# Instale dependências
sudo apt install build-essential cmake libcurl4-openssl-dev libssl-dev \
    zlib1g-dev libboost-system-dev libboost-thread-dev libsqlite3-dev

# Build
mkdir build && cd build
cmake .. && make -j$(nproc)

# Execute
CONFIG_PATH=../data/config.json ./english_buddy_bot
```
