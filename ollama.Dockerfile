FROM ollama/ollama:latest

# Install curl for healthcheck
RUN apt-get update && apt-get install -y --no-install-recommends curl && \
    rm -rf /var/lib/apt/lists/*

# Bake a LIGHTWEIGHT model optimized for CPU
# phi3:mini (~2.3GB) is much faster than llama3 (~4.7GB) on CPU
RUN ollama serve & \
    SERVER_PID=$! && \
    sleep 10 && \
    ollama pull phi3:mini && \
    kill $SERVER_PID && \
    wait $SERVER_PID 2>/dev/null || true
