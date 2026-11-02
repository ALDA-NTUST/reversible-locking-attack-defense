FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates git cmake g++ make python3 libboost-filesystem-dev \
    libboost-system-dev zlib1g-dev && rm -rf /var/lib/apt/lists/*
WORKDIR /opt/revc-lock
COPY . .
RUN python3 scripts/setup.py --jobs 2
WORKDIR /work
ENTRYPOINT ["python3", "/opt/revc-lock/revc-tool"]
