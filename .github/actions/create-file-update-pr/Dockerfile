FROM alpine:latest

RUN apk add --no-cache \
    bash \
    jq \
    git \
    ca-certificates \
    curl

COPY entrypoint.sh /entrypoint.sh

LABEL name="create-file-pr"
LABEL version="1.0.1"

ENTRYPOINT ["/entrypoint.sh"]
