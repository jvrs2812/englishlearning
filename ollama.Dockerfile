FROM ollama/ollama:latest

# Install curl for healthcheck
RUN apt-get update && apt-get install -y --no-install-recommends curl && \
    rm -rf /var/lib/apt/lists/*

# Bake the LLM model into the image at build time
# Start ollama in the background, pull the model, then stop
RUN ollama serve & \
    SERVER_PID=$! && \
    sleep 10 && \
    ollama pull llama3 && \
    kill $SERVER_PID && \
    wait $SERVER_PID 2>/dev/null || true
